#pragma once

#include <QByteArray>

// 日志类别
static const char L_CATE[] = "SmartLaneController";

static const int MAX_BUFF_SIZE = 4 * 1024 * 1024; // 数据缓冲区允许的最大长度（4MB）

static const QByteArray STX = QByteArray::fromHex("FFFF");                       // 帧头
static const QByteArray VER = QByteArray::fromHex("00");                         // 版本号
static const int STX_LEN = 2;                                                    // 帧头 2 字节
static const int VER_LEN = 1;                                                    // 版本号 1 字节
static const int SEQ_LEN = 1;                                                    // 序列号 1 字节
static const int LEN_FIELD_LEN = 4;                                              // 数据长度字段 4 字节（大端）
static const int CRC_LEN = 2;                                                    // CRC 校验码 2 字节
static const int FIXED_HEADER_LEN = STX_LEN + VER_LEN + SEQ_LEN + LEN_FIELD_LEN; // 固定头长度

// 重传参数
static const int MAX_RETRY_TIMES = 3;   // 最大重传尝试次数
static const int REQUEST_TIMEOUT = 750; // 请求超时时间，单位ms

// 重连参数
static const int HEARTBEAT_TIMEOUT = 6000;  // 心跳超时时间，单位ms
static const int MAX_RECONNECT_TIMES = 3;   // 最大自动重连次数
static const int RECONNECT_INTERVAL = 1000; // 自动重连等待间隔，单位ms

// 指令队列数据
struct ST_SendTask
{
    uchar cmdType = 0;
    QByteArray cmd;
    uchar seq = 0;
    QByteArray frame;
    int retryCount = 0;
};
