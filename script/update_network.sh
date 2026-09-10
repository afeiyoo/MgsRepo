#!/usr/bin/env bash

# Configure one Ethernet interface through NetworkManager.
# Usage:
#   update_network.sh <interface> <ipv4> <netmask-or-prefix> <gateway> <dns1> <dns2>

set -Eeuo pipefail

readonly EXIT_USAGE=2
readonly EXIT_NOT_ROOT=10
readonly EXIT_DEPENDENCY=11
readonly EXIT_INVALID_ARGUMENT=12
readonly EXIT_INTERFACE=13
readonly EXIT_CONNECTION=20
readonly EXIT_MODIFY=30
readonly EXIT_ACTIVATE=31
readonly EXIT_VERIFY=32
readonly LOG_TAG='update_network'

log()
{
    printf '[%s] %s\n' "${LOG_TAG}" "$*"
}

fail()
{
    local exit_code="$1"
    shift
    printf '[%s] %s\n' "${LOG_TAG}" "$*" >&2
    exit "${exit_code}"
}

usage()
{
    printf 'Usage: %s <interface> <ipv4> <netmask-or-prefix> <gateway> <dns1> <dns2>\n' "$0" >&2
}

is_ipv4()
{
    local address="$1"
    local octet
    local -a octets

    [[ "${address}" =~ ^([0-9]{1,3}\.){3}[0-9]{1,3}$ ]] || return 1
    IFS='.' read -r -a octets <<< "${address}"
    [[ "${#octets[@]}" -eq 4 ]] || return 1

    for octet in "${octets[@]}"; do
        ((10#${octet} >= 0 && 10#${octet} <= 255)) || return 1
    done
}

ipv4_to_integer()
{
    local address="$1"
    local a b c d

    IFS='.' read -r a b c d <<< "${address}"
    printf '%u\n' "$(((10#${a} << 24) | (10#${b} << 16) | (10#${c} << 8) | 10#${d}))"
}

netmask_to_prefix()
{
    local netmask="$1"
    local mask_integer
    local bit
    local bit_is_one
    local met_zero=0
    local prefix_value=0

    if [[ "${netmask}" =~ ^[0-9]{1,2}$ ]]; then
        ((10#${netmask} >= 0 && 10#${netmask} <= 32)) || return 1
        printf '%u\n' "$((10#${netmask}))"
        return 0
    fi

    is_ipv4 "${netmask}" || return 1
    mask_integer="$(ipv4_to_integer "${netmask}")"

    for ((bit = 31; bit >= 0; --bit)); do
        bit_is_one=$(((mask_integer >> bit) & 1))
        if ((bit_is_one == 1)); then
            ((met_zero == 0)) || return 1
            ((prefix_value += 1))
        else
            met_zero=1
        fi
    done

    printf '%u\n' "${prefix_value}"
}

is_uuid()
{
    [[ "$1" =~ ^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$ ]]
}

connection_property()
{
    local property="$1"
    local connection_uuid="$2"

    nmcli -g "${property}" connection show "${connection_uuid}" 2>/dev/null | tr -d '\r' || true
}

rollback_connection()
{
    local rollback_failed=0

    log '新网络配置应用失败，正在恢复原连接配置'
    set +e

    if [[ "${connection_created}" -eq 1 ]]; then
        nmcli connection delete "${connection_uuid}" >/dev/null 2>&1 || rollback_failed=1
    else
        nmcli connection modify "${connection_uuid}" \
            connection.interface-name "${old_interface_name}" \
            connection.autoconnect "${old_autoconnect}" \
            ipv4.method "${old_method}" \
            ipv4.addresses "${old_addresses}" \
            ipv4.gateway "${old_gateway}" \
            ipv4.dns "${old_dns}" \
            ipv4.ignore-auto-dns "${old_ignore_auto_dns}" \
            >/dev/null 2>&1 || rollback_failed=1

        if [[ "${connection_was_active}" -eq 1 ]]; then
            nmcli --wait 30 connection up "${connection_uuid}" ifname "${interface_name}" \
                >/dev/null 2>&1 || rollback_failed=1
        fi
    fi

    set -e

    if [[ "${rollback_failed}" -eq 0 ]]; then
        log '原连接配置已恢复'
        return 0
    fi

    printf '[%s] 自动恢复原连接配置失败，请在本机检查网络配置\n' "${LOG_TAG}" >&2
    return 1
}

fail_after_rollback()
{
    local exit_code="$1"
    shift
    local message="$*"

    printf '[%s] %s\n' "${LOG_TAG}" "${message}" >&2
    rollback_connection || true
    exit "${exit_code}"
}

if [[ "$#" -ne 6 ]]; then
    usage
    exit "${EXIT_USAGE}"
fi

readonly interface_name="$1"
readonly ip_address="$2"
readonly netmask="$3"
readonly gateway="$4"
readonly dns1="$5"
readonly dns2="$6"

[[ "${EUID}" -eq 0 ]] || fail "${EXIT_NOT_ROOT}" '需要root权限才能修改网络配置'

for command_name in nmcli grep head tr sleep; do
    command -v "${command_name}" >/dev/null 2>&1 \
        || fail "${EXIT_DEPENDENCY}" "系统缺少命令：${command_name}"
done

[[ "${interface_name}" =~ ^[a-zA-Z0-9_.:-]+$ ]] \
    || fail "${EXIT_INVALID_ARGUMENT}" "网卡名称不合法：${interface_name}"
[[ -d "/sys/class/net/${interface_name}" ]] \
    || fail "${EXIT_INTERFACE}" "网卡不存在：${interface_name}"

interface_type="$(nmcli -g GENERAL.TYPE device show "${interface_name}" 2>/dev/null | head -n 1 | tr -d '\r' || true)"
[[ "${interface_type}" == 'ethernet' ]] \
    || fail "${EXIT_INTERFACE}" "只支持NetworkManager管理的以太网卡：${interface_name}，当前类型：${interface_type:-未知}"

is_ipv4 "${ip_address}" || fail "${EXIT_INVALID_ARGUMENT}" "IPv4地址不合法：${ip_address}"
is_ipv4 "${gateway}" || fail "${EXIT_INVALID_ARGUMENT}" "网关地址不合法：${gateway}"
is_ipv4 "${dns1}" || fail "${EXIT_INVALID_ARGUMENT}" "DNS地址不合法：${dns1}"
is_ipv4 "${dns2}" || fail "${EXIT_INVALID_ARGUMENT}" "DNS地址不合法：${dns2}"

prefix=''
if ! prefix="$(netmask_to_prefix "${netmask}")"; then
    fail "${EXIT_INVALID_ARGUMENT}" "子网掩码不合法：${netmask}"
fi
readonly prefix

ip_integer="$(ipv4_to_integer "${ip_address}")"
gateway_integer="$(ipv4_to_integer "${gateway}")"
if ((10#${prefix} == 32)); then
    fail "${EXIT_INVALID_ARGUMENT}" '配置默认网关时不支持/32地址'
elif ((10#${prefix} > 0)); then
    network_mask=$(((0xFFFFFFFF << (32 - 10#${prefix})) & 0xFFFFFFFF))
    if (((ip_integer & network_mask) != (gateway_integer & network_mask))); then
        fail "${EXIT_INVALID_ARGUMENT}" \
            "网关${gateway}与IP地址${ip_address}/${prefix}不在同一子网"
    fi
fi

nmcli general status >/dev/null 2>&1 \
    || fail "${EXIT_DEPENDENCY}" 'NetworkManager当前不可用'

connection_uuid="$(nmcli -g GENERAL.CON-UUID device show "${interface_name}" 2>/dev/null | head -n 1 | tr -d '\r' || true)"
connection_was_active=0
connection_created=0

if is_uuid "${connection_uuid}"; then
    connection_was_active=1
else
    active_connection_name="$(nmcli -g GENERAL.CONNECTION device show "${interface_name}" 2>/dev/null | head -n 1 | tr -d '\r' || true)"
    if [[ -n "${active_connection_name}" && "${active_connection_name}" != '--' ]]; then
        connection_uuid="$(nmcli -g connection.uuid connection show "${active_connection_name}" 2>/dev/null | head -n 1 | tr -d '\r' || true)"
        is_uuid "${connection_uuid}" && connection_was_active=1
    fi
fi

if ! is_uuid "${connection_uuid}"; then
    connection_name="mgs-network-${interface_name}"
    connection_uuid="$(nmcli -g connection.uuid connection show "${connection_name}" 2>/dev/null | head -n 1 | tr -d '\r' || true)"

    if ! is_uuid "${connection_uuid}"; then
        log "网卡${interface_name}没有可用连接配置，正在创建${connection_name}"
        nmcli connection add type ethernet ifname "${interface_name}" con-name "${connection_name}" autoconnect no \
            >/dev/null 2>&1 \
            || fail "${EXIT_CONNECTION}" "为网卡${interface_name}创建连接配置失败"

        connection_uuid="$(nmcli -g connection.uuid connection show "${connection_name}" 2>/dev/null | head -n 1 | tr -d '\r' || true)"
        is_uuid "${connection_uuid}" \
            || fail "${EXIT_CONNECTION}" "已创建连接${connection_name}，但无法获取其UUID"
        connection_created=1
    fi
fi

connection_name="$(connection_property connection.id "${connection_uuid}")"
log "使用连接配置：${connection_name:-${connection_uuid}}，网卡：${interface_name}"

old_interface_name=''
old_autoconnect='no'
old_method='auto'
old_addresses=''
old_gateway=''
old_dns=''
old_ignore_auto_dns='no'

if [[ "${connection_created}" -eq 0 ]]; then
    old_interface_name="$(connection_property connection.interface-name "${connection_uuid}")"
    old_autoconnect="$(connection_property connection.autoconnect "${connection_uuid}")"
    old_method="$(connection_property ipv4.method "${connection_uuid}")"
    old_addresses="$(connection_property ipv4.addresses "${connection_uuid}")"
    old_gateway="$(connection_property ipv4.gateway "${connection_uuid}")"
    old_dns="$(connection_property ipv4.dns "${connection_uuid}")"
    old_ignore_auto_dns="$(connection_property ipv4.ignore-auto-dns "${connection_uuid}")"
fi

log "设置IPv4地址${ip_address}/${prefix}，网关${gateway}，DNS ${dns1},${dns2}"
if ! nmcli connection modify "${connection_uuid}" \
    connection.interface-name "${interface_name}" \
    connection.autoconnect yes \
    ipv4.method manual \
    ipv4.addresses "${ip_address}/${prefix}" \
    ipv4.gateway "${gateway}" \
    ipv4.dns "${dns1},${dns2}" \
    ipv4.ignore-auto-dns yes; then
    fail_after_rollback "${EXIT_MODIFY}" '写入NetworkManager连接配置失败'
fi

if ! nmcli --wait 30 connection up "${connection_uuid}" ifname "${interface_name}"; then
    fail_after_rollback "${EXIT_ACTIVATE}" "激活网卡${interface_name}的新连接配置失败"
fi

verification_passed=0
actual_addresses=''
actual_gateway=''
actual_dns=''

for _attempt in 1 2 3 4 5 6 7 8 9 10; do
    actual_addresses="$(nmcli -g IP4.ADDRESS device show "${interface_name}" 2>/dev/null | tr -d '\r' || true)"
    actual_gateway="$(nmcli -g IP4.GATEWAY device show "${interface_name}" 2>/dev/null | tr -d '\r' || true)"
    actual_dns="$(nmcli -g IP4.DNS device show "${interface_name}" 2>/dev/null | tr -d '\r' || true)"

    if grep -Fqx -- "${ip_address}/${prefix}" <<< "${actual_addresses}" \
        && grep -Fqx -- "${gateway}" <<< "${actual_gateway}" \
        && grep -Fqx -- "${dns1}" <<< "${actual_dns}" \
        && grep -Fqx -- "${dns2}" <<< "${actual_dns}"; then
        verification_passed=1
        break
    fi

    sleep 1
done

if [[ "${verification_passed}" -ne 1 ]]; then
    fail_after_rollback "${EXIT_VERIFY}" \
        "网络配置验证失败；实际地址=${actual_addresses:-空}，实际网关=${actual_gateway:-空}，实际DNS=${actual_dns:-空}"
fi

log "网络配置成功并已持久化：${interface_name} ${ip_address}/${prefix}"
exit 0
