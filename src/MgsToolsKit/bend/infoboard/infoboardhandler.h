#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

class InfoBoardHandler : public QObject
{
    Q_OBJECT

public:
    explicit InfoBoardHandler(QObject *parent = nullptr);
    ~InfoBoardHandler() override;

    // 连接折叠情报板
    bool connectServer(const QString &ip, quint16 port);
    // 断连折叠情报板
    void disconnectServer();

    // 播放列表下安并立即显示
    bool filePublishAndShowImmediate(const QByteArray &fileData, const QString &fileName = "play00.lst");

    // 文件下发
    bool filePublish(const QString &fileName);

    // 设置状态上报URL
    bool setupUrl(const QString &url, ushort interval = 10, bool isReport = false);

    // 是否启用重连机制
    void enableRetryConnect(bool isOn, ushort m_reconnectMaxTimes = 3);

    // 获取情报板连接状态
    bool isConnected() const;

    // 供http协议使用
    QByteArray constructSetupUrlFrame(const QString &url, ushort interval, bool isReport);
    QByteArray constructFilePublishAndShowImmediateFrame(const QByteArray &fileData, const QString &fileName = "play00.lst");

public slots:
    void onStateChanged(QAbstractSocket::SocketState state);
    void onReadyRead();
    void onTryConnect();
    void onErrorOccurred(QAbstractSocket::SocketError error);

private:
    uint genCRC(const uchar *buffer, int bufferLen);
    QByteArray constructSendData(const QByteArray &cmd, const QByteArray &data);

private:
    QString m_peerAddr;
    quint16 m_peerPort;
    bool m_connected = false;

    bool m_isEnableRetryConnect = false; // 是否启用重连机制

    QTcpSocket *m_socket = nullptr;

    QByteArray m_recvBuffer; // 数据缓冲区
    // 网络重连定时器
    QTimer *m_reconnectTimer = nullptr;
    quint32 m_reconnectCount = 0;
    ushort m_reconnectMaxTimes = 3;   // 默认重连3次
    bool m_isForceDisconnect = false; // 是否主动断连
};
