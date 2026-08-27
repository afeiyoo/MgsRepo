#include "deploytool.h"

#include "Logger.h"
#include "define.h"
#include "utils/configutils.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"

#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>

using namespace Utils;

#ifndef Q_OS_WIN
namespace {

bool parseIpv4(const QString &address, quint32 *numericAddress = nullptr)
{
    const QStringList parts = address.trimmed().split('.');
    if (parts.size() != 4) {
        return false;
    }

    quint32 result = 0;
    for (const QString &part : parts) {
        if (part.isEmpty()) {
            return false;
        }
        for (const QChar ch : part) {
            if (!ch.isDigit()) {
                return false;
            }
        }

        bool ok = false;
        const int value = part.toInt(&ok);
        if (!ok || value < 0 || value > 255) {
            return false;
        }
        result = (result << 8) | static_cast<quint32>(value);
    }

    if (numericAddress) {
        *numericAddress = result;
    }
    return true;
}

bool netMaskToPrefix(const QString &netMask, int &prefix)
{
    const QString value = netMask.trimmed();

    bool isPrefix = false;
    const int prefixValue = value.toInt(&isPrefix);
    if (isPrefix && !value.contains('.')) {
        if (prefixValue < 1 || prefixValue > 32) {
            return false;
        }
        prefix = prefixValue;
        return true;
    }

    quint32 mask = 0;
    if (!parseIpv4(value, &mask)) {
        return false;
    }

    bool metZero = false;
    int bitCount = 0;
    for (int bit = 31; bit >= 0; --bit) {
        const bool isOne = (mask & (quint32(1) << bit)) != 0;
        if (isOne) {
            if (metZero) {
                return false;
            }
            ++bitCount;
        } else {
            metZero = true;
        }
    }

    if (bitCount == 0) {
        return false;
    }
    prefix = bitCount;
    return true;
}

bool runCommand(const QString &program,
                const QStringList &arguments,
                QByteArray &standardOutput,
                QString &error,
                int timeoutMs = 30000)
{
    QProcess process;
    process.setProgram(program);
    process.setArguments(arguments);
    process.start();

    if (!process.waitForStarted(5000)) {
        error = QStringLiteral("无法启动%1: %2").arg(program, process.errorString());
        return false;
    }

    if (!process.waitForFinished(timeoutMs)) {
        process.kill();
        process.waitForFinished(3000);
        error = QStringLiteral("命令执行超时: %1 %2").arg(program, arguments.join(' '));
        return false;
    }

    standardOutput = process.readAllStandardOutput();
    const QString standardError = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        error = standardError.isEmpty()
                    ? QStringLiteral("命令执行失败，退出码: %1").arg(process.exitCode())
                    : standardError;
        return false;
    }

    return true;
}

} // namespace
#endif

DeployTool::DeployTool(QObject *parent)
    : IDeployTool(parent)
{
    m_conf = new ConfigUtils(this);
}

DeployTool::~DeployTool() {}

bool DeployTool::loadDeployInfo(const QString &path, QString &errDesc)
{
    m_deployInfo = new ST_DeployInfo{};

    // TODO
    return true;
}

bool DeployTool::saveDeviceCtrlFile(const QString &path, QString &errDesc)
{
    QByteArray data = genDeviceCtrlData(*m_deployInfo);
    return saveConfigFile(path, data, QStringLiteral("DeviceCtrl.json"), errDesc);
}

bool DeployTool::saveLaneUIFile(const QString &path, QString &errDesc)
{
    QByteArray data = genLaneUIData(*m_deployInfo);
    return saveConfigFile(path, data, QStringLiteral("LaneUI.ini"), errDesc);
}

bool DeployTool::saveLaneBaseConfigFile(const QString &path, QString &errDesc)
{
    QByteArray data = genLaneBaseConfigData(*m_deployInfo);
    return saveConfigFile(path, data, QStringLiteral("LaneBaseConfig.ini"), errDesc);
}

bool DeployTool::saveDtpAgentFile(const QString &path, QString &errDesc)
{
    QByteArray data = genDtpAgentData(*m_deployInfo);
    return saveConfigFile(path, data, QStringLiteral("DtpAgent.cfg"), errDesc);
}

bool DeployTool::saveStartFile(const QString &path, QString &errDesc)
{
    QByteArray data = genStartData(*m_deployInfo);
    return saveConfigFile(path, data, QStringLiteral("start123.json"), errDesc);
}

bool DeployTool::updateNetwork(const QString &interfaceName, QString &errDesc)
{
    errDesc.clear();
#ifdef Q_OS_WIN
    Q_UNUSED(interfaceName)
    errDesc = QStringLiteral("修改网络配置仅支持Linux系统");
    return false;
#else
    const QString device = interfaceName.trimmed();
    const QString laneIp = m_deployInfo->laneIP.trimmed();
    const QString gateway = m_deployInfo->gateWay.trimmed();

    if (device.isEmpty()) {
        errDesc = QStringLiteral("未指定需要配置的网卡");
        return false;
    }
    if (!parseIpv4(laneIp)) {
        errDesc = QStringLiteral("车道IP格式错误: %1").arg(laneIp);
        return false;
    }
    if (!parseIpv4(gateway)) {
        errDesc = QStringLiteral("网关格式错误: %1").arg(gateway);
        return false;
    }

    int prefix = 0;
    if (!netMaskToPrefix(m_deployInfo->netMask, prefix)) {
        errDesc = QStringLiteral("子网掩码格式错误: %1").arg(m_deployInfo->netMask);
        return false;
    }

    const QString nmcli = QStandardPaths::findExecutable(QStringLiteral("nmcli"));
    if (nmcli.isEmpty()) {
        errDesc = QStringLiteral("系统未安装nmcli，当前仅支持NetworkManager管理的网络");
        return false;
    }

    QByteArray output;
    QString commandError;
    if (!runCommand(nmcli,
                    {QStringLiteral("-g"), QStringLiteral("GENERAL.TYPE"), QStringLiteral("device"),
                     QStringLiteral("show"), device},
                    output,
                    commandError)) {
        errDesc = QStringLiteral("网卡%1不存在或无法读取: %2").arg(device, commandError);
        return false;
    }
    if (QString::fromLocal8Bit(output).trimmed() != QStringLiteral("ethernet")) {
        errDesc = QStringLiteral("指定设备不是以太网网卡: %1").arg(device);
        return false;
    }

    output.clear();
    if (!runCommand(nmcli,
                    {QStringLiteral("-g"), QStringLiteral("GENERAL.CONNECTION"), QStringLiteral("device"),
                     QStringLiteral("show"), device},
                    output,
                    commandError)) {
        errDesc = QStringLiteral("获取网卡%1的连接配置失败: %2").arg(device, commandError);
        return false;
    }

    const QString connectionName = QString::fromLocal8Bit(output).trimmed();
    if (connectionName.isEmpty() || connectionName == QStringLiteral("--")) {
        errDesc = QStringLiteral("网卡%1没有活动的NetworkManager连接配置").arg(device);
        return false;
    }

    const QString addressWithPrefix = QStringLiteral("%1/%2").arg(laneIp).arg(prefix);
    const QString dnsServers = QStringLiteral("35.16.1.49,35.17.1.57");
    const QStringList modifyArguments = {
        QStringLiteral("connection"),
        QStringLiteral("modify"),
        connectionName,
        QStringLiteral("connection.interface-name"),
        device,
        QStringLiteral("ipv4.method"),
        QStringLiteral("manual"),
        QStringLiteral("ipv4.addresses"),
        addressWithPrefix,
        QStringLiteral("ipv4.gateway"),
        gateway,
        QStringLiteral("ipv4.dns"),
        dnsServers,
        QStringLiteral("ipv4.ignore-auto-dns"),
        QStringLiteral("yes")};

    output.clear();
    if (!runCommand(nmcli, modifyArguments, output, commandError)) {
        errDesc = QStringLiteral("修改网卡%1的连接配置失败: %2").arg(device, commandError);
        return false;
    }

    output.clear();
    if (!runCommand(nmcli,
                    {QStringLiteral("--wait"), QStringLiteral("30"), QStringLiteral("connection"),
                     QStringLiteral("up"), connectionName, QStringLiteral("ifname"), device},
                    output,
                    commandError,
                    35000)) {
        errDesc = QStringLiteral("启用网卡%1的新配置失败: %2；可修正问题后重试").arg(device, commandError);
        return false;
    }

    output.clear();
    if (!runCommand(nmcli,
                    {QStringLiteral("-g"), QStringLiteral("IP4.ADDRESS,IP4.GATEWAY,IP4.DNS"),
                     QStringLiteral("device"), QStringLiteral("show"), device},
                    output,
                    commandError)) {
        errDesc = QStringLiteral("读取网卡%1的生效配置失败: %2").arg(device, commandError);
        return false;
    }

    const QStringList effectiveValues = QString::fromLocal8Bit(output).split('\n', Qt::SkipEmptyParts);
    bool addressMatched = false;
    bool gatewayMatched = false;
    bool primaryDnsMatched = false;
    bool secondaryDnsMatched = false;
    for (const QString &rawValue : effectiveValues) {
        const QString value = rawValue.trimmed();
        addressMatched = addressMatched || value == addressWithPrefix;
        gatewayMatched = gatewayMatched || value == gateway;
        primaryDnsMatched = primaryDnsMatched || value == QStringLiteral("35.16.1.49");
        secondaryDnsMatched = secondaryDnsMatched || value == QStringLiteral("35.17.1.57");
    }

    if (!addressMatched || !gatewayMatched || !primaryDnsMatched || !secondaryDnsMatched) {
        errDesc = QStringLiteral("网卡%1配置已提交，但生效结果校验失败，请重试").arg(device);
        return false;
    }

    LOG_CINFO(L_CATE).noquote() << "网卡" << device << "配置成功，IP:" << addressWithPrefix
                                << "网关:" << gateway << "DNS:" << dnsServers;
    return true;
#endif
}

uchar DeployTool::getLaneType(const QString &str) const
{
    if (str.trimmed() == QStringLiteral("混合入口")) {
        return 1;
    } else if (str.trimmed() == QStringLiteral("混合出口")) {
        return 2;
    } else if (str.trimmed() == QStringLiteral("ETC入口")) {
        return 3;
    } else if (str.trimmed() == QStringLiteral("ETC出口")) {
        return 4;
    } else {
        LOG_CWARNING(L_CATE).noquote() << "未知车道类型:" << str;
        return 0;
    }
}

QByteArray DeployTool::genLaneUIData(const ST_DeployInfo &info) const
{
    QString content;

    QTextStream stream(&content);

    stream << "[BaseInfo]\n"
           << "LaneID=" << info.laneID << "\n"
           << "LaneType=" << info.laneType << "\n"
           << "StationID=" << info.stationID << "\n"
           << "StationName=" << info.stationName << "\n"
           << "\n"
           << "[LaneDB]\n"
           << "dbhost=" << info.dbIP << "\n"
           << "dbname=" << info.dbName << "\n"
           << "dbpasswd=" << info.dbPassword << "\n"
           << "dbport=" << info.dbPort << "\n"
           << "dbtype=" << info.dbType << "\n"
           << "dbuser=" << info.dbUser << "\n"
           << "\n"
           << "[RedisDB]\n"
           << "dbhost=" << info.redisIP << "\n"
           << "dbname=" << info.redisName << "\n"
           << "dbpasswd=" << info.redisPassword << "\n"
           << "dbport=" << info.redisPort << "\n"
           << "dbuser=" << info.redisUser << "\n";

    return content.toUtf8();
}

QByteArray DeployTool::genLaneBaseConfigData(const ST_DeployInfo &info) const
{
    QVariantMap oneMap;

    QVariantMap readerMap;
    readerMap["ApiFileName"] = info.readerApiName;
    readerMap["ApiType"] = QString::number(info.readerApiType);
    readerMap["ComPort"] = info.readerPort;
    readerMap["SlotNo"] = QString::number(info.readerSlot);

    oneMap["LaneType"] = info.laneType;
    oneMap["CfgVersion"] = "202312231614";
    oneMap["CardReader"] = readerMap;

    return DataDealUtils::mapToJson(oneMap, QJsonDocument::Indented);
}

QByteArray DeployTool::genDeviceCtrlData(const ST_DeployInfo &info) const
{
    QVariantMap oneMap;

    QVariantMap capMap;
    capMap["CapDevice"] = info.capDevice;
    capMap["IP"] = info.capIP;
    capMap["LaneID"] = QString::number(info.laneID);
    capMap["Port"] = QString::number(info.capPort);
    capMap["StationHex"] = info.stationID;
    capMap["StationName"] = info.stationName;
    capMap["TriggerMode"] = QString::number(info.capTriggerMode);
    capMap["Type"] = QString::number(info.capType);

    QVariantMap robotMap;
    // TODO

    return DataDealUtils::mapToJson(oneMap, QJsonDocument::Indented);
}

QByteArray DeployTool::genDtpAgentData(const ST_DeployInfo &info) const
{
    QString content;

    QTextStream stream(&content);

    // TODO

    return content.toUtf8();
}

QByteArray DeployTool::genStartData(const ST_DeployInfo &info) const
{
    // TODO
    QVariantMap oneMap;

    return DataDealUtils::mapToJson(oneMap, QJsonDocument::Indented);
}

bool DeployTool::saveConfigFile(const QString &path, const QByteArray &data, const QString &configName, QString &errDesc) const
{
    errDesc.clear();

    const QFileInfo targetInfo(path);
    QDir targetDir = targetInfo.dir();
    if (!targetDir.exists() && !targetDir.mkpath(QStringLiteral("."))) {
        errDesc = QStringLiteral("创建配置目录失败: %1").arg(targetDir.absolutePath());
        return false;
    }

    LOG_CINFO(L_CATE).noquote() << "待写入" << configName << "数据:" << data;

    FileSaver saver(targetInfo.absoluteFilePath());
    if (!saver.write(data)) {
        LOG_CERROR(L_CATE).noquote() << "将数据写入到" << configName << "失败:" << saver.errorString();
        errDesc = QStringLiteral("写入%1失败: %2").arg(configName, saver.errorString());
        return false;
    }
    if (!saver.finalize()) {
        LOG_CERROR(L_CATE).noquote() << "将数据保存到" << configName << "失败:" << saver.errorString();
        errDesc = QStringLiteral("保存%1失败: %2").arg(configName, saver.errorString());
        return false;
    }

    LOG_CINFO(L_CATE).noquote() << "配置信息成功写入" << configName;
    return true;
}

// --------------------------------------------------------
IDeployTool *createDeployTool()
{
    return new DeployTool();
}

void destroyDeployTool(IDeployTool *tool)
{
    delete tool;
}
