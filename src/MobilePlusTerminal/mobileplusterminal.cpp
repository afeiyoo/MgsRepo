#include "mobileplusterminal.h"

#include <QTimer>

#include "Logger.h"
#include "cmdhandler.h"
#include "utils/datadealutils.h"
#include "utils/stdafx.h"

using namespace Utils;

MobilePlusTerminal::MobilePlusTerminal(const QString &stationID, uint laneID, uint devSeq, QObject *parent)
    : IMobilePlusTerminal{parent}
{
    m_stationID = stationID;
    m_laneID = laneID;
    m_devSeq = devSeq;

    m_socket = new QTcpSocket(this);

    m_heartbeatTimer = new QTimer(this);
    m_heartbeatTimer->setSingleShot(true);

    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setSingleShot(true);

    m_handler = new CmdHandlerV1(m_devSeq); // 默认版本号V1

    connect(m_socket, &QTcpSocket::stateChanged, this, &MobilePlusTerminal::onStageChanged);
    connect(m_socket, &QTcpSocket::readyRead, this, &MobilePlusTerminal::onReadyRead);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &MobilePlusTerminal::handleHeartbeatTimeout);
    connect(m_reconnectTimer, &QTimer::timeout, this, &MobilePlusTerminal::attemptReconnect);
}

MobilePlusTerminal::~MobilePlusTerminal()
{
    SAFE_DELETE(m_handler);
}

void MobilePlusTerminal::connectServer(const QString &ip, quint16 port)
{
    m_isForceDisconnect = false;
    m_initialized = false;
    m_reconnectCount = 0;
    m_reconnectFailureNotified = false;
    m_reconnectTimer->stop();

    m_peerAddr = ip;
    m_peerPort = port;

    LOG_CINFO(L_CATE).noquote() << deviceLogTag() << QString("开始连接手机+自助交易终端(IP: %1, Port: %2)").arg(ip).arg(port);
    m_socket->connectToHost(ip, port);
}

void MobilePlusTerminal::disconnectServer()
{
    m_isForceDisconnect = true;
    m_heartbeatTimer->stop();
    m_reconnectTimer->stop();
    m_socket->disconnectFromHost();
}

void MobilePlusTerminal::initialize(const QString &stationID, uint laneID, uint seq)
{
    QVariantMap aMap;
    QString deviceID = QString("3501%1%2%3").arg(stationID).arg(laneID, 2, 10, QLatin1Char('0')).arg(seq, 2, 10, QLatin1Char('0'));
    aMap["deviceid"] = deviceID;
    aMap["devicename"] = "车道展码设备";
    aMap["datetime"] = DataDealUtils::curDateTimeStr("yyyyMMddhhmmss");

    QByteArray jsonData = DataDealUtils::mapToJson(aMap);

    LOG_CINFO(L_CATE).noquote() << deviceLogTag() << "发送指令 A1 Type:0";
    // 向服务端发送初始化指令
    sendA1Command(0, jsonData, false);
}

void MobilePlusTerminal::showQRCode(const QString &stationName, const QString &vehClass, const QString &vehPlate, const QString &barCode)
{
    QVariantMap aMap;
    aMap["stationname"] = stationName;
    aMap["vehclass"] = vehClass;
    aMap["vehplate"] = vehPlate;
    aMap["barcode"] = barCode;

    QByteArray jsonData = DataDealUtils::mapToJson(aMap);

    LOG_CINFO(L_CATE).noquote() << deviceLogTag() << "发送指令 A1 Type:1";
    sendA1Command(1, jsonData);
}

void MobilePlusTerminal::showLED(const QString &text)
{
    QVariantMap aMap;
    aMap["text"] = text;

    QByteArray jsonData = DataDealUtils::mapToJson(aMap);

    LOG_CINFO(L_CATE).noquote() << deviceLogTag() << "发送指令 A1 Type:2";
    sendA1Command(2, jsonData);
}

void MobilePlusTerminal::showPics(const QByteArray &data)
{
    QVariantMap aMap;
    aMap["pic"] = QString::fromLatin1(data.toBase64());

    QByteArray jsonData = DataDealUtils::mapToJson(aMap);

    LOG_CINFO(L_CATE).noquote() << deviceLogTag() << "发送指令 A1 Type:3";
    sendA1Command(3, jsonData);
}

void MobilePlusTerminal::setUploadUrl(const QString &url, int time)
{
    QVariantMap aMap;
    aMap["url"] = url;
    aMap["time"] = time;

    QByteArray jsonData = DataDealUtils::mapToJson(aMap);

    LOG_CINFO(L_CATE).noquote() << deviceLogTag() << "发送指令 A1 Type:4";
    sendA1Command(4, jsonData);
}

void MobilePlusTerminal::resetDisplay()
{
    QByteArray jsonData = "";

    LOG_CINFO(L_CATE).noquote() << deviceLogTag() << "发送指令 A1 Type:5";
    sendA1Command(5, jsonData);
}

void MobilePlusTerminal::setVersion(uchar ver)
{
    if (ver == 0x01) {
        delete m_handler;
        m_handler = new CmdHandlerV1(m_devSeq);
    } else {
        LOG_CERROR(L_CATE).noquote() << deviceLogTag() << "不支持的协议版本:" << ver;
        return;
    }
    m_ver = ver;
}

void MobilePlusTerminal::onStageChanged(QAbstractSocket::SocketState state)
{
    switch (state) {
    case QAbstractSocket::ConnectedState: {
        LOG_CINFO(L_CATE).noquote() << deviceLogTag() << "与手机+自助交易终端建立连接";
        m_connected = true;
        emit sigConnectionStateChanged(m_devSeq, true);
        m_heartbeatTimer->start(HEARTBEAT_TIMEOUT);

        initialize(m_stationID, m_laneID, m_devSeq);
    } break;
    case QAbstractSocket::UnconnectedState: {
        LOG_CERROR(L_CATE).noquote() << deviceLogTag() << "与手机+自助交易终端断开连接";
        m_connected = false;
        m_initialized = false;
        emit sigConnectionStateChanged(m_devSeq, false);
        m_heartbeatTimer->stop();
        m_pendingRequests.clear();
        m_buffer.clear(); // 清空数据缓冲区

        // 非主动断开，则延迟进行重连
        if (!m_isForceDisconnect)
            scheduleReconnect(RECONNECT_INTERVAL);
    } break;
    default:
        break;
    }
}

void MobilePlusTerminal::onReadyRead()
{
    QByteArray recvData = m_socket->readAll();
    LOG_CINFO(L_CATE).noquote() << deviceLogTag() << QString("【RX】[%1:%2]").arg(m_peerAddr).arg(m_peerPort)
                                << DataDealUtils::byteArrayToHexStr(recvData);

    LOG_CINFO(L_CATE).noquote() << deviceLogTag() << "数据缓冲区现有数据长度(Byte):" << m_buffer.size()
                                << "数据缓冲区最大数据长度(Byte):" << MAX_BUFF_SIZE << "接收到的数据长度(Byte):" << recvData.size();
    if (recvData.size() > MAX_BUFF_SIZE || m_buffer.size() > MAX_BUFF_SIZE - recvData.size()) {
        LOG_CINFO(L_CATE).noquote() << deviceLogTag() << "接收缓冲区溢出，清空缓冲区";
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
            LOG_CINFO(L_CATE).noquote() << deviceLogTag() << "接收缓冲区中无有效数据，清空缓冲区";
            m_buffer.clear();
            return;
        }

        if (m_buffer.size() < FIXED_HEADER_LEN)
            return;

        // 获取版本号
        uchar ver = static_cast<uchar>(m_buffer.at(2));
        if (ver != m_ver) {
            LOG_CERROR(L_CATE).noquote() << deviceLogTag() << "版本号错误: 当前版本号" << m_ver << "接收版本号" << ver;
            m_buffer.remove(0, 1);
            continue;
        }

        // 验证序列号是否正确
        uchar seq = static_cast<uchar>(m_buffer.at(3));
        bool ok = ((seq & 0xF0) == 0x80) && ((seq & 0x0F) >= 1 && (seq & 0x0F) <= 9);
        if (!ok) {
            LOG_CERROR(L_CATE).noquote() << deviceLogTag() << "序列号错误:" << seq;
            m_buffer.remove(0, 1);
            continue;
        }

        // 解析数据长度
        quint32 dataLen = DataDealUtils::byteToUInt(m_buffer.mid(STX_LEN + VER_LEN + SEQ_LEN, 4));

        quint32 totalSize = STX_LEN + VER_LEN + SEQ_LEN + LEN_FIELD_LEN + dataLen + CRC_LEN;
        // 数据长度不足，等待更多后续数据到来
        if ((quint32) m_buffer.size() < totalSize)
            return;

        QByteArray frame = m_buffer.left(totalSize);
        // 进行CRC校验
        QByteArray remoteCrc = frame.right(CRC_LEN);
        QByteArray localCrc = DataDealUtils::getCRCCode(frame.mid(STX_LEN, totalSize - STX_LEN - CRC_LEN));

        if (remoteCrc != localCrc) {
            QString localCrcStr = QString::fromLatin1(localCrc.toHex()).toUpper();
            QString remoteCrcStr = QString::fromLatin1(remoteCrc.toHex()).toUpper();
            LOG_CERROR(L_CATE).noquote() << deviceLogTag() << QString("CRC校验失败(localCrc: %1, remoteCrc: %2)").arg(localCrcStr).arg(remoteCrcStr);
            m_buffer.remove(0, 1);
            continue;
        }

        QByteArray command = frame.mid(STX_LEN + VER_LEN + SEQ_LEN + LEN_FIELD_LEN, dataLen);
        if (command.isEmpty()) {
            LOG_CERROR(L_CATE).noquote() << deviceLogTag() << "收到空指令帧";
            m_buffer.remove(0, totalSize);
            continue;
        }

        // 处理指令并响应
        dealCommand(seq, command);

        m_buffer.remove(0, totalSize);
        continue;
    }
}

bool MobilePlusTerminal::sendA1Command(uchar type, const QByteArray &jsonData, bool requiresInitialized)
{
    if (!m_connected) {
        LOG_CERROR(L_CATE).noquote() << deviceLogTag() << "发送A1失败: 与服务端网络连接失效!";
        return false;
    }

    if (requiresInitialized && !m_initialized) {
        LOG_CERROR(L_CATE).noquote() << deviceLogTag() << "发送A1失败: 设备尚未完成初始化, Type:" << type;
        return false;
    }

    uchar seq = getClientSeq();
    QByteArray cmd = m_handler->assembleA1Cmd(type, jsonData);
    QByteArray frame = makeFrame(seq, cmd);
    QByteArray requestKey = m_handler->makeRequestKey(seq, cmd);
    if (m_pendingRequests.contains(requestKey)) {
        LOG_CERROR(L_CATE).noquote() << deviceLogTag() << "发送A1失败: 序列号、DateTime和Type存在重复的未响应请求";
        return false;
    }

    ST_PendingRequest request;
    request.seq = seq;
    request.type = type;
    request.frame = frame;
    m_pendingRequests[requestKey] = request;

    if (!sendFrame(frame)) {
        m_pendingRequests.remove(requestKey);
        return false;
    }

    QTimer::singleShot(REQUEST_TIMEOUT, this, [this, requestKey]() { handleRequestTimeout(requestKey); });

    return true;
}

bool MobilePlusTerminal::sendFrame(const QByteArray &data)
{
    LOG_CINFO(L_CATE).noquote() << deviceLogTag() << QString("【TX】[%1:%2]").arg(m_peerAddr).arg(m_peerPort)
                                << DataDealUtils::byteArrayToHexStr(data);

    if (!m_connected) {
        LOG_CERROR(L_CATE).noquote() << deviceLogTag() << "发送失败: 与服务端网络连接失效!";
        return false;
    }

    qint64 ret = m_socket->write(data);
    if (ret == -1) {
        LOG_CWARNING(L_CATE).noquote() << deviceLogTag() << "发送失败: 数据写入失败";
        return false;
    }

    m_socket->flush();
    LOG_CINFO(L_CATE).noquote() << deviceLogTag() << "发送成功: 数据已进入网络发送队列";
    return true;
}

QByteArray MobilePlusTerminal::makeFrame(uchar seq, const QByteArray &cmd)
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

uchar MobilePlusTerminal::getClientSeq()
{
    uchar ret = static_cast<uchar>(0x10 | m_nextSeq);

    ++m_nextSeq;
    if (m_nextSeq > 9)
        m_nextSeq = 1; // 回到 1

    return ret;
}

void MobilePlusTerminal::dealCommand(uchar seq, const QByteArray &cmd)
{
    uchar cmdType = m_handler->getCmdType(cmd);
    if (cmdType == 0xB1) {
        // 解析B1帧，并返回F1帧
        ST_B1HandleResult result = m_handler->handleB1Cmd(cmd);
        sendFrame(makeFrame(static_cast<uchar>((seq & 0x0F) | 0x10), result.response));

        // 其他操作
        uchar type = m_handler->getB1Type(cmd);
        if (type == 1) { // 心跳
            resetHeartbeatWatchdog();
        } else if (type == 2 && result.requestHelp) { // 求助
            emit sigRequestHelp(m_devSeq, result.helpType);
        }
    } else if (cmdType == 0xF1) {
        handleF1Response(seq, cmd);
    } else {
        LOG_CERROR(L_CATE).noquote() << deviceLogTag() << "未知指令类型:" << QString("%1").arg(cmdType, 2, 16, QLatin1Char('0'));
    }
}

void MobilePlusTerminal::handleF1Response(uchar seq, const QByteArray &cmd)
{
    uchar requestSeq = static_cast<uchar>((seq & 0x0F) | 0x10);
    QByteArray requestKey = m_handler->makeRequestKey(requestSeq, cmd);
    auto it = m_pendingRequests.find(requestKey);
    if (it == m_pendingRequests.end()) {
        LOG_CWARNING(L_CATE).noquote() << deviceLogTag() << "收到无对应请求的F1应答帧, Seq:" << QString("0x%1").arg(seq, 2, 16, QLatin1Char('0'));
        return;
    }

    bool success = m_handler->handleF1Cmd(cmd);
    uchar requestType = it->type;
    m_pendingRequests.erase(it);

    if (requestType == 0) {
        m_initialized = success;
        emit sigInitStateChanged(m_devSeq, success);
    }
    emit sigCmdFinished(m_devSeq, requestType, success);
}

void MobilePlusTerminal::handleRequestTimeout(const QByteArray &requestKey)
{
    auto it = m_pendingRequests.find(requestKey);
    if (it == m_pendingRequests.end())
        return; // 已收到匹配的F1应答，或连接已断开

    if (it->retryCount >= MAX_RETRY_TIMES) {
        LOG_CERROR(L_CATE).noquote() << deviceLogTag() << "A1重传" << MAX_RETRY_TIMES
                                     << "次后仍未收到F1应答, Seq:" << QString("0x%1").arg(it->seq, 2, 16, QLatin1Char('0')) << "Type:" << it->type;
        const uchar requestType = it->type;
        m_pendingRequests.erase(it);
        if (requestType == 0)
            emit sigInitStateChanged(m_devSeq, false);
        emit sigCmdFinished(m_devSeq, requestType, false);
        return;
    }

    ++it->retryCount;
    LOG_CWARNING(L_CATE).noquote() << deviceLogTag() << "A1等待F1应答超时，执行第" << it->retryCount
                                   << "次重传, Seq:" << QString("0x%1").arg(it->seq, 2, 16, QLatin1Char('0')) << "Type:" << it->type;
    sendFrame(it->frame);

    QTimer::singleShot(RETRY_INTERVAL, this, [this, requestKey]() { handleRequestTimeout(requestKey); });
}

void MobilePlusTerminal::resetHeartbeatWatchdog()
{
    if (m_connected) {
        m_reconnectCount = 0;
        m_reconnectFailureNotified = false;
        m_heartbeatTimer->start(HEARTBEAT_TIMEOUT);
    }
}

void MobilePlusTerminal::handleHeartbeatTimeout()
{
    if (!m_connected || m_isForceDisconnect)
        return;

    LOG_CERROR(L_CATE).noquote() << deviceLogTag() << "连续" << HEARTBEAT_TIMEOUT / 1000 << "秒未收到有效心跳，连接异常";
    m_initialized = false;
    m_socket->abort();
}

void MobilePlusTerminal::scheduleReconnect(int delayMs)
{
    if (m_isForceDisconnect || m_reconnectTimer->isActive())
        return;

    if (m_reconnectCount >= MAX_RECONNECT_TIMES) {
        LOG_CERROR(L_CATE).noquote() << deviceLogTag() << "自动重连已达到最大次数:" << MAX_RECONNECT_TIMES;
        if (!m_reconnectFailureNotified) {
            m_reconnectFailureNotified = true;
            emit sigReconnectFailed(m_devSeq);
        }
        return;
    }

    LOG_CWARNING(L_CATE).noquote() << deviceLogTag() << "将在" << delayMs / 1000 << "秒后尝试自动重连";
    m_reconnectTimer->start(delayMs);
}

void MobilePlusTerminal::attemptReconnect()
{
    if (m_isForceDisconnect || m_connected || m_socket->state() != QAbstractSocket::UnconnectedState)
        return;

    if (m_reconnectCount >= MAX_RECONNECT_TIMES)
        return;

    ++m_reconnectCount;
    LOG_CWARNING(L_CATE).noquote() << deviceLogTag() << "开始第" << m_reconnectCount << "次自动重连，最大次数:" << MAX_RECONNECT_TIMES;
    m_socket->connectToHost(m_peerAddr, m_peerPort);
}

QString MobilePlusTerminal::deviceLogTag() const
{
    return QString("[DevSeq:%1]").arg(m_devSeq, 2, 10, QLatin1Char('0'));
}

// --------------------------------------------------------
IMobilePlusTerminal *createMobilePlusTerminal(const QString &stationID, uint laneID, uint devSeq)
{
    if (laneID > 99 || devSeq > 99)
        return nullptr;

    return new MobilePlusTerminal(stationID, laneID, devSeq);
}

void destroyMobilePlusTerminal(IMobilePlusTerminal *terminal)
{
    delete terminal;
}
