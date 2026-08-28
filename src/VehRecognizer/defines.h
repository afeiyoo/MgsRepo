#pragma once

#include <QByteArray>
#include <QDateTime>
#include <QString>

// 日志类别
static const char L_CATE[] = "VehRecognizer";

static const int MAX_BUFF_SIZE = 4 * 1024 * 1024; // 数据缓冲区允许的最大长度（4MB）

// 帧参数
static const QByteArray STX = QByteArray::fromHex("FFFF");                       // 帧头
static const int STX_LEN = 2;                                                    // 帧头 2 字节
static const int VER_LEN = 1;                                                    // 版本号 1 字节
static const int SEQ_LEN = 1;                                                    // 序列号 1 字节
static const int LEN_FIELD_LEN = 4;                                              // 数据长度字段 4 字节（大端）
static const int CRC_LEN = 2;                                                    // CRC 校验码 2 字节
static const int FIXED_HEADER_LEN = STX_LEN + VER_LEN + SEQ_LEN + LEN_FIELD_LEN; // 固定头长度

// 重传参数
static const int MAX_RETRY_TIMES = 3;    // 最大重传尝试次数
static const int RETRY_INTERVAL = 1000;  // 重传时间间隔，单位ms
static const int REQUEST_TIMEOUT = 1000; // 请求应答超时时间，单位ms

// 重连参数
static const int HEARTBEAT_TIMEOUT = 30000; // 心跳超时时间，单位ms
static const int MAX_RECONNECT_TIMES = 3;   // 最大自动重连次数
static const int RECONNECT_INTERVAL = 1000; // 自动重连等待间隔，单位ms

// 图像信息参数
static const uint MAX_IMG_SIZE = 1024 * 1024;
static const uint MAX_VED_SIZE = 2 * 1024 * 1024;

// 车辆信息聚合参数
static const int VEHICLE_CACHE_TIMEOUT_SECS = 30 * 60;

// 重传数据信息
struct ST_PendingRequest
{
    uchar seq = 0;
    uchar type = 0;
    int retryCount = 0;
    QByteArray frame;
};

// EA指令处理结果
struct ST_EAHandleResult
{
    bool status = false;

    QString vehPlate;    // 车牌
    int plateColor = 0;  // 车牌颜色
    QDateTime vehTime;   // 过车时间
    int vehClass = 0;    // 车型
    QString axleType;    // 轴型
    int axleCount = 0;   // 轴数
    int totalLength = 0; // 车货总长度
    int totalWidth = 0;  // 车货总宽度
    int totalHeight = 0; // 车货总高度
    quint32 extFlag = 0; // 扩展标识
    int direction = 0;   // 行驶方向
};

// EB指令处理结果
struct ST_EBHandleResult
{
    bool status = false;

    QString vehPlate;   // 车牌
    int plateColor = 0; // 车牌颜色
    QDateTime vehTime;  // 过车时间
    int imgType = 0;    // 图像类型
    QByteArray imgInfo; // 图像信息
};
