#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

#include "smartlanecontroller_global.h"

class SMARTLANECONTROLLER_EXPORT SmartLaneController : public QObject
{
    Q_OBJECT
public:
    struct ST_PendingCommand
    {
        QString type;
        uchar seq;
        int retryCount;
        QTimer *timer = nullptr;

        ST_PendingCommand(const QString &t, uchar s, int r, QTimer *tm)
            : type(t)
            , seq(s)
            , retryCount(r)
            , timer(tm)
        {}
    };

    explicit SmartLaneController(QObject *parent = nullptr);
    virtual ~SmartLaneController() override;

    // 建立连接（与智能网关）
    bool connectServer(const QString &ip, quint16 port);
    // 断开连接（与智能网关）
    void disconnectServer();
    // 向智能网关发送指令 type：指令类型  data：指令内容
    void sendCommand(const QString &type, QByteArray data);

    // 获取网络连接状态
    bool isConnected() const;
    // 设置自动重连
    void enableRetryConnect(bool isOn, int times = 3);

public slots:
    void onStateChanged(QAbstractSocket::SocketState state);
    void onTryConnect();
    void onErrorOccurred(QAbstractSocket::SocketError error);

    void onReadyRead();

signals:
    void sigRecvFromSmartLaneController(uchar type, QByteArray data); // 智能网关消息
    void sigNetworkStatusChanged(bool status);                        // 网络连接状态

private:
    void dealCommand(uchar seq, const QByteArray &command);
    void sendResponse(uchar cmdType, uchar status, uchar seq);
    uchar getClientSeq();
    QString makeKey(const QString &type, uchar seq);
    void handleACmd(const QString &type, uchar seq, const QByteArray &command);

private:
    bool m_isForceDisconnect = false; // 是否客户端主动断连
    bool m_connected = false;         // 网络连接状态

    int m_reconnectCount = 0;            // 重连次数
    QTimer *m_reconnectTimer = nullptr;  // 重连定时器
    int m_reconnectMaxTimes = 3;         // 最大重连次数
    bool m_isEnableRetryConnect = false; // 是否启用掉线重连机制

    QTcpSocket *m_tcpSocket = nullptr;
    QString m_peerAddr;
    quint16 m_peerPort;
    QByteArray m_buffer; // 用于缓存从 socket 中读到但还未解析完的数据（处理粘包问题）

    QMap<QString, ST_PendingCommand *> m_pendingCommands; // 重发等待队列

    QMap<QString, QByteArray> m_lastCommand;
};
