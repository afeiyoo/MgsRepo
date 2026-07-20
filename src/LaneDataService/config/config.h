#pragma once

#include <QObject>
#include <QReadWriteLock>

namespace Utils {
class FileName;
class ConfigUtils;
} // namespace Utils

struct ST_ConfigSnap;
class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = nullptr);
    ~Config() override;

    // 加载配置
    void loadConfig(const Utils::FileName &path);

    // 读取配置
    ST_ConfigSnap getConfigSnap() const;

    // 写配置
    void setFullBatchNo(QString batchNo);
    void setDeltaBatchNo(QString batchNo);

private:
    // 数据库配置
    uint m_dbType;
    QString m_dbHost;
    QString m_dbName;
    QString m_dbUser;
    QString m_dbPassword;
    uint m_dbPort;

    // 日志配置
    QString m_logFormat;
    int m_logLimits = 180;

    // sql配置
    QStringList m_sqlFiles; // sql文件存储路径

    // 状态名单配置
    QString m_fullBlackPath;    // 全量文件所在路径
    QString m_fullBatchNo = 0;  // 当前全量批次（可变）
    QString m_deltaBlackPath;   // 增量文件保存路径
    QString m_deltaBatchNo = 0; // 增量批次（可变）

    // 服务配置
    QString m_stationServiceURL; // 站级服务

private:
    mutable QReadWriteLock m_lock;
    Utils::ConfigUtils *m_confUtil = nullptr; // 配置读取工具
};
