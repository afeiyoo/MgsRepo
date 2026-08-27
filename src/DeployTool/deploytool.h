#pragma once

#include "ideploytool.h"

namespace Utils {
class ConfigUtils;
}
struct ST_DeployInfo;
class DeployTool : public IDeployTool
{
    Q_OBJECT
public:
    explicit DeployTool(QObject *parent = nullptr);
    ~DeployTool() override;

    bool loadDeployInfo(const QString &path, QString &errDesc) override;

    bool saveDeviceCtrlFile(const QString &path, QString &errDesc) override;

    bool saveLaneUIFile(const QString &path, QString &errDesc) override;

    bool saveLaneBaseConfigFile(const QString &path, QString &errDesc) override;

    bool saveDtpAgentFile(const QString &path, QString &errDesc) override;

    bool saveStartFile(const QString &path, QString &errDesc) override;

    bool updateNetwork(const QString &interfaceName, QString &errDesc) override;

private:
    uchar getLaneType(const QString &str) const;
    QByteArray genLaneUIData(const ST_DeployInfo &info) const;
    QByteArray genLaneBaseConfigData(const ST_DeployInfo &info) const;
    QByteArray genDeviceCtrlData(const ST_DeployInfo &info) const;
    QByteArray genDtpAgentData(const ST_DeployInfo &info) const;
    QByteArray genStartData(const ST_DeployInfo &info) const;

    bool saveConfigFile(const QString &path, const QByteArray &data, const QString &configName, QString &errDesc) const;

private:
    ST_DeployInfo *m_deployInfo = nullptr;
    Utils::ConfigUtils *m_conf = nullptr;
};
