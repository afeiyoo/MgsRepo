#pragma once

#include <QQueue>
#include <QTcpSocket>
#include <QTimer>

#include "ismartlanecontroller.h"

struct ST_SendTask;
class ICmdHandler;
class SmartLaneController : public ISmartLaneController
{
    Q_OBJECT
public:
    explicit SmartLaneController(QObject *parent = nullptr);
    ~SmartLaneController() override;

    void connectServer(const QString &ip, quint16 port) override;

    void disconnectServer() override;

    void setVersion(uchar ver) override;

    void sendA1Cmd(const QMap<int, int> &relayMap, const QMap<int, int> &levelMap) override;

    void sendA2Cmd(const QByteArray &url, uchar time) override;

    void sendA3Cmd(uchar type, const QByteArray &data) override;

private slots:
    void onStateChanged(QAbstractSocket::SocketState state);
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError error);

private:
    // 指令进入同步发送队列
    void enqueueCommand(uchar cmdType, const QByteArray &cmd);
    // 当前无等待应答的指令时，发送队首指令
    void trySendNextCommand();
    // 匹配A1/A2/A3应答，匹配成功后结束当前同步请求
    bool handleCommandResponse(uchar seq, uchar cmdType);
    // 当前队首指令应答超时处理
    void handleCommandResponseTimeout();
    // 获取PC端序列号
    uchar getClientSeq();
    // 处理指令
    void dealCommand(uchar seq, const QByteArray &cmd);
    // 组装完整帧
    QByteArray makeFrame(uchar seq, const QByteArray &cmd);
    // 发送帧
    bool sendFrame(const QByteArray &data);
    // 收到心跳后复位心跳看门狗
    void resetHeartbeatWatchdog();
    // 心跳超时处理
    void handleHeartbeatTimeout();
    // 重连设备
    void scheduleReconnect(int delayMs);
    // 尝试重连设备
    void attemptReconnect();

private:
    // 是否客户端主动断开连接
    bool m_isForceDisconnect = false;
    // 协议版本号
    uchar m_ver = 0x00; // 默认版本号0x00

    // 网络信息
    QTcpSocket *m_socket = nullptr;
    QTimer *m_heartbeatTimer = nullptr;
    QTimer *m_reconnectTimer = nullptr;
    QTimer *m_cmdResponseTimer = nullptr;
    QString m_peerAddr;
    quint16 m_peerPort = 0;
    bool m_connected = false; // 连接是否建立
    int m_reconnectCount = 0;
    bool m_reconnectFailureNotified = false;

    // 数据缓冲区
    QByteArray m_buffer;

    // 客户端序列号
    int m_nextSeq = 1;
    // A1/A2/A3同步发送队列，队首为当前等待应答的指令
    QQueue<ST_SendTask> m_sendQueue;
    bool m_waitingCmdResponse = false;
    // 指令解析器
    ICmdHandler *m_handler = nullptr;
};
