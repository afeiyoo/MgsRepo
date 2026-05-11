#pragma once

#include "global/appdefs.h"
#include <QObject>

class SignalManager : public QObject
{
    Q_OBJECT
public:
    explicit SignalManager(QObject *parent = nullptr);
    ~SignalManager() override;

signals:
    // 卡机信号
    // 界面显示指令
    void sigRobotReceiveCommand(uchar cmdType, QByteArray json);

    // 卡机心跳状态设置
    void sigRobotStatusChanged(ST_CardRobotStatusInfo info1, ST_CardRobotStatusInfo info2, ST_CardRobotStatusInfo info3, ST_CardRobotStatusInfo info4);

    // 指令发送
    void sigRobotSendCommand(uchar cmdType, QByteArray json);

    // 指令发送完成结果通过
    void sigRobotSendCommandFinish(uchar cmdType, bool res);

    // 是否自动应答触发按键取卡
    void sigIsAutoRespCommand52(bool res);

    // 界面还原
    void sigRobotScreenInit();

    // 情报板信号
    // 设备状态变化
    void sigDevStatusChanged(bool isNormal);

    // 网络状态信号
    void sigNetworkStatusChanged(bool isOnline);
};
