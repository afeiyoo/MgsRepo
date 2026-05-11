#include "infoboardhandler.h"

#include "Logger.h"
#include "global/globalmanager.h"
#include "global/signalmanager.h"
#include "qevent.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"

#include <QThread>

#define MAX_BUFF_SIZE 5 * 1024
#define MIN_FRAME_SIZE 11

using namespace Utils;

InfoBoardHandler::InfoBoardHandler(QObject *parent)
    : QObject{parent}
{
    m_socket = new QTcpSocket(this);

    connect(m_socket, &QTcpSocket::stateChanged, this, &InfoBoardHandler::onStateChanged);
    connect(m_socket, &QTcpSocket::readyRead, this, &InfoBoardHandler::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &InfoBoardHandler::onErrorOccurred);

    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setInterval(1000 * 60);
    connect(m_reconnectTimer, &QTimer::timeout, this, &InfoBoardHandler::onTryConnect);
}

InfoBoardHandler::~InfoBoardHandler()
{
    m_socket->disconnectFromHost();
}

bool InfoBoardHandler::connectServer(const QString &ip, quint16 port)
{
    m_isForceDisconnect = false; // 复位
    m_peerAddr = ip;
    m_peerPort = port;

    LOG_CINFO("infoboard").noquote() << QString("开始连接情报板(IP: %1, Port: %2)").arg(ip).arg(port);
    m_socket->connectToHost(ip, port);

    return true;
}

void InfoBoardHandler::disconnectServer()
{
    m_isForceDisconnect = true;
    m_reconnectTimer->stop();
    m_socket->disconnectFromHost();
}

bool InfoBoardHandler::filePublishAndShowImmediate(const QByteArray &fileData, const QString &fileName)
{
    if (!m_connected) {
        LOG_CWARNING("infoboard").noquote() << "网络异常，无法请求播放列表文件下发并立即显示";
        return false;
    }

    QByteArray sendData = constructFilePublishAndShowImmediateFrame(fileData, fileName);

    LOG_CINFO("infoboard").noquote() << QString("播放列表文件下发并立即显示: %1").arg(DataDealUtils::byteArrayToHexStr(sendData));
    qint64 ret = m_socket->write(sendData);
    if (ret == -1) {
        LOG_CWARNING("infoboard").noquote() << "数据写入失败";
        return false;
    }
    m_socket->flush();
    LOG_CINFO("infoboard").noquote() << "数据已进入发送队列";

    return true;
}

bool InfoBoardHandler::filePublish(const QString &fileName)
{
    if (!m_connected) {
        LOG_CWARNING("infoboard").noquote() << QString("网络异常，无法请求文件%1下发").arg(fileName);
        return false;
    }

    // 验证文件名格式是否符合 XXX.bmp (XXX为000-999)
    QRegExp fileNamePattern("^(\\d{3})\\.bmp$");
    if (!fileNamePattern.exactMatch(fileName)) {
        LOG_CWARNING("infoboard").noquote() << QString("位图文件名格式错误: %1").arg(fileName);
        return false;
    }
    int fileId = fileNamePattern.cap(1).toInt();
    if (fileId < 0 || fileId > 999) {
        LOG_CWARNING("infoboard").noquote() << QString("位图文件名ID异常: %1").arg(fileId);
        return false;
    }

    // 检查文件是否存在
    QString filePath = FileUtils::curApplicationDirPath() + "/files/" + fileName;
    QFile file(filePath);
    if (!file.exists()) {
        LOG_CWARNING("infoboard").noquote() << QString("位图文件不存在: %1").arg(filePath);
        return false;
    }

    // 打开并读取文件内容
    if (!file.open(QIODevice::ReadOnly)) {
        LOG_CWARNING("infoboard").noquote() << QString("无法打开位图文件: %1").arg(filePath);
        return false;
    }
    QByteArray fileData = file.readAll();
    file.close();

    QByteArray cmd;
    cmd.append('\x40');
    cmd.append('\x31');

    QByteArray data;
    data.append('\x2b');
    for (int i = 0; i < 8; ++i)
        data.append('\x30');

    QByteArray fileNameGBK = DataDealUtils::encodeString(fileName, 1);
    data.append(fileNameGBK);

    QByteArray fileDataGBK = DataDealUtils::encodeString(QString::fromUtf8(fileData), 1);
    data.append(fileDataGBK);

    QByteArray sendData = constructSendData(cmd, data);

    LOG_CINFO("infoboard").noquote() << QString("文件%1下发: %2").arg(fileName, DataDealUtils::byteArrayToHexStr(sendData));
    qint64 ret = m_socket->write(sendData);
    if (ret == -1) {
        LOG_CWARNING("infoboard").noquote() << "数据写入失败";
        return false;
    }
    m_socket->flush();
    LOG_CINFO("infoboard").noquote() << "数据已进入发送队列";

    return true;
}

bool InfoBoardHandler::setupUrl(const QString &url, ushort interval, bool isReport)
{
    if (!m_connected) {
        LOG_CWARNING("infoboard").noquote() << "网络异常，无法请求设置状态上报Url";
        return false;
    }

    QByteArray sendData = constructSetupUrlFrame(url, interval, isReport);

    LOG_CINFO("infoboard").noquote() << QString("设置状态上报Url请求: %1").arg(DataDealUtils::byteArrayToHexStr(sendData));
    qint64 ret = m_socket->write(sendData);
    if (ret == -1) {
        LOG_CWARNING("infoboard").noquote() << "数据写入失败";
        return false;
    }
    m_socket->flush();
    LOG_CINFO("infoboard").noquote() << "数据已进入发送队列";

    return true;
}

void InfoBoardHandler::enableRetryConnect(bool isOn, ushort times)
{
    m_isEnableRetryConnect = isOn;
    if (times > 3)
        m_reconnectMaxTimes = times;
}

void InfoBoardHandler::onStateChanged(QAbstractSocket::SocketState state)
{
    switch (state) {
    case QAbstractSocket::ConnectedState:
        LOG_CINFO("infoboard").noquote() << QString("连接情报板成功(IP: %1, port: %2)").arg(m_peerAddr).arg(m_peerPort);
        m_isForceDisconnect = false;
        m_connected = true;
        m_reconnectCount = 0;
        m_reconnectTimer->stop();
        emit GM_INSTANCE->m_signalMan->sigNetworkStatusChanged(true);
        break;
    case QAbstractSocket::UnconnectedState:
        LOG_CINFO("infoboard").noquote() << "情报板断开连接";
        m_connected = false;
        if (m_isEnableRetryConnect && !m_isForceDisconnect && m_reconnectCount < m_reconnectMaxTimes) {
            m_reconnectTimer->start();
        }
        emit GM_INSTANCE->m_signalMan->sigNetworkStatusChanged(false);
        break;
    default:
        break;
    }
}

void InfoBoardHandler::onReadyRead()
{
    QByteArray recvData = m_socket->readAll();
    LOG_CINFO("infoboard").noquote() << "接收到情报板数据:" << DataDealUtils::byteArrayToHexStr(recvData);

    if (m_recvBuffer.size() + recvData.size() >= MAX_BUFF_SIZE) {
        LOG_CWARNING("infoboard").noquote() << QString("数据缓冲区剩余大小不足，无法存储接收的数据(%1B)，忽略处理").arg(recvData.size());
        return;
    }

    m_recvBuffer.append(recvData);

    while (1) {
        int frameStart = m_recvBuffer.indexOf('\x02'); // 寻找帧头
        if (frameStart >= 0) {
            m_recvBuffer.remove(0, frameStart); // 0~frameStart-1前的数据为垃圾数据
        } else {
            m_recvBuffer.clear(); // 无帧头，则为垃圾数据
            return;
        }

        int frameEnd = m_recvBuffer.indexOf('\x03', frameStart + 1); // 寻找帧尾
        if (frameEnd < 0)
            return; // 数据不完整，等待更多后续数据到来

        QByteArray oneFrame = m_recvBuffer.mid(frameStart, frameEnd - frameStart + 1);
        m_recvBuffer.remove(0, frameEnd + 1);   // 从缓冲区中移除已处理部分
        if (oneFrame.size() < MIN_FRAME_SIZE) { // NOTE：目前完整帧最小长度为11B
            LOG_CWARNING("infoboard").noquote() << QString("收到异常帧(长度过短): %1").arg(oneFrame.size());
            continue;
        }

        // 解转义
        QByteArray payload = oneFrame.mid(1, oneFrame.size() - 2); // 不包含帧头和帧尾
        QByteArray unescaped;
        for (int i = 0; i < payload.size(); ++i) {
            uchar ch = static_cast<uchar>(payload[i]);
            if (ch == 0x1B) { // ESC
                if (i + 1 < payload.size()) {
                    uchar next = static_cast<uchar>(payload[i + 1]);
                    if (next == 0xE7)
                        unescaped.append('\x02'); // STX
                    else if (next == 0xE8)
                        unescaped.append('\x03'); // ETX
                    else if (next == 0x00)
                        unescaped.append('\x1B'); // ESC
                    i++;                          // 跳过下一个字节
                }
            } else {
                unescaped.append(ch);
            }
        }

        // CRC校验
        QByteArray dataWithoutCrc = unescaped.left(unescaped.size() - 2);
        QByteArray crcBytes = unescaped.right(2);
        quint16 crcRecv = static_cast<quint8>(crcBytes[0]) << 8 | static_cast<quint8>(crcBytes[1]);
        quint16 crcCalc = genCRC(reinterpret_cast<const unsigned char *>(dataWithoutCrc.constData()), dataWithoutCrc.size());
        if (crcCalc != crcRecv) {
            LOG_CWARNING("infoboard").noquote() << QString("CRC校验失败: 接收CRC=%1, 计算CRC=%2, 数据=%3")
                                                       .arg(crcRecv, 4, 16, QChar('0'))
                                                       .arg(crcCalc, 4, 16, QChar('0'))
                                                       .arg(DataDealUtils::byteArrayToHexStr(unescaped));
            continue;
        }

        QByteArray cmdType = unescaped.mid(4, 2);
        uchar result = static_cast<uchar>(unescaped[6]);
        if (cmdType == QByteArray("\x37\x32", 2)) { // 播放列表下发并立即显示
            if (result == 0x30) {
                LOG_CINFO("infoboard").noquote() << "播放列表下发并立即显示异常";
            } else if (result == 0x31) {
                LOG_CINFO("infoboard").noquote() << "播放列表下发并立即显示成功";
            } else {
                LOG_CINFO("infoboard").noquote() << QString("播放列表下发返回未知状态码: 0x%1").arg(result, 2, 16, QChar('0')).toUpper();
            }
        } else if (cmdType == QByteArray("\x40\x32", 2)) { // 文件下发
            if (result == 0x30) {
                LOG_CINFO("infoboard").noquote() << "文件下发异常";
            } else if (result == 0x31) {
                LOG_CINFO("infoboard").noquote() << "文件下发成功";
            } else {
                LOG_CINFO("infoboard").noquote() << QString("文件下发返回未知状态码: 0x%1").arg(result, 2, 16, QChar('0')).toUpper();
            }
        } else if (cmdType == QByteArray("\x38\x32", 2)) { // 设置状态上报URL
            if (result == 0x30) {
                LOG_CINFO("infoboard").noquote() << "设置状态上报URL异常";
            } else if (result == 0x31) {
                LOG_CINFO("infoboard").noquote() << "设置状态上报URL成功";
            } else {
                LOG_CINFO("infoboard").noquote() << QString("设置状态上报URL返回未知状态码: 0x%1").arg(result, 2, 16, QChar('0')).toUpper();
            }
        } else if (cmdType == QByteArray("\x39\x31", 2)) { // 状态上报
            if (unescaped.size() < 50) {
                LOG_CINFO("infoboard").noquote() << QString("状态上报数据长度异常: %1").arg(DataDealUtils::byteArrayToHexStr(unescaped));
                continue;
            }

            QByteArray upText = unescaped.mid(6, 20);
            QByteArray downText = unescaped.mid(26, 20);
            uchar upState = static_cast<uchar>(unescaped[46]);
            uchar downState = static_cast<uchar>(unescaped[47]);
            LOG_CINFO("infoboard").noquote() << QString("状态上报：上屏%2，下屏%2")
                                                    .arg(upState == 0x30 ? "正常" : "异常", downState == 0x30 ? "正常" : "异常");
            LOG_CINFO("infoboard").noquote() << QString("上屏内容: %1，下屏内容: %2")
                                                    .arg(DataDealUtils::byteArrayToHexStr(upText), DataDealUtils::byteArrayToHexStr(downText));

            // 下屏协议无效
            if (upState == 0x30) {
                emit GM_INSTANCE->m_signalMan->sigDevStatusChanged(true);
            } else {
                emit GM_INSTANCE->m_signalMan->sigDevStatusChanged(false);
            }
        } else {
            LOG_CWARNING("infoboard").noquote() << QString("未知的命令类型: %1").arg(DataDealUtils::byteArrayToHexStr(cmdType));
        }
    }
}

void InfoBoardHandler::onTryConnect()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) { // 连接成功，则停止重连
        m_reconnectTimer->stop();
        return;
    }

    if (m_reconnectCount >= m_reconnectMaxTimes) {
        LOG_CWARNING("infoboard").noquote() << QString("重连次数已达上限(%1次)，停止自动重连").arg(m_reconnectMaxTimes);
        m_reconnectTimer->stop(); // 停止定时器，不再尝试
        return;
    }

    LOG_CINFO("infoboard").noquote() << QString("第%1次尝试重连情报板").arg(++m_reconnectCount);
    m_socket->abort();
    connectServer(m_peerAddr, m_peerPort);
}

void InfoBoardHandler::onErrorOccurred(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)

    LOG_CWARNING("infoboard").noquote() << QString("Socket错误: %1").arg(m_socket->errorString());
}

uint InfoBoardHandler::genCRC(const uchar *buffer, int bufferLen)
{
    uchar c, treat, bcrc;
    uint wcrc = 0;
    short i, j;
    for (i = 0; i < bufferLen; i++) {
        c = buffer[i];
        for (j = 0; j < 8; j++) {
            treat = c & 0x80;
            c <<= 1;
            bcrc = (wcrc >> 8) & 0x80;
            wcrc <<= 1;
            if (treat != bcrc)
                wcrc ^= 0x1021;
        }
    }
    return wcrc;
}

QByteArray InfoBoardHandler::constructSendData(const QByteArray &cmd, const QByteArray &data)
{
    QByteArray sendData;
    sendData.append('\x02'); // 帧头

    // 帧内容
    QByteArray frameContent;
    frameContent.append('\x30'); // 目的地址
    frameContent.append('\x30');
    frameContent.append('\x30'); // 源地址
    frameContent.append('\x31');
    frameContent.append(cmd);  // 控制码
    frameContent.append(data); // 数据域

    // 校验码
    uint crc = genCRC(reinterpret_cast<const uchar *>(frameContent.constData()), frameContent.length());
    frameContent.append(static_cast<char>(crc >> 8));
    frameContent.append(static_cast<char>(crc & 0xFF));

    // 字符转义
    QByteArray escapedContent;
    for (const auto &byte : frameContent) {
        switch (static_cast<uchar>(byte)) {
        case 0x02:
            escapedContent.append('\x1B');
            escapedContent.append('\xE7');
            break;
        case 0x03:
            escapedContent.append('\x1B');
            escapedContent.append('\xE8');
            break;
        case 0x1B:
            escapedContent.append('\x1B');
            escapedContent.append('\x00');
            break;
        default:
            escapedContent.append(byte);
            break;
        }
    }

    sendData.append(escapedContent);
    sendData.append('\x03'); // 帧尾
    return sendData;
}

bool InfoBoardHandler::isConnected() const
{
    return m_connected;
}

QByteArray InfoBoardHandler::constructSetupUrlFrame(const QString &url, ushort interval, bool isReport)
{
    QByteArray cmd;
    cmd.append('\x38');
    cmd.append('\x31');

    QByteArray data;
    if (isReport) {
        data.append('\x31');
    } else {
        data.append('\x30');
    }
    data.append(DataDealUtils::ushortToByte(interval));
    data.append(DataDealUtils::intToByte(url.size()));
    data.append(url.toUtf8());

    QByteArray sendData = constructSendData(cmd, data);
    return sendData;
}

QByteArray InfoBoardHandler::constructFilePublishAndShowImmediateFrame(const QByteArray &fileData, const QString &fileName /*= "play00.lst"*/)
{
    QByteArray cmd;
    cmd.append('\x37');
    cmd.append('\x31');

    QByteArray data;
    data.append('\x2b');
    for (int i = 0; i < 8; ++i)
        data.append('\x30');

    QByteArray fileNameGBK = DataDealUtils::encodeString(fileName, 1);
    data.append(fileNameGBK);

    // NOTE: 源文件使用Utf-8编码，所以这里通过fromUtf8转成QString
    QByteArray fileDataGBK = DataDealUtils::encodeString(QString::fromUtf8(fileData), 1);
    data.append(fileDataGBK);

    QByteArray sendData = constructSendData(cmd, data);
    return sendData;
}
