#pragma once

#include <QObject>

#include "smartlanecontroller_global.h"

class SMARTLANECONTROLLER_EXPORT ISmartLaneController : public QObject
{
    Q_OBJECT
public:
    ISmartLaneController(QObject *parent = nullptr)
        : QObject(parent)
    {}

    virtual ~ISmartLaneController() = default;

    // 与服务端建立连接
    virtual void connectServer(const QString &ip, quint16 port) = 0;

    // 与服务端断开连接
    virtual void disconnectServer() = 0;

    // 设置协议版本号，必须在 connectServer() 前调用；默认使用版本 0x00
    virtual void setVersion(uchar ver) = 0;

    // 发送A1指令
    virtual void sendA1Cmd(const QMap<int, int> &relayMap, const QMap<int, int> &levelMap) = 0;

    // 发送A2指令
    virtual void sendA2Cmd(const QByteArray &url, uchar time) = 0;

    // 发送A3指令
    virtual void sendA3Cmd(uchar type, const QByteArray &data) = 0;

signals:
    void sigRecvD2Cmd(QByteArray cmd);
    void sigRecvD3Cmd(QByteArray cmd);
    void sigRecvD6Cmd(QByteArray cmd);
    void sigConnectionStateChanged(bool connected);
    void sigHeartbeatStateChanged(bool normal);
    void sigReconnectFailed();
};

extern "C" SMARTLANECONTROLLER_EXPORT ISmartLaneController *createSmartLaneController();
extern "C" SMARTLANECONTROLLER_EXPORT void destroySmartLaneController(ISmartLaneController *controller);
