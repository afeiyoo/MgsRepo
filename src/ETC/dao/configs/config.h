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

    virtual void load(const Utils::FileName &confPath) = 0;

    QString dump() const;

protected:
    void readConfig();

    Utils::ConfigUtils *m_confUtil = nullptr; // 配置读取工具

public:
    // 日志配置
    QString m_logFormat;
    uint m_logMaxSaveDays;

    // 车道基础配置
    QString m_stationName; // 站名称
    QString m_stationID;   // 站代码
    uint m_laneID;         // 车道号
    uint m_laneType;       // 车道类型
    QString m_psdFlagID;   // 承载门架编号

    // 数据库配置
    uint m_dbType;        // 类型
    QString m_dbHost;     // IP
    uint m_dbPort;        // 端口
    QString m_dbName;     // 数据库名
    QString m_dbUser;     // 用户名
    QString m_dbPassword; // 密码

    // Redis配置
    QString m_redisHost;     // IP
    uint m_redisPort;        // 端口
    QString m_redisDBName;   // 数据库名
    QString m_redisUser;     // 用户名
    QString m_redisPassword; // 密码

    // 测试配置
    bool m_isTest;           // 是否测试版
    QString m_testPlate;     // 测试车牌
    QString m_testImagePath; // 测试车辆图片

    // 车道个性化配置
    bool m_isEnableCompareVehplate; // 是否启用车牌比对
    QString m_savePicPath;          // 保存抓拍图片路径
};
