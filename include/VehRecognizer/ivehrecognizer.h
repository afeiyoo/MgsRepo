#pragma once

#include <QDateTime>
#include <QObject>
#include <QString>

#include "vehrecognizer_global.h"

struct ST_VehicleTypeInfo
{
    QString vehPlate;    // 车牌
    int plateColor = 0;  // 车牌颜色
    QDateTime vehTime;   // 过车时间
    int vehClass = 0;    // 车型
    QString axleType;    // 轴型
    int axleCount = 0;   // 轴数
    int totalLength = 0; // 车货总长度，单位cm；0表示无法识别
    int totalWidth = 0;  // 车货总宽度，单位cm；0表示无法识别
    int totalHeight = 0; // 车货总高度，单位cm；0表示无法识别
    quint32 extFlag = 0; // 扩展标识
    int direction = 0;   // 行驶方向
};
Q_DECLARE_METATYPE(ST_VehicleTypeInfo)

struct ST_VehicleImageInfo
{
    QString vehPlate;   // 车牌
    int plateColor = 0; // 车牌颜色
    QDateTime vehTime;  // 过车时间
    int imageType = 0;  // 1车头图片、2车尾图片、3车身图片、4短视频
    QString imagePath;  // 图片或视频的绝对路径
};
Q_DECLARE_METATYPE(ST_VehicleImageInfo)

class VEHRECOGNIZER_EXPORT IVehRecognizer : public QObject
{
    Q_OBJECT
public:
    explicit IVehRecognizer(QObject *parent = nullptr)
        : QObject(parent)
    {}
    virtual ~IVehRecognizer() = default;

    // 与服务端建立连接
    virtual void connectServer(const QString &ip, quint16 port) = 0;

    // 与服务端断开连接
    virtual void disconnectServer() = 0;

    // 设置协议版本号，必须在 connectServer() 前调用；默认使用版本 0x01
    virtual void setVersion(uchar ver) = 0;

    // 设置状态上报地址
    virtual void setUploadUrl(const QString &url, uchar minutes) = 0;

    // LED显示
    virtual void showLED(uchar color, const QString &data) = 0;

    // 语音播放
    virtual void playVoice(uchar count, const QString &text, int intervalMs = 3000) = 0;

signals:
    // 收到车型信息
    void sigVehicleTypeInfoReady(const ST_VehicleTypeInfo &vehicleInfo);

    // 收到车辆媒体信息
    void sigVehicleImageInfoReady(const ST_VehicleImageInfo &imageInfo);

    // TCP连接状态变化
    void sigConnectionStateChanged(bool connected);

    // 设备初始化状态变化
    void sigInitStateChanged(bool initialized);

    // 指令收到应答或最终超时；type对应A1指令类型
    void sigCmdFinished(uchar type, bool success);

    // 自动重连次数耗尽
    void sigReconnectFailed();
};

extern "C" VEHRECOGNIZER_EXPORT IVehRecognizer *createVehRecognizer(const QString &stationID, const QString &stationName, uint laneID);
extern "C" VEHRECOGNIZER_EXPORT void destroyVehRecognizer(IVehRecognizer *rec);
