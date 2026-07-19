#pragma once

#include <QObject>
#include <QReadWriteLock>

#include "env/defines.h"

namespace Utils {
class FileName;
class ConfigUtils;
} // namespace Utils

class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = nullptr);
    ~Config() override;

    void loadConfig(const Utils::FileName &path);

    int fullBatchNo() const;
    void setFullBatchNo(int batchNo);

public:
    // 启动加载后，不会再被修改的配置

    // 数据库配置
    uint m_dbType;
    QString m_dbHost;
    QString m_dbName;
    QString m_dbUser;
    QString m_dbPassword;
    uint m_dbPort;

    // 日志配置
    QString m_logFormat;
    int m_logLimits;

    // sql配置
    QStringList m_sqlFiles; // sql文件存储路径

    // 全量配置
    QString m_fullBlackPath; // 全量文件所在路径

    // 服务配置
    QString m_stationServiceURL; // 站级服务

private:
    // 启动加载后，还有可能被修改的配置

    // 全量配置
    int m_fullBatchNo = 0; // 当前全量批次

private:
    mutable QReadWriteLock m_lock;
    Utils::ConfigUtils *m_confUtil = nullptr; // 配置读取工具
};
