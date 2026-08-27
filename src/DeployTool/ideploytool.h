#pragma once

#include <QObject>

#include "deploytool_global.h"

class DEPLOYTOOL_EXPORT IDeployTool : public QObject
{
    Q_OBJECT
public:
    explicit IDeployTool(QObject *parent = nullptr)
        : QObject(parent)
    {}

    virtual ~IDeployTool() = default;

    // 加载部署信息
    virtual bool loadDeployInfo(const QString &path, QString &errDesc) = 0;

    // 生成DeviceCtrl.json配置文件
    virtual bool saveDeviceCtrlFile(const QString &path, QString &errDesc) = 0;

    // 生成LaneUI.json配置文件
    virtual bool saveLaneUIFile(const QString &path, QString &errDesc) = 0;

    // 生成LaneBaseConfig.ini配置文件
    virtual bool saveLaneBaseConfigFile(const QString &path, QString &errDesc) = 0;

    // 生成DtpAgent.cfg配置文件
    virtual bool saveDtpAgentFile(const QString &path, QString &errDesc) = 0;

    // 生成Start123.json配置文件
    virtual bool saveStartFile(const QString &path, QString &errDesc) = 0;

    // 修改指定网卡的网络配置信息
    virtual bool updateNetwork(const QString &interfaceName, QString &errDesc) = 0;
};

extern "C" DEPLOYTOOL_EXPORT IDeployTool *createDeployTool();
extern "C" DEPLOYTOOL_EXPORT void destroyDeployTool(IDeployTool *tool);
