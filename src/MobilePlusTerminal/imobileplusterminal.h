#pragma once

#include <QObject>

#include "mobileplusterminal_global.h"

class MOBILEPLUSTERMINAL_EXPORT IMobilePlusTerminal : public QObject
{
    Q_OBJECT
public:
    IMobilePlusTerminal(QObject *parent = nullptr)
        : QObject(parent)
    {}

    virtual ~IMobilePlusTerminal() = default;

    // 与服务端建立连接
    virtual void connectServer(const QString &ip, quint16 port) = 0;

    // 与服务端断开连接
    virtual void disconnectServer() = 0;

    // 设置协议版本号，必须在 connectServer() 前调用；默认使用版本 0x01
    virtual void setVersion(uchar ver) = 0;

    // 二维码显示
    virtual void showQRCode(const QString &stationName, const QString &vehClass, const QString &vehPlate, const QString &barCode) = 0;

    // 二维码显示
    virtual void showLED(const QString &text) = 0;

    // 图片显示
    virtual void showPics(const QByteArray &data) = 0;

    // 设置状态上传URL
    virtual void setUploadUrl(const QString &url, int time) = 0;

signals:
    // TCP连接状态变化
    void sigConnectionStateChanged(uint devSeq, bool connected);

    // 设备初始化状态变化
    void sigInitStateChanged(uint devSeq, bool initialized);

    // 指令收到应答或最终超时；type对应A1指令类型
    void sigCmdFinished(uint devSeq, uchar type, bool success);

    // 自动重连次数耗尽
    void sigReconnectFailed(uint devSeq);
};

extern "C" MOBILEPLUSTERMINAL_EXPORT IMobilePlusTerminal *createMobilePlusTerminal(const QString &stationID, uint laneID, uint devSeq);
extern "C" MOBILEPLUSTERMINAL_EXPORT void destroyMobilePlusTerminal(IMobilePlusTerminal *terminal);
