#pragma once

#include "ivehrecognizer.h"

#include <QHash>
#include <QTcpSocket>
#include <QTimer>

class ICmdHandler;
struct ST_PendingRequest;
struct ST_EAHandleResult;
struct ST_EBHandleResult;
class VehRecognizer : public IVehRecognizer
{
    Q_OBJECT
public:
    explicit VehRecognizer(const QString &stationID, const QString &stationName, uint laneID, QObject *parent = nullptr);
    ~VehRecognizer() override;

    void connectServer(const QString &ip, quint16 port) override;

    void disconnectServer() override;

    void setVersion(uchar ver) override;

    void setUploadUrl(const QString &url, uchar minutes) override;

    void showLED(uchar color, const QString &data) override;

    void playVoice(uchar count, const QString &text, int intervalMs) override;

private slots:
    void onStateChanged(QAbstractSocket::SocketState state);
    void onReadyRead();

private:
    void initialize();

    void scheduleReconnect(int delayMs);
    void attemptReconnect();

    QByteArray makeFrame(uchar seq, const QByteArray &cmd);
    bool sendCommand(const QByteArray &cmd);
    bool sendFrame(const QByteArray &frame);
    QByteArray makeRequestKey(uchar seq, uchar type) const;
    void handleRequestTimeout(const QByteArray &requestKey);

    uchar getClientSeq();

    void dealCommand(uchar seq, const QByteArray &cmd);

    void handleResponse(uchar seq, const QByteArray &cmd);

    void resetHeartbeatWatchdog();
    void handleHeartbeatTimeout();

    QString saveVehicleMedia(const ST_EBHandleResult &result) const;
    void emitVehicleTypeInfo(const ST_EAHandleResult &result);
    void emitVehicleImageInfo(const ST_EBHandleResult &result, const QString &absolutePath);

private:
    QString m_stationName;
    QString m_stationID;
    uint m_laneID = 0;

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
    // 等待设备应答的请求，键为“PC端序列号 + 指令类型”
    QHash<QByteArray, ST_PendingRequest> m_pendingRequests;
    // 指令解析器
    ICmdHandler *m_handler = nullptr;
};
