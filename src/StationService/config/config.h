#pragma once

#include <QObject>

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

public:
    // 日志配置
    QString m_logFormat; // 日志格式
    uint m_logLimits;    // 日志文件保存天数

    // 数据库配置
    uint m_dbType;
    QString m_dbHost;
    QString m_dbName;
    QString m_dbUser;
    QString m_dbPassword;
    uint m_dbPort;

    // 基础配置
    uint m_queryAuthType;
    QString m_queryAuthIP;
    QString m_blackCheckUrl; // 增量下载地址
    uint m_saveDays;         // 增量文件保存时间

private:
    Utils::ConfigUtils *m_confUtil = nullptr; // 配置读取工具
};
