#include "smartlanecontroller.h"

#include <QNetworkProxy>

#include "Logger.h"
#include "cmdhandler.h"
#include "defines.h"
#include "utils/datadealutils.h"

using namespace Utils;

SmartLaneController::SmartLaneController(QObject *parent)
    : ISmartLaneController{parent}
{
    m_socket = new QTcpSocket(this);
    m_socket->setProxy(QNetworkProxy::NoProxy);

    m_heartbeatTimer = new QTimer(this);
    m_heartbeatTimer->setSingleShot(true);

    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setSingleShot(true);

    m_cmdResponseTimer = new QTimer(this);
    m_cmdResponseTimer->setSingleShot(true);

    m_handler = new CmdHandlerV0();

    connect(m_socket, &QTcpSocket::stateChanged, this, &SmartLaneController::onStateChanged);
    connect(m_socket, &QTcpSocket::readyRead, this, &SmartLaneController::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &SmartLaneController::onErrorOccurred);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &SmartLaneController::handleHeartbeatTimeout);
    connect(m_reconnectTimer, &QTimer::timeout, this, &SmartLaneController::attemptReconnect);
    connect(m_cmdResponseTimer, &QTimer::timeout, this, &SmartLaneController::handleCommandResponseTimeout);
}

SmartLaneController::~SmartLaneController()
{
    delete m_handler;
}

void SmartLaneController::connectServer(const QString &ip, quint16 port)
{
    m_isForceDisconnect = false;
    m_reconnectCount = 0;
    m_reconnectFailureNotified = false;
    m_reconnectTimer->stop();

    m_peerAddr = ip;
    m_peerPort = port;

    LOG_CINFO(L_CATE).noquote() << QString("开始连接智能网关(IP: %1, Port: %2)").arg(ip).arg(port);
    m_socket->connectToHost(ip, port);
}

void SmartLaneController::disconnectServer()
{
    m_isForceDisconnect = true;
    m_heartbeatTimer->stop();
    m_reconnectTimer->stop();
    m_cmdResponseTimer->stop();
    m_socket->disconnectFromHost();
}

void SmartLaneController::setVersion(uchar ver)
{
    if (ver == 0x00) {
        delete m_handler;
        m_handler = new CmdHandlerV0();
    } else {
        LOG_CERROR(L_CATE).noquote() << "不支持的协议版本:" << ver;
        return;
    }
    m_ver = ver;
}

void SmartLaneController::sendA1Cmd(const QMap<int, int> &relayMap, const QMap<int, int> &levelMap)
{
    QByteArray cmd = m_handler->assembleA1Cmd(relayMap, levelMap);
    enqueueCommand(0xA1, cmd);
}

void SmartLaneController::sendA2Cmd(const QByteArray &url, uchar time)
{
    QByteArray cmd = m_handler->assembleA2Cmd(url, time);
    enqueueCommand(0xA2, cmd);
}

void SmartLaneController::sendA3Cmd(uchar type, const QByteArray &data)
{
    QByteArray cmd = m_handler->assembleA3Cmd(type, data);
    enqueueCommand(0xA3, cmd);
}

void SmartLaneController::onStateChanged(QAbstractSocket::SocketState state)
{
    switch (state) {
    case QAbstractSocket::ConnectedState: {
        LOG_CINFO(L_CATE).noquote() << "与智能网关建立连接";
        m_connected = true;

        emit sigConnectionStateChanged(true);
        m_heartbeatTimer->start(HEARTBEAT_TIMEOUT);
    } break;
    case QAbstractSocket::UnconnectedState: {
        LOG_CERROR(L_CATE).noquote() << "与智能网关断开连接";
        m_connected = false;
        m_waitingCmdResponse = false;

        m_heartbeatTimer->stop();
        m_cmdResponseTimer->stop();
        m_buffer.clear();    // 清空数据缓冲区
        m_sendQueue.clear(); // 清空指令队列

        emit sigConnectionStateChanged(false);
        // 非主动断开连接，则尝试重连设备
        if (!m_isForceDisconnect)
            scheduleReconnect(RECONNECT_INTERVAL);
    } break;
    default:
        break;
    }
}

void SmartLaneController::onReadyRead()
{
    QByteArray recvData = m_socket->readAll();
    LOG_CINFO(L_CATE).noquote() << QString("【RX】[%1:%2]").arg(m_peerAddr).arg(m_peerPort) << DataDealUtils::byteArrayToHexStr(recvData);

    LOG_CINFO(L_CATE).noquote() << "数据缓冲区现有数据长度(Byte):" << m_buffer.size() << "数据缓冲区最大数据长度(Byte):" << MAX_BUFF_SIZE
                                << "接收到的数据长度(Byte):" << recvData.size();
    if (recvData.size() > MAX_BUFF_SIZE || m_buffer.size() > MAX_BUFF_SIZE - recvData.size()) {
        LOG_CINFO(L_CATE).noquote() << "接收缓冲区溢出，清空缓冲区";
        m_buffer.clear();
        return;
    }

    m_buffer.append(recvData);
    while (true) {
        // 在 m_buffer 中查找第一对连续的 0xFF,0xFF。找到，则0~idx-1前的数据为垃圾数据。没有找到帧头，则缓冲区数据无效，直接丢弃
        int idx = m_buffer.indexOf(STX);
        if (idx >= 0) {
            m_buffer.remove(0, idx);
        } else {
            LOG_CINFO(L_CATE).noquote() << "接收缓冲区中无有效数据，清空缓冲区";
            m_buffer.clear();
            return;
        }

        if (m_buffer.size() < FIXED_HEADER_LEN)
            return;

        // 获取版本号
        uchar ver = static_cast<uchar>(m_buffer.at(2));
        if (ver != m_ver) {
            LOG_CERROR(L_CATE).noquote() << "版本号错误: 当前版本号" << m_ver << "接收版本号" << ver;
            m_buffer.remove(0, 1);
            continue;
        }

        // 验证序列号是否正确
        uchar seq = static_cast<uchar>(m_buffer.at(3));
        bool ok = ((seq & 0xF0) == 0x00) && ((seq & 0x0F) >= 1 && (seq & 0x0F) <= 9);
        if (!ok) {
            LOG_CERROR(L_CATE).noquote() << "序列号错误:" << seq;
            m_buffer.remove(0, 1);
            continue;
        }

        // 解析数据长度
        quint32 dataLen = DataDealUtils::byteToUInt(m_buffer.mid(STX_LEN + VER_LEN + SEQ_LEN, 4));

        quint32 totalSize = STX_LEN + VER_LEN + SEQ_LEN + LEN_FIELD_LEN + dataLen + CRC_LEN;
        // 数据长度不足，等待后续更多数据到来
        if ((quint32) m_buffer.size() < totalSize)
            return;

        QByteArray frame = m_buffer.left(totalSize);
        // 进行CRC校验
        QByteArray remoteCrc = frame.right(CRC_LEN);
        QByteArray localCrc = DataDealUtils::getCRCCode(frame.mid(STX_LEN, totalSize - STX_LEN - CRC_LEN));

        if (remoteCrc != localCrc) {
            QString localCrcStr = QString::fromLatin1(localCrc.toHex()).toUpper();
            QString remoteCrcStr = QString::fromLatin1(remoteCrc.toHex()).toUpper();
            LOG_CERROR(L_CATE).noquote() << QString("CRC校验失败(localCrc: %1, remoteCrc: %2)").arg(localCrcStr).arg(remoteCrcStr);
            m_buffer.remove(0, 1);
            continue;
        }

        QByteArray command = frame.mid(STX_LEN + VER_LEN + SEQ_LEN + LEN_FIELD_LEN, dataLen);
        if (command.isEmpty()) {
            LOG_CERROR(L_CATE).noquote() << "收到空指令帧";
            m_buffer.remove(0, totalSize);
            continue;
        }

        // 处理指令并响应
        dealCommand(seq, command);

        m_buffer.remove(0, totalSize);
        continue;
    }
}

void SmartLaneController::onErrorOccurred(QAbstractSocket::SocketError)
{
    LOG_CWARNING(L_CATE).noquote() << QString("网络错误: %1").arg(m_socket->errorString());
}

void SmartLaneController::enqueueCommand(uchar cmdType, const QByteArray &cmd)
{
    QString cmdTypeStr = QString("%1").arg(cmdType, 2, 16, QLatin1Char('0')).toUpper();
    if (!m_connected) {
        LOG_CERROR(L_CATE).noquote() << QString("指令[0x%1]入队失败: 与服务端网络连接失效").arg(cmdTypeStr);
        return;
    }

    ST_SendTask task;
    task.cmdType = cmdType;
    task.cmd = cmd;
    m_sendQueue.enqueue(task);

    LOG_CINFO(L_CATE).noquote() << QString("指令[0x%1]进入发送队列，当前队列长度: %2").arg(cmdTypeStr).arg(m_sendQueue.size());
    trySendNextCommand();
}

void SmartLaneController::trySendNextCommand()
{
    if (!m_connected || m_waitingCmdResponse || m_sendQueue.isEmpty())
        return;

    ST_SendTask &task = m_sendQueue.head();
    task.seq = getClientSeq();
    task.frame = makeFrame(task.seq, task.cmd);
    task.retryCount = 0;

    QString cmdTypeStr = QString("%1").arg(task.cmdType, 2, 16, QLatin1Char('0')).toUpper();
    LOG_CINFO(L_CATE).noquote() << QString("发送指令[0x%1][0x%2]").arg(cmdTypeStr).arg(task.seq, 2, 16, QLatin1Char('0'));

    if (!sendFrame(task.frame)) {
        LOG_CERROR(L_CATE).noquote() << "队首指令发送失败，移出发送队列";
        m_sendQueue.dequeue();
        trySendNextCommand();
        return;
    }

    m_waitingCmdResponse = true;
    m_cmdResponseTimer->start(REQUEST_TIMEOUT);
}

bool SmartLaneController::handleCommandResponse(uchar seq, uchar cmdType)
{
    QString cmdTypeStr = QString("%1").arg(cmdType, 2, 16, QLatin1Char('0')).toUpper();
    if (!m_waitingCmdResponse || m_sendQueue.isEmpty()) {
        LOG_CWARNING(L_CATE).noquote() << QString("收到无对应请求的应答: [0x%1][0x%2]").arg(cmdTypeStr).arg(seq, 2, 16, QLatin1Char('0'));
        return false;
    }

    const ST_SendTask &task = m_sendQueue.head();
    uchar responseSeq = static_cast<uchar>(seq << 4);
    QString taskCmdTypeStr = QString("%1").arg(task.cmdType, 2, 16, QLatin1Char('0'));

    if (task.cmdType != cmdType || task.seq != responseSeq) {
        LOG_CWARNING(L_CATE).noquote() << QString("应答与队首指令不匹配: 期望[0x%1][0x%2]; 实际[0x%3][0x%4]")
                                              .arg(taskCmdTypeStr)
                                              .arg(static_cast<uchar>(task.seq >> 4), 2, 16, QLatin1Char('0'))
                                              .arg(cmdTypeStr)
                                              .arg(seq, 2, 16, QLatin1Char('0'));
        return false;
    }

    LOG_CINFO(L_CATE).noquote() << QString("收到队首指令应答: [0x%1][0x%2]").arg(cmdTypeStr).arg(seq, 2, 16, QLatin1Char('0'));

    m_cmdResponseTimer->stop();
    m_sendQueue.dequeue();
    m_waitingCmdResponse = false;
    return true;
}

void SmartLaneController::handleCommandResponseTimeout()
{
    if (!m_waitingCmdResponse || m_sendQueue.isEmpty())
        return;

    ST_SendTask &task = m_sendQueue.head();
    QString cmdTypeStr = QString("%1").arg(task.cmdType, 2, 16, QLatin1Char('0')).toUpper();
    if (task.retryCount >= MAX_RETRY_TIMES) {
        LOG_CERROR(L_CATE).noquote() << QString("指令[0x%1][0x%2]重传%3次仍未收到应答，放弃当前指令")
                                            .arg(cmdTypeStr)
                                            .arg(task.seq, 2, 16, QLatin1Char('0'))
                                            .arg(MAX_RETRY_TIMES);

        m_sendQueue.dequeue();
        m_waitingCmdResponse = false;
        trySendNextCommand();
        return;
    }

    ++task.retryCount;
    LOG_CWARNING(L_CATE).noquote() << QString("指令[0x%1][0x%2]等待应答超时，进行第%3次重传")
                                          .arg(cmdTypeStr)
                                          .arg(task.seq, 2, 16, QLatin1Char('0'))
                                          .arg(task.retryCount);

    sendFrame(task.frame);
    m_cmdResponseTimer->start(REQUEST_TIMEOUT);
}

uchar SmartLaneController::getClientSeq()
{
    uchar ret = static_cast<uchar>(m_nextSeq << 4);

    ++m_nextSeq;
    if (m_nextSeq > 9)
        m_nextSeq = 1;

    return ret;
}

void SmartLaneController::dealCommand(uchar seq, const QByteArray &cmd)
{
    uchar cmdType = m_handler->getCmdType(cmd);
    if (cmdType == 0xD2) {
        QByteArray resp = m_handler->handleD2Cmd(seq, cmd);
        QByteArray frame = makeFrame(static_cast<uchar>(seq << 4), resp);
        sendFrame(frame);

        emit sigRecvD2Cmd(cmd);
    } else if (cmdType == 0xD3) {
        QByteArray resp = m_handler->handleD3Cmd(seq, cmd);
        QByteArray frame = makeFrame(static_cast<uchar>(seq << 4), resp);
        sendFrame(frame);

        emit sigRecvD3Cmd(cmd);
    } else if (cmdType == 0xD6) {
        QByteArray resp = m_handler->handleD6Cmd(seq, cmd);
        QByteArray frame = makeFrame(static_cast<uchar>(seq << 4), resp);
        sendFrame(frame);

        resetHeartbeatWatchdog();
        emit sigRecvD6Cmd(cmd);
    } else if (cmdType == 0xA1) {
        if (handleCommandResponse(seq, cmdType)) {
            m_handler->handleA1Cmd(seq, cmd);
            trySendNextCommand();
        }
    } else if (cmdType == 0xA2) {
        if (handleCommandResponse(seq, cmdType)) {
            m_handler->handleA2Cmd(seq, cmd);
            trySendNextCommand();
        }
    } else if (cmdType == 0xA3) {
        if (handleCommandResponse(seq, cmdType)) {
            m_handler->handleA3Cmd(seq, cmd);
            trySendNextCommand();
        }
    } else {
        LOG_CERROR(L_CATE).noquote() << "未知指令类型:" << QString("%1").arg(cmdType, 2, 16, QLatin1Char('0'));
    }
}

QByteArray SmartLaneController::makeFrame(uchar seq, const QByteArray &cmd)
{
    QByteArray frame;
    frame.append(STX);
    frame.append(m_ver);
    frame.append(seq);
    frame.append(DataDealUtils::intToByte(cmd.size()));
    frame.append(cmd);
    QByteArray crc = DataDealUtils::getCRCCode(frame.mid(STX_LEN));
    frame.append(crc);

    return frame;
}

bool SmartLaneController::sendFrame(const QByteArray &data)
{
    LOG_CINFO(L_CATE).noquote() << QString("【TX】[%1:%2]").arg(m_peerAddr).arg(m_peerPort) << DataDealUtils::byteArrayToHexStr(data);

    if (!m_connected) {
        LOG_CERROR(L_CATE).noquote() << "发送失败: 与服务端网络连接失效!";
        return false;
    }

    qint64 ret = m_socket->write(data);
    if (ret == -1) {
        LOG_CWARNING(L_CATE).noquote() << "发送失败: 数据写入失败";
        return false;
    }

    m_socket->flush();
    LOG_CINFO(L_CATE).noquote() << "发送成功: 数据已进入网络发送队列";
    return true;
}

void SmartLaneController::resetHeartbeatWatchdog()
{
    if (!m_connected)
        return;

    m_reconnectCount = 0;
    m_reconnectFailureNotified = false;
    m_heartbeatTimer->start(HEARTBEAT_TIMEOUT);
}

void SmartLaneController::handleHeartbeatTimeout()
{
    if (!m_connected || m_isForceDisconnect)
        return;

    LOG_CERROR(L_CATE).noquote() << "连续" << HEARTBEAT_TIMEOUT / 1000 << "秒未收到有效D6心跳，连接异常";
    m_socket->abort();
}

void SmartLaneController::scheduleReconnect(int delayMs)
{
    if (m_isForceDisconnect || m_reconnectTimer->isActive())
        return;

    if (m_reconnectCount >= MAX_RECONNECT_TIMES) {
        LOG_CERROR(L_CATE).noquote() << "自动重连已达到最大次数:" << MAX_RECONNECT_TIMES;
        if (!m_reconnectFailureNotified) {
            m_reconnectFailureNotified = true;
            emit sigReconnectFailed();
        }
        return;
    }

    LOG_CWARNING(L_CATE).noquote() << "将在" << delayMs / 1000 << "秒后尝试自动重连";
    m_reconnectTimer->start(delayMs);
}

void SmartLaneController::attemptReconnect()
{
    if (m_isForceDisconnect || m_connected || m_socket->state() != QAbstractSocket::UnconnectedState)
        return;

    if (m_reconnectCount >= MAX_RECONNECT_TIMES)
        return;

    ++m_reconnectCount;
    LOG_CWARNING(L_CATE).noquote() << "开始第" << m_reconnectCount << "次自动重连，最大次数:" << MAX_RECONNECT_TIMES;
    m_socket->connectToHost(m_peerAddr, m_peerPort);
}

// --------------------------------------------------------
ISmartLaneController *createSmartLaneController()
{
    return new SmartLaneController();
}

void destroySmartLaneController(ISmartLaneController *controller)
{
    delete controller;
}
