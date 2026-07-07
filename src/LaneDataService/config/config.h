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

private:
    Utils::ConfigUtils *m_confUtil = nullptr; // 配置读取工具
};
