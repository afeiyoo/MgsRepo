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
class QProcess;
class QTimer;
struct ST_DeployInfo;
class DeployTool : public IDeployTool
{
    Q_OBJECT
public:
    explicit DeployTool(QObject *parent = nullptr);
    ~DeployTool() override;

    bool loadDeployInfo(const QString &path, QString &errDesc) override;

    ST_DeployInfo getCurDeployInfo(const QString &stationID, int laneID) override;

    bool saveDeviceCtrlFile(const ST_DeployInfo &info, const QString &path, QString &errDesc) override;

    bool saveLaneUIFile(const ST_DeployInfo &info, const QString &path, QString &errDesc) override;

    bool saveLaneBaseConfigFile(const ST_DeployInfo &info, const QString &path, QString &errDesc) override;

    bool saveDtpAgentFile(const ST_DeployInfo &info, const QString &fullBlackName, const QString &path, QString &errDesc) override;

    bool saveStartFile(const ST_DeployInfo &info, const QString &ver, const QString &path, QString &errDesc) override;

    bool updateNetwork(const ST_DeployInfo &info, const QString &interfaceName, QString &errDesc) override;

    bool syncFeeRate(const QString &filePath, const QString &stationID, QString &errDesc) override;

    bool syncDBConfig(const ST_DeployInfo &info, QString &errDesc) override;

    bool isNetworkUpdating() const override;

private:
    int getLaneType(const QString &str) const;
    int getCapTriggerMode(const QString &str) const;
    int getOverlapType(const QString &str) const;
    int getFeeScrType(const QString &str) const;
    int getRobotType(const QString &str) const;
    int getReaderType(const QString &str) const;

    // 将数据data保存到文件path中
    bool saveFile(const QString &path, const QByteArray &data, QString &errDesc) const;

    // 使用EasyQtSql执行一条MySQL UPSERT语句
    bool executeUpsert(const QSqlDatabase &sdb, const QString &itemName, const QString &sql, const QVariantMap &params, QString &errDesc) const;

    void finishNetworkUpdate(bool success, const QString &message);

private:
    // 完整部署信息
    QHash<QString, ST_DeployInfo> m_deployInfos;
    Utils::ConfigUtils *m_conf = nullptr;
    QProcess *m_networkProcess = nullptr;
    QTimer *m_networkTimeoutTimer = nullptr;
    bool m_networkUpdating = false;
    bool m_networkTimedOut = false;
    // 数据库连接池
    EasyQtSql::SqlFactory *m_dbFactory = nullptr;
};
