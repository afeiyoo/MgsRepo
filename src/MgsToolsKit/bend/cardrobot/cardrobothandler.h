#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "global/appdefs.h"

class CardRobotHandler : public QObject
{
    Q_OBJECT
public:
    explicit CardRobotHandler(QObject *parent = nullptr);
    ~CardRobotHandler() override;

    // 在指定端口启动监听
    bool startListen(quint16 port);
    // 关闭监听
    void stopListen();
    // 设置默认工位
    void setDefInfo(uchar defWorkStation, uchar defUpBoxNo, uchar defDownBoxNo);

public slots:
    void onNewConnection();
    void onDisconnected();
    void onReadyRead();

    // 卡机心跳状态设置
    void onRobotStatusChanged(ST_CardRobotStatusInfo info1, ST_CardRobotStatusInfo info2, ST_CardRobotStatusInfo info3, ST_CardRobotStatusInfo info4);

    // 发送命令
    void onRobotSendCommand(uchar cmdType, QByteArray json);

    // 是否自动应答触发按键取卡变更
    void onIsAutoRespCommand52(bool res);

private:
    void dealCommand(uchar seq, const QByteArray &command);
    void sendResponse(uchar cmdType, uchar status, uchar seq);
    QByteArray constructSendData(const QByteArray &data);
    QString getVoiceContent(uchar voiceNum);

    // 循环生成0x81到0x89的序列号
    uchar getServerSeq();

    // 上电信息
    bool sendCommand65();
    // 状态信息
    bool sendCommand66();
    // 出卡信息
    bool sendCommand67(uchar workStation, uchar boxNo);
    // 按键取卡信息
    bool sendCommand68(uchar workStation, uchar boxNo);
    // 卡被取走信息
    bool sendCommand69(uchar workStation, uchar boxNo);
    // 卡夹信息
    bool sendCommand70(ushort cardNum1, ushort cardNum2, ushort cardNum3, ushort cardNum4);
    // 卡回收完成信息
    bool sendCommand71(uchar workStation, uchar boxNo);
    // 按键忽略信息
    bool sendCommand72(uchar workStation, uchar cause);
    // 纸券取走及状态信息
    bool sendCommand73(uchar workStation, uchar cause, QString paperNum, uchar statusUp, uchar statusDown);
    // 发送报文
    bool sendPacket(uchar cmdType, const QByteArray &sendData);

private:
    QTcpServer *m_server = nullptr;     // 服务端
    QTcpSocket *m_client = nullptr;     // 客户端连接
    bool m_connected = false;           // 是否与客户端建立连接
    bool m_isAutoRespCommand52 = false; // 是否自动应答触发按键取卡

    QByteArray m_recvBuffer; // 数据缓冲区

    int m_bagNo1; // 卡机1卡夹编号
    int m_bagNo2; // 卡机2卡夹编号
    int m_bagNo3; // 卡机3卡夹编号
    int m_bagNo4; // 卡机4卡夹编号

    uchar m_defWorkStation; // 默认工位
    uchar m_defUpBoxNo;     // 上工位默认卡机编号
    uchar m_defDownBoxNo;   // 下工位默认卡机编号

    ST_CardRobotStatusInfo m_statusInfo1; // 卡机1状态
    ST_CardRobotStatusInfo m_statusInfo2; // 卡机2状态
    ST_CardRobotStatusInfo m_statusInfo3; // 卡机3状态
    ST_CardRobotStatusInfo m_statusInfo4; // 卡机4状态
};
