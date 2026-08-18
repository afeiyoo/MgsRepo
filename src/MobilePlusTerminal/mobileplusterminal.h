#pragma once

#include <QHash>
#include <QObject>
#include <QTcpSocket>

#include "defines.h"
#include "imobileplusterminal.h"

class ICmdHandler;
class QTimer;
class MobilePlusTerminal : public IMobilePlusTerminal
{
    Q_OBJECT
public:
    explicit MobilePlusTerminal(const QString &stationID, uint laneID, uint devSeq, QObject *parent = nullptr);
    ~MobilePlusTerminal() override;

    void connectServer(const QString &ip, quint16 port) override;

    void disconnectServer() override;

    void showQRCode(const QString &stationName, const QString &vehClass, const QString &vehPlate, const QString &barCode) override;

    void showLED(const QString &text) override;

    void showPics(const QByteArray &data) override;

    void setUploadUrl(const QString &url, int time) override;

    void setVersion(uchar ver) override;

private slots:
    void onStageChanged(QAbstractSocket::SocketState state);
    void onReadyRead();

private:
    void initialize(const QString &stationID, uint laneID, uint seq);

    bool sendA1Command(uchar type, const QByteArray &jsonData, bool requiresInitialized = true);
    bool sendFrame(const QByteArray &data);
    QByteArray makeFrame(uchar seq, const QByteArray &cmd);
    uchar getClientSeq();
    void dealCommand(uchar seq, const QByteArray &cmd);
    void handleF1Response(uchar seq, const QByteArray &cmd);
    void handleRequestTimeout(const QByteArray &requestKey);
    void resetHeartbeatWatchdog();
    void handleHeartbeatTimeout();
    void scheduleReconnect(int delayMs);
    void attemptReconnect();
    QString deviceLogTag() const;

private:
    QString m_stationID;
    uint m_laneID = 0;
    uint m_devSeq = 0;

    // 是否客户端主动断开连接
    bool m_isForceDisconnect = false;
    // 协议版本号
    uchar m_ver = 0x01; // 默认版本号0x01
    // 网络信息
    bool m_connected = false;
    bool m_initialized = false;
    QTcpSocket *m_socket = nullptr;
    QTimer *m_heartbeatTimer = nullptr;
    QTimer *m_reconnectTimer = nullptr;
    QString m_peerAddr;
    quint16 m_peerPort = 0;
    int m_reconnectCount = 0;
    bool m_reconnectFailureNotified = false;
    // 数据缓冲区
    QByteArray m_buffer;

    // 客户端序列号
    int m_nextSeq = 1;
    // 等待F1应答的A1请求，键为“PC端序列号 + DateTime + Type”
    QHash<QByteArray, ST_PendingRequest> m_pendingRequests;
    // 指令解析器
    ICmdHandler *m_handler = nullptr;
};
