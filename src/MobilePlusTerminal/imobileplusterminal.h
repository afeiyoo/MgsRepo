#pragma once

#include <QObject>

#include "mobileplusterminal_global.h"

class MOBILEPLUSTERMINAL_EXPORT IMobilePlusTerminal : public QObject
{
    Q_OBJECT
public:
    IMobilePlusTerminal(QObject *parent = nullptr)
        : QObject(parent) {};

    virtual ~IMobilePlusTerminal() = default;

    // 与服务端建立连接
    virtual void connectServer(const QString &ip, quint16 port) = 0;

    // 与服务端断开连接
    virtual void disconnectServer() = 0;

    // 设置协议版本号
    virtual void setVersion(uchar ver) = 0;

    // 设备初始化
    virtual void initialize(const QString &stationID, uint laneID, uint seq) = 0;

    // 二维码显示
    virtual void showQRCode(const QString &stationName, const QString &vehClass, const QString &vehPlate, const QString &barCode) = 0;

    // 二维码显示
    virtual void showLED(const QString &text) = 0;

    // 图片显示
    virtual void showPics(const QString &data) = 0;

    // 设置状态上传URL
    virtual void setUploadUrl(const QString &url, int time) = 0;
};

extern "C" MOBILEPLUSTERMINAL_EXPORT IMobilePlusTerminal *createMobilePlusTerminal();
extern "C" MOBILEPLUSTERMINAL_EXPORT void destroyMobilePlusTerminal(IMobilePlusTerminal *terminal);
