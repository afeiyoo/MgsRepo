#pragma once

#include <QImage>
#include <QList>
#include <QString>
#include <QStringList>

// 界面称重信息结构体
struct ST_WeightInfoItem
{
    int index = 0; // 队列号
    QString plate;
    uint axisType = 0;
    uint axisNum = 0;
    qreal tollWeight = 0.0;
    uint status = 0; // 0-等待中 1-已交易
    bool isManual = false;
    qreal limitWeight = 0.0;
    bool allowPass = false; // 是否允许过车
    qreal exceedRate = 0.0;
};

class IPageController
{
public:
    // 设备状态图标
    enum EM_DeviceIcon {
        CAPTURE = 1,
        CELLING_LAMP,
        FIRST_COIL,
        SECOND_COIL,
        THIRD_COIL,
        FOURTH_COIL,
        CAP_COIL,
        RAILLING_COIL,
        PASSING_LAMP,
        RAILING_MACHINE,
        RSU,
        WEIGHT
    };

    // 设备状态
    enum EM_DeviceIconStatus {
        FAIL = 1,     // 设备异常
        NORMAL_FALSE, // 设备正常，但设备状态false
        NORMAL_TRUE   // 设备正常，且设备状态true
    };

    // 界面显示日志等级
    enum EM_LogLevel { DEBUG = 1, INFO, WARN, ERROR };
};
