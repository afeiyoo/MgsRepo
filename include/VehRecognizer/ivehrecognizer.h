#pragma once

#include <QDateTime>
#include <QMetaType>
#include <QObject>
#include <QString>

#include "vehrecognizer_global.h"

struct ST_VehicleTypeInfo
{
    int vehClass = 0;    // 车型
    QString axleType;    // 轴型
    int axleCount = 0;   // 轴数
    int totalLength = 0; // 车货总长度，单位cm；0表示无法识别
    int totalWidth = 0;  // 车货总宽度，单位cm；0表示无法识别
    int totalHeight = 0; // 车货总高度，单位cm；0表示无法识别
    quint32 extFlag = 0; // 扩展标识
    int direction = 0;   // 行驶方向
};

struct ST_VehicleImageInfo
{
    QString headImagePath;  // 车头图片绝对路径
    QString tailImagePath;  // 车尾图片绝对路径
    QString bodyImagePath;  // 车身图片绝对路径
    QString shortVideoPath; // 短视频MP4绝对路径
};

// 完整车辆信息。EA车型信息和四类EB媒体信息全部到齐后通过信号发送。
struct ST_VehicleInfo
{
    QString vehPlate;              // 车牌
    int plateColor = 0;            // 车牌颜色
    QDateTime vehTime;             // 过车时间
    ST_VehicleTypeInfo typeInfo;   // 车型信息
    ST_VehicleImageInfo imageInfo; // 图片及视频信息
};
Q_DECLARE_METATYPE(ST_VehicleInfo)

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
    // 一辆车的EA车型信息及四类EB媒体信息全部到齐
    void sigVehicleInfoReady(const ST_VehicleInfo &vehicleInfo);

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
