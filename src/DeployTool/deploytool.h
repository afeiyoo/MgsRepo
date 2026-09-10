#pragma once

#include "ideploytool.h"

#include <QHash>
#include <QSqlDatabase>
#include <QVariant>

namespace Utils {
class ConfigUtils;
}
namespace QXlsx {
class Document;
}
namespace EasyQtSql {
class SqlFactory;
}
struct ST_DeployInfo;
class DeployTool : public IDeployTool
{
    Q_OBJECT
public:
    explicit DeployTool(QObject *parent = nullptr);
    ~DeployTool() override;

    bool loadDeployInfo(const QString &path, QString &errDesc) override;

    ST_DeployInfo getCurDeployInfo(const QString &stationID, int laneID) override;

    bool initLaneSoftware(const ST_DeployInfo &info, QString &errDesc) override;

    bool initDtpAgent(const ST_DeployInfo &info, const QString &fullVer, QString &errDesc) override;

    bool initStart123(const ST_DeployInfo &info, const QString &ver, QString &errDesc) override;

    bool updateNetwork(const ST_DeployInfo &info, const QString &interfaceName, QString &errDesc) override;

    bool syncFeeRate(const QString &filePath, const QString &stationID, QString &errDesc) override;

private:
    bool saveDeviceCtrlFile(const ST_DeployInfo &info, QString &errDesc);

    bool saveLaneUIFile(const ST_DeployInfo &info, QString &errDesc);

    bool saveLaneBaseConfigFile(const ST_DeployInfo &info, QString &errDesc);

    bool syncDBConfig(const ST_DeployInfo &info, QString &errDesc);

    // 同步重启服务；调用方应在工作线程中执行。
    static bool restartService(const QString &serviceName, QString &errDesc);

    int getLaneType(const QString &str) const;
    int getCapTriggerMode(const QString &str) const;
    int getOverlapType(const QString &str) const;
    int getFeeScrType(const QString &str) const;
    int getRobotType(const QString &str) const;
    int getReaderType(const QString &str) const;

    // 将数据data保存到文件path中
    bool saveFile(const QString &path, const QByteArray &data, QString &errDesc) const;

    bool executeSql(const QSqlDatabase &sdb, const QString &itemName, const QString &sql, const QVariantMap &params, QString &errDesc) const;

private:
    // 完整部署信息
    QHash<QString, ST_DeployInfo> m_deployInfos;
    Utils::ConfigUtils *m_conf = nullptr;
    // 数据库连接池
    EasyQtSql::SqlFactory *m_dbFactory = nullptr;
};
