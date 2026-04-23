#include "smartlanecontroller.h"

#include <QNetworkProxy>

#include "Logger.h"
#include "RollingFileAppender.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"

using namespace Utils;

static const QByteArray STX = QByteArray::fromHex("FFFF"); // 帧头
static const QByteArray VER = QByteArray::fromHex("00");   // 版本号
static const int STX_LEN = 2;                              // 帧头 2 字节
static const int VER_LEN = 1;                              // 版本号 1 字节，固定为 0x00
static const int SEQ_LEN = 1;                              // 序列号 1 字节（服务端格式：0x0X, X=1~9）
static const int LEN_FIELD_LEN = 4;                        // 数据长度字段 4 字节（大端）
static const int CRC_LEN = 2;                              // CRC 校验码 2 字节

// 允许的最大数据长度（4MB），超过则判为非法
static const int MAX_BUFF_SIZE = 4 * 1024 * 1024;

SmartLaneController::SmartLaneController(QObject *parent)
    : QObject{parent}
{
    // 日志初始化
#ifndef SMARTLANECONTROLLER_NO_CREATE_LOG
    FileName logPath = FileName::fromString(Utils::FileUtils::curApplicationDirPath() + "/log/SmartLaneCtrl.log");
    FileUtils::makeSureDirExist(logPath.parentDir());
    RollingFileAppender *rollingFileAppender = new RollingFileAppender(FileUtils::canonicalPath(logPath).toString());
    rollingFileAppender->setFormat("%{time} [%{type}] [%{threadid}] %{message}\n\n");
    rollingFileAppender->setLogFilesLimit(180);
    rollingFileAppender->setFlushOnWrite(true);
    rollingFileAppender->setDatePattern(RollingFileAppender::DatePattern::DailyRollover);
    cuteLogger->registerCategoryAppender("smartctrl", rollingFileAppender);
#endif

    m_tcpSocket = new QTcpSocket(this);
    m_tcpSocket->setProxy(QNetworkProxy::NoProxy);

    connect(m_tcpSocket, &QTcpSocket::stateChanged, this, &SmartLaneController::onStateChanged);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &SmartLaneController::onReadyRead);

    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setInterval(1000 * 30);
    connect(m_reconnectTimer, &QTimer::timeout, this, &SmartLaneController::onTryConnect);
}

SmartLaneController::~SmartLaneController()
{
    m_tcpSocket->disconnectFromHost();
}

bool SmartLaneController::connectServer(const QString &ip, quint16 port)
{
    m_isForceDisconnect = false; // 复位
    // 连接服务端
    m_peerAddr = ip;
    m_peerPort = port;
    m_tcpSocket->connectToHost(ip, port);
    if (!m_tcpSocket->waitForConnected(5 * 1000)) {
        LOG_CWARNING("smartctrl").noquote() << "无法连接智能网关";
        return false;
    }
    return true;
}

void SmartLaneController::disconnectServer()
{
    m_isForceDisconnect = true;
    m_reconnectTimer->stop();
    m_tcpSocket->disconnectFromHost();
}

bool SmartLaneController::isConnected() const
{
    return m_connected;
}

void SmartLaneController::enableRetryConnect(bool isOn, int times /*= 3*/)
{
    m_isEnableRetryConnect = isOn;
    if (times > 3)
        m_reconnectMaxTimes = times;
}

void SmartLaneController::onStateChanged(QAbstractSocket::SocketState state)
{
    switch (state) {
    case QAbstractSocket::ConnectedState:
        LOG_CINFO("smartctrl").noquote() << QString("连接智能网关成功(IP: %1, port: %2)").arg(m_peerAddr).arg(m_peerPort);
        m_isForceDisconnect = false;
        m_connected = true;
        m_reconnectCount = 0;
        m_reconnectTimer->stop();
        emit sigNetworkStatusChanged(true);
        break;
    case QAbstractSocket::UnconnectedState:
        LOG_CINFO("smartctrl").noquote() << "智能网关断开连接";
        m_connected = false;
        if (m_isEnableRetryConnect && !m_isForceDisconnect && m_reconnectCount < m_reconnectMaxTimes) {
            m_reconnectTimer->start();
        }
        emit sigNetworkStatusChanged(false);
        break;
    default:
        break;
    }
}

void SmartLaneController::onTryConnect()
{
    if (m_tcpSocket->state() == QAbstractSocket::ConnectedState) { // 连接成功，则停止重连
        m_reconnectTimer->stop();
        return;
    }

    if (m_reconnectCount >= m_reconnectMaxTimes) {
        LOG_CWARNING("smartctrl").noquote() << QString("重连次数已达上限(%1次)，停止自动重连").arg(m_reconnectMaxTimes);
        m_reconnectTimer->stop(); // 停止定时器，不再尝试
        return;
    }

    LOG_CINFO("smartctrl").noquote() << QString("第%1次尝试重连智能网关").arg(++m_reconnectCount);
    m_tcpSocket->abort();
    connectServer(m_peerAddr, m_peerPort);
}

void SmartLaneController::sendCommand(const QString &type, QByteArray data)
{
    if (!m_connected) {
        LOG_CERROR("smartctrl").noquote() << QString("向智能网关发送%1帧失败: 未与服务端建立连接").arg(type);
        return;
    }

    // 组装发送帧
    if (type == "A3") {
        data.prepend(DataDealUtils::intToByte(data.size()));
        data.prepend(char(0x01));
        data.prepend(char(0xA3));
    }

    QByteArray buffer;
    buffer.append(STX);
    buffer.append(VER);
    uchar seq = getClientSeq();
    buffer.append(seq);
    buffer.append(DataDealUtils::intToByte(data.size()));
    buffer.append(data);
    QByteArray crc = DataDealUtils::getCRCCode(buffer.mid(STX_LEN));
    buffer.append(crc);

    LOG_CINFO("smartctrl").noquote() << QString("向智能网关发送%1帧: %2").arg(type, DataDealUtils::byteArrayToHexStr(buffer));
    m_tcpSocket->write(buffer);

    // 设置超时监听队列，超时未返回，则重发3次
    QString key = makeKey(type, seq);
    if (!m_pendingCommands.contains(key)) {
        ST_PendingCommand *cmd = new ST_PendingCommand(type, data, seq, 0, new QTimer(this));
        m_pendingCommands[key] = cmd;

        cmd->timer->setSingleShot(true);
        connect(cmd->timer, &QTimer::timeout, this, [=]() {
            LOG_CINFO("smartctrl").noquote() << "测试日志: " << "超时重发" << cmd->retryCount;
            if (cmd->retryCount < 3) {
                cmd->retryCount += 1;
                LOG_CINFO("smartctrl").noquote() << QString("向智能网关发送%1帧超时，进行第%2次重传").arg(type).arg(cmd->retryCount);
                sendCommandWithSeq(cmd->type, cmd->data, cmd->seq);
                cmd->timer->start(1500);
            } else {
                LOG_CWARNING("smartctrl").noquote() << QString("%1帧重发3次失败，放弃发送").arg(type);
                m_pendingCommands.remove(key);
                cmd->timer->stop();
                delete cmd;
            }
        });

        cmd->timer->start(1500); // 第一次启动超时检测
    }
}

void SmartLaneController::onReadyRead()
{
    QByteArray recvData = m_tcpSocket->readAll();
    LOG_CINFO("smartctrl").noquote() << "接收到智能网关数据:" << DataDealUtils::byteArrayToHexStr(recvData);

    if (m_buffer.size() + recvData.size() >= MAX_BUFF_SIZE) {
        LOG_CINFO("smartctrl").noquote() << QString("数据缓冲区现有大小(%1B) + 接收的数据大小(%2B) 超过数据缓冲区大小限制(%3B), 忽略处理")
                                                .arg(m_buffer.size())
                                                .arg(recvData.size())
                                                .arg(MAX_BUFF_SIZE);
        return;
    }

    m_buffer.append(recvData);
    while (true) {
        // 在 m_buffer 中查找第一对连续的 0xFF,0xFF。找到，则0~idx-1前的数据为垃圾数据。没有找到帧头，则缓冲区数据无效，直接丢弃
        int idx = m_buffer.indexOf(STX);
        if (idx >= 0) {
            m_buffer.remove(0, idx);
        } else {
            m_buffer.clear(); // 无帧头，则为垃圾数据
            return;
        }

        // 验证版本号是否为 0x00
        uchar ver = static_cast<uchar>(m_buffer.at(2));
        if (ver != 0x00) {
            LOG_CERROR("smartctrl").noquote() << "版本号错误";
            m_buffer.remove(0, STX_LEN);
            continue;
        }

        // 验证序列号是否正确
        uchar seq = static_cast<uchar>(m_buffer.at(3));
        bool ok = ((seq & 0xF0) == 0x00) && ((seq & 0x0F) >= 1 && (seq & 0x0F) <= 9);
        if (!ok) {
            LOG_CERROR("smartctrl").noquote() << QString("序列号错误");
            m_buffer.remove(0, STX_LEN);
            continue;
        }

        // 解析数据长度
        quint32 dataLen = DataDealUtils::byteToUInt(m_buffer.mid(STX_LEN + VER_LEN + SEQ_LEN, LEN_FIELD_LEN));

        // 计算数据总大小（单位：B）
        quint32 totalSize = STX_LEN + VER_LEN + SEQ_LEN + LEN_FIELD_LEN + dataLen + CRC_LEN;
        // 数据长度不足，等待更多后续数据到来
        if ((quint32) m_buffer.size() < totalSize)
            return;

        QByteArray fullFrame = m_buffer.left(totalSize);
        // 进行CRC校验
        QByteArray remoteCrc = fullFrame.right(CRC_LEN);
        QByteArray localCrc = DataDealUtils::getCRCCode(fullFrame.mid(STX_LEN, totalSize - STX_LEN - CRC_LEN));

        if (remoteCrc != localCrc) {
            QString localCrcStr = QString::fromLatin1(localCrc.toHex()).toUpper();
            QString remoteCrcStr = QString::fromLatin1(remoteCrc.toHex()).toUpper();
            LOG_CERROR("smartctrl").noquote() << QString("CRC校验失败(localCrc: %1, remoteCrc: %2)").arg(localCrcStr).arg(remoteCrcStr);
            m_buffer.remove(0, STX_LEN);
            continue;
        }

        QByteArray command = fullFrame.mid(STX_LEN + VER_LEN + SEQ_LEN + LEN_FIELD_LEN, dataLen);
        // 处理单条指令
        dealCommand(seq, command);
        m_buffer.remove(0, totalSize);
        continue;
    }
}

void SmartLaneController::dealCommand(uchar seq, const QByteArray &command)
{
    uchar cmdType = static_cast<uchar>(command.at(0));
    LOG_CINFO("smartctrl").noquote() << QString("开始处理指令(%1): %2")
                                            .arg(QString("0x%1").arg(cmdType, 2, 16, QLatin1Char('0')).toUpper())
                                            .arg(DataDealUtils::byteArrayToHexStr(command));

    switch (cmdType) {
    case 0xD2:
    case 0xD3:
    case 0xD6: {
        // IO状态信息,透传接收信息,心跳及设备状态
        sendResponse(cmdType, 0x01, seq << 4);
        emit sigRecvFromSmartLaneController(cmdType, command);
    } break;
    case 0xA1: {
        handleACmd("A1", seq << 4, command);
    } break;
    case 0xA2: {
        handleACmd("A2", seq << 4, command);
    } break;
    case 0xA3: {
        handleACmd("A3", seq << 4, command);
    } break;
    default: {
        LOG_CINFO("smartctrl").noquote() << "未知的命令类型:" << QString("0x%1").arg(cmdType, 2, 16, QChar('0')).toUpper();
    } break;
    }
}

void SmartLaneController::sendResponse(uchar cmdType, uchar status, uchar seq)
{
    if (!m_connected) {
        LOG_CERROR("smartctrl").noquote() << "返回应答失败: 连接未建立";
        return;
    }

    QByteArray command;
    command.append(cmdType);
    command.append(status);

    // 组装发送帧
    QByteArray buffer;
    buffer.append(STX);
    buffer.append(VER);
    buffer.append(seq);
    buffer.append(DataDealUtils::intToByte(command.size()));
    buffer.append(command);
    QByteArray crc = DataDealUtils::getCRCCode(buffer.mid(STX_LEN));
    buffer.append(crc);

    LOG_CINFO("smartctrl").noquote() << QString("返回应答: %1").arg(DataDealUtils::byteArrayToHexStr(buffer));
    m_tcpSocket->write(buffer);
}

uchar SmartLaneController::getClientSeq()
{
    static uchar x = 1; // X 范围 1~9

    uchar ret = (x << 4); // X0H，其实就是 x * 0x10

    x++;
    if (x > 9) {
        x = 1; // 回到 1
    }

    return ret;
}

QString SmartLaneController::makeKey(const QString &type, uchar seq)
{
    return QString("%1_%2").arg(type).arg(seq, 2, 16, QLatin1Char('0'));
}

void SmartLaneController::sendCommandWithSeq(const QString &type, const QByteArray &data, uchar seq)
{
    if (!m_connected) {
        LOG_CERROR("smartctrl").noquote() << QString("向智能网关发送%1帧[%2]失败: 未与服务端建立连接").arg(type, makeKey(type, seq));
        return;
    }

    QByteArray buffer;
    buffer.append(STX);
    buffer.append(VER);
    buffer.append(seq);
    buffer.append(DataDealUtils::intToByte(data.size()));
    buffer.append(data);

    QByteArray crc = DataDealUtils::getCRCCode(buffer.mid(STX_LEN));
    buffer.append(crc);

    LOG_CINFO("smartctrl").noquote() << QString("向智能网关发送%1帧: %2").arg(type, DataDealUtils::byteArrayToHexStr(buffer));
    m_tcpSocket->write(buffer);
}

void SmartLaneController::handleACmd(const QString &type, uchar seq, const QByteArray &command)
{
    QString key = makeKey(type, seq);

    if (!m_pendingCommands.contains(key)) {
        LOG_CINFO("smartctrl").noquote() << QString("对应的%1指令[%2]已处理或已超时").arg(type).arg(seq, 2, 16, QLatin1Char('0'));
        return;
    }

    int status = static_cast<uchar>(command.at(1));
    if (status == 0x01) {
        LOG_CINFO("smartctrl").noquote() << QString("智能网关返回%1指令[%2]处理失败").arg(type).arg(seq, 2, 16, QLatin1Char('0'));
    } else {
        LOG_CINFO("smartctrl").noquote() << QString("智能网关返回%1指令[%2]处理成功").arg(type).arg(seq, 2, 16, QLatin1Char('0'));
    }

    auto *cmd = m_pendingCommands[key];
    cmd->timer->stop();
    delete cmd;
    m_pendingCommands.remove(key);
}
