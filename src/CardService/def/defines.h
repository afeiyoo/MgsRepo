#pragma once

#include <QByteArray>
#include <QString>

struct ST_ConfigSnap
{
    QString comPort;
    int slotNo;
    int apiType;
    QString apiName;
    int laneType = 1;
};

// 读取PSAM卡信息结果
struct ST_PsamReadResult
{
    uchar errorCode = 0;
    uchar isNeedAuth = 0; // 1. 需要授权
    QByteArray termNo;
    QByteArray psamNo;
    QByteArray random;
};
