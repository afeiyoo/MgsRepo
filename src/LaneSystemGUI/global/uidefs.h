#pragma once

#include <QString>

// 称重信息
struct ST_WeightInfoItem
{
    int index = 0; // 队列号
    QString plate;
    uint axisType = 0;
    uint axisNum = 0;
    qreal tollWeight = 0.0;
    uint status = 0; // 0-等待中，1-已交易
    bool isManual = false;
    qreal limitWeight = 0.0; // 限重
    bool allowPass = false;  // 是否允许过车
    qreal exceedRate = 0.0;
};
