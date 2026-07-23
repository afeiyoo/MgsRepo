#!/usr/bin/env bash

set -euo pipefail

APP_NAME="CardService"
SERVICE_NAME="cardservice.service"
INSTALL_DIR="${INSTALL_DIR:-/opt/cardservice}"
SERVICE_USER="${SERVICE_USER:-cardservice}"
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="${SOURCE_DIR:-${SCRIPT_DIR}}"
UNIT_FILE="/etc/systemd/system/${SERVICE_NAME}"

usage() {
    cat <<EOF
Usage: sudo ./cardservice.sh <install|uninstall|start|stop|restart|status|logs>

Environment variables:
  SOURCE_DIR    Deployment package directory (default: script directory)
  INSTALL_DIR   Installation directory (default: /opt/cardservice)
  SERVICE_USER  Linux service user (default: cardservice)
  PURGE=1       Remove the installation directory during uninstall

The deployment package must contain:
  CardService
  config.ini
  lib/           Optional directory containing private shared libraries
EOF
}

require_root() {
    if [[ "${EUID}" -ne 0 ]]; then
        echo "This operation must be run as root." >&2
        exit 1
    fi
}

install_service() {
    require_root

    local source_binary="${SOURCE_DIR}/${APP_NAME}"
    local source_config="${SOURCE_DIR}/config.ini"
    if [[ ! -f "${source_binary}" ]]; then
        echo "Missing executable: ${source_binary}" >&2
        exit 1
    fi
    if [[ ! -f "${source_config}" ]]; then
        echo "Missing configuration: ${source_config}" >&2
        exit 1
    fi

    if ! id "${SERVICE_USER}" >/dev/null 2>&1; then
        useradd --system --home-dir "${INSTALL_DIR}" --shell /usr/sbin/nologin "${SERVICE_USER}"
    fi
    if getent group dialout >/dev/null 2>&1; then
        usermod -a -G dialout "${SERVICE_USER}"
    fi

    install -d -m 0750 -o "${SERVICE_USER}" -g "${SERVICE_USER}" "${INSTALL_DIR}" "${INSTALL_DIR}/logs" "${INSTALL_DIR}/lib"
    install -m 0750 -o "${SERVICE_USER}" -g "${SERVICE_USER}" "${source_binary}" "${INSTALL_DIR}/${APP_NAME}"

    # Preserve an existing production configuration during upgrades.
    if [[ ! -f "${INSTALL_DIR}/config.ini" ]]; then
        install -m 0640 -o "${SERVICE_USER}" -g "${SERVICE_USER}" "${source_config}" "${INSTALL_DIR}/config.ini"
    fi

    if [[ -d "${SOURCE_DIR}/lib" ]]; then
        cp -a "${SOURCE_DIR}/lib/." "${INSTALL_DIR}/lib/"
        chown -R "${SERVICE_USER}:${SERVICE_USER}" "${INSTALL_DIR}/lib"
    fi

    local missing_libraries
    missing_libraries="$(LD_LIBRARY_PATH="${INSTALL_DIR}/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}" ldd "${INSTALL_DIR}/${APP_NAME}" \
        | awk '/not found/ {print $1}' | sort -u)"
    if [[ -n "${missing_libraries}" ]]; then
        echo "The following shared libraries are missing:" >&2
        echo "${missing_libraries}" >&2
        exit 1
    fi

    cat >"${UNIT_FILE}" <<EOF
[Unit]
Description=CardService card reader HTTP service
After=network-online.target
Wants=network-online.target

[Service]
Type=simple
User=${SERVICE_USER}
Group=${SERVICE_USER}
WorkingDirectory=${INSTALL_DIR}
Environment=LD_LIBRARY_PATH=${INSTALL_DIR}/lib
ExecStart=${INSTALL_DIR}/${APP_NAME} -exec
ExecStop=-${INSTALL_DIR}/${APP_NAME} -terminate
Restart=on-failure
RestartSec=3
TimeoutStopSec=30
UMask=0027
NoNewPrivileges=true

[Install]
WantedBy=multi-user.target
EOF

    systemctl daemon-reload
    systemctl enable --now "${SERVICE_NAME}"
    systemctl --no-pager --full status "${SERVICE_NAME}"
}

uninstall_service() {
    require_root
    systemctl disable --now "${SERVICE_NAME}" 2>/dev/null || true
    rm -f -- "${UNIT_FILE}"
    systemctl daemon-reload
    systemctl reset-failed "${SERVICE_NAME}" 2>/dev/null || true

    if [[ "${PURGE:-0}" == "1" ]]; then
        if [[ "${INSTALL_DIR}" != "/" && "${INSTALL_DIR}" == /opt/* ]]; then
            rm -rf -- "${INSTALL_DIR}"
        else
            echo "Refusing to purge unexpected INSTALL_DIR: ${INSTALL_DIR}" >&2
            exit 1
        fi
    else
        echo "Application files and config are preserved in ${INSTALL_DIR}."
    fi
}

action="${1:-}"
case "${action}" in
install)
    install_service
    ;;
uninstall)
    uninstall_service
    ;;
start | stop | restart)
    require_root
    systemctl "${action}" "${SERVICE_NAME}"
    ;;
status)
    systemctl --no-pager --full status "${SERVICE_NAME}"
    ;;
logs)
    journalctl -u "${SERVICE_NAME}" -f
    ;;
*)
    usage
    exit 1
    ;;
esac
