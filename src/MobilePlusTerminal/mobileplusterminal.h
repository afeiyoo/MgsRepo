#pragma once

#include <QObject>
#include <QTcpSocket>

#include "imobileplusterminal.h"

class ICmdHandler;
class MobilePlusTerminal : public IMobilePlusTerminal
{
    Q_OBJECT
public:
    explicit MobilePlusTerminal(QObject *parent = nullptr);
    ~MobilePlusTerminal() override;

    void connectServer(const QString &ip, quint16 port) override;

    void disconnectServer() override;

    void showQRCode(const QString &stationName, const QString &vehClass, const QString &vehPlate, const QString &barCode) override;

    void showLED(const QString &text) override;

    void showPics(const QString &data) override;

    void setUploadUrl(const QString &url, int time) override;

    void setVersion(uchar ver) override;

    // 设备初始化
    void initialize(const QString &stationID, uint laneID, uint seq) override;

private slots:
    void onStageChanged(QAbstractSocket::SocketState state);
    void onReadyRead();

private:
    bool sendFrame(const QByteArray &data);
    QByteArray makeFrame(uchar seq, const QByteArray &cmd);
    uchar getClientSeq();
    void dealCommand(uchar seq, const QByteArray &cmd);

private:
    // 是否客户端主动断开连接
    bool m_isForceDisconnect = false;
    // 协议版本号
    uchar m_ver = 0x01; // 默认版本号0x01
    // 网络信息
    bool m_connected = false;
    QTcpSocket *m_socket = nullptr;
    QString m_peerAddr;
    quint16 m_peerPort = 0;
    // 数据缓冲区
    QByteArray m_buffer;

    // 客户端序列号
    int m_nextSeq = 1;
    // 指令解析器
    ICmdHandler *m_handler = nullptr;
};
