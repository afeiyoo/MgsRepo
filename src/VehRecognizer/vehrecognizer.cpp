#include "vehrecognizer.h"

#include <QDir>
#include <QFile>

#include "Logger.h"
#include "cmdhandler.h"
#include "defines.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"
#include "utils/stdafx.h"

using namespace Utils;

VehRecognizer::VehRecognizer(const QString &stationID, const QString &stationName, uint laneID, QObject *parent)
    : IVehRecognizer(parent)
{
    qRegisterMetaType<ST_VehicleInfo>("ST_VehicleInfo"); // 自定义信号参数类型注册

    m_stationName = stationName;
    m_stationID = stationID;
    m_laneID = laneID;

    m_socket = new QTcpSocket(this);

    m_heartbeatTimer = new QTimer(this);
    m_heartbeatTimer->setSingleShot(true);

    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setSingleShot(true);

    m_vehicleCleanupTimer = new QTimer(this);
    m_vehicleCleanupTimer->setInterval(60 * 1000); // 每分钟检查一次过期车辆信息

    m_handler = new CmdHandlerV1(); // 默认版本号V1

    connect(m_socket, &QTcpSocket::stateChanged, this, &VehRecognizer::onStateChanged);
    connect(m_socket, &QTcpSocket::readyRead, this, &VehRecognizer::onReadyRead);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &VehRecognizer::handleHeartbeatTimeout);
    connect(m_reconnectTimer, &QTimer::timeout, this, &VehRecognizer::attemptReconnect);
    connect(m_vehicleCleanupTimer, &QTimer::timeout, this, &VehRecognizer::cleanupExpiredVehicles);

    m_vehicleCleanupTimer->start();
}

VehRecognizer::~VehRecognizer()
{
    SAFE_DELETE(m_handler);
}

void VehRecognizer::connectServer(const QString &ip, quint16 port)
{
    m_isForceDisconnect = false;
    m_initialized = false;
    m_reconnectCount = 0;
    m_reconnectFailureNotified = false;
    m_reconnectTimer->stop();

    m_peerAddr = ip;
    m_peerPort = port;

    LOG_CINFO(L_CATE).noquote() << QString("开始连接车型识别器(IP: %1, Port: %2)").arg(ip).arg(port);
    m_socket->connectToHost(ip, port);
}

void VehRecognizer::disconnectServer()
{
    m_isForceDisconnect = true;
    m_heartbeatTimer->stop();
    m_reconnectTimer->stop();
    m_socket->disconnectFromHost();
}

void VehRecognizer::setVersion(uchar ver)
{
    if (ver == 0x01) {
        delete m_handler;
        m_handler = new CmdHandlerV1();
    } else {
        LOG_CERROR(L_CATE).noquote() << "不支持的协议版本:" << ver;
        return;
    }

    m_ver = ver;
}

void VehRecognizer::setUploadUrl(const QString &url, uchar minutes)
{
    LOG_CINFO(L_CATE).noquote() << "发送指令 AB";

    QByteArray cmd = m_handler->assembleABCmd(url, minutes);
    sendCommand(cmd);
}

void VehRecognizer::showLED(uchar color, const QString &data)
{
    LOG_CINFO(L_CATE).noquote() << "发送指令 AC";

    QByteArray cmd = m_handler->assembleACCmd(color, data);
    sendCommand(cmd);
}

void VehRecognizer::playVoice(uchar count, const QString &text, int intervalMs)
{
    LOG_CINFO(L_CATE).noquote() << "发送指令 AD";

    QByteArray cmd = m_handler->assembleADCmd(count, text, intervalMs);
    sendCommand(cmd);
}

void VehRecognizer::onStateChanged(QAbstractSocket::SocketState state)
{
    switch (state) {
    case QAbstractSocket::ConnectedState: {
        LOG_CINFO(L_CATE).noquote() << "与车型识别器建立连接";
        m_connected = true;
        emit sigConnectionStateChanged(true);
        m_heartbeatTimer->start(HEARTBEAT_TIMEOUT);

        initialize();
    } break;
    case QAbstractSocket::UnconnectedState: {
        LOG_CERROR(L_CATE).noquote() << "与车型识别器断开连接";
        m_connected = false;
        m_initialized = false;
        emit sigConnectionStateChanged(false);
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

void VehRecognizer::onReadyRead()
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
    while (!m_buffer.isEmpty()) {
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
        bool ok = ((seq & 0xF0) == 0x80) && ((seq & 0x0F) >= 1 && (seq & 0x0F) <= 9);
        if (!ok) {
            LOG_CERROR(L_CATE).noquote() << "序列号错误:" << seq;
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

void VehRecognizer::initialize()
{
    LOG_CINFO(L_CATE).noquote() << "发送指令 AA";

    QByteArray cmd = m_handler->assembleAACmd(m_stationID, m_stationName, static_cast<int>(m_laneID));
    sendCommand(cmd, false);
}

void VehRecognizer::scheduleReconnect(int delayMs)
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

void VehRecognizer::attemptReconnect()
{
    if (m_isForceDisconnect || m_connected || m_socket->state() != QAbstractSocket::UnconnectedState)
        return;

    if (m_reconnectCount >= MAX_RECONNECT_TIMES)
        return;

    ++m_reconnectCount;
    LOG_CWARNING(L_CATE).noquote() << "开始第" << m_reconnectCount << "次自动重连，最大次数:" << MAX_RECONNECT_TIMES;
    m_socket->connectToHost(m_peerAddr, m_peerPort);
}

QByteArray VehRecognizer::makeFrame(uchar seq, const QByteArray &cmd)
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

bool VehRecognizer::sendCommand(const QByteArray &cmd, bool requiresInitialized)
{
    if (!m_connected) {
        LOG_CERROR(L_CATE).noquote() << "发送失败: 与车型识别器的网络连接失效!";
        return false;
    }

    if (requiresInitialized && !m_initialized) {
        LOG_CERROR(L_CATE).noquote() << "发送失败: 车型识别器尚未完成初始化";
        return false;
    }

    const uchar type = m_handler->getCmdType(cmd);
    const uchar seq = getClientSeq();
    const QByteArray frame = makeFrame(seq, cmd);
    const QByteArray requestKey = makeRequestKey(seq, type);

    QString cmdTypeStr = QString("%1").arg(type, 2, 16, QLatin1Char('0')).toUpper();
    if (m_pendingRequests.contains(requestKey)) {
        LOG_CERROR(L_CATE).noquote() << "发送失败: 同一指令类型存在相同序列号的未响应请求, Seq:" << QString("0x%1").arg(seq, 2, 16, QLatin1Char('0'))
                                     << "CmdType:" << QString("0x%1").arg(cmdTypeStr);
        return false;
    }

    ST_PendingRequest request;
    request.seq = seq;
    request.type = type;
    request.frame = frame;
    m_pendingRequests.insert(requestKey, request);

    if (!sendFrame(frame)) {
        m_pendingRequests.remove(requestKey);
        return false;
    }

    QTimer::singleShot(REQUEST_TIMEOUT, this, [this, requestKey]() { handleRequestTimeout(requestKey); });
    return true;
}

bool VehRecognizer::sendFrame(const QByteArray &frame)
{
    LOG_CINFO(L_CATE).noquote() << QString("【TX】[%1:%2]").arg(m_peerAddr).arg(m_peerPort) << DataDealUtils::byteArrayToHexStr(frame);

    if (!m_connected || m_socket->state() != QAbstractSocket::ConnectedState) {
        LOG_CERROR(L_CATE).noquote() << "发送失败: 与车型识别器的网络连接失效!";
        return false;
    }

    const qint64 ret = m_socket->write(frame);
    if (ret == -1) {
        LOG_CWARNING(L_CATE).noquote() << "发送失败: 数据写入失败";
        return false;
    }

    m_socket->flush();
    LOG_CINFO(L_CATE).noquote() << "发送成功: 数据已进入网络发送队列";
    return true;
}

QByteArray VehRecognizer::makeRequestKey(uchar seq, uchar type) const
{
    QByteArray key;
    key.append(seq);
    key.append(type);
    return key;
}

void VehRecognizer::handleRequestTimeout(const QByteArray &requestKey)
{
    auto it = m_pendingRequests.find(requestKey);
    if (it == m_pendingRequests.end())
        return; // 已收到对应应答，或连接断开后请求已清理

    if (it->retryCount >= MAX_RETRY_TIMES) {
        const uchar requestType = it->type;
        const uchar requestSeq = it->seq;
        m_pendingRequests.erase(it);

        QString requestTypeStr = QString("%1").arg(requestType, 2, 16, QLatin1Char('0')).toUpper();
        LOG_CERROR(L_CATE).noquote() << "指令重传" << MAX_RETRY_TIMES
                                     << "次后仍未收到应答, Seq:" << QString("0x%1").arg(requestSeq, 2, 16, QLatin1Char('0'))
                                     << "CmdType:" << QString("0x%1").arg(requestTypeStr);

        if (requestType == 0xAA) {
            m_initialized = false;
            emit sigInitStateChanged(false);
        }
        emit sigCmdFinished(requestType, false);
        return;
    }

    ++it->retryCount;
    LOG_CWARNING(L_CATE).noquote() << "等待设备应答超时，执行第" << it->retryCount
                                   << "次重传, Seq:" << QString("0x%1").arg(it->seq, 2, 16, QLatin1Char('0'))
                                   << "CmdType:" << QString("0x%1").arg(it->type, 2, 16, QLatin1Char('0'));
    sendFrame(it->frame);

    QTimer::singleShot(RETRY_INTERVAL, this, [this, requestKey]() { handleRequestTimeout(requestKey); });
}

uchar VehRecognizer::getClientSeq()
{
    uchar ret = static_cast<uchar>(0x10 | m_nextSeq);

    ++m_nextSeq;
    if (m_nextSeq > 9)
        m_nextSeq = 1; // 回到 1

    return ret;
}

void VehRecognizer::dealCommand(uchar seq, const QByteArray &cmd)
{
    uchar cmdType = m_handler->getCmdType(cmd);
    if (cmdType == 0xEA) {
        const ST_EAHandleResult result = m_handler->handleEACmd(cmd);

        QByteArray response;
        response.append(uchar(0xEA));
        response.append(result.status ? uchar(0x01) : uchar(0x00));
        sendFrame(makeFrame(static_cast<uchar>((seq & 0x0F) | 0x10), response));
        // 处理成功
        if (result.status)
            updateVehicleInfo(result);
    } else if (cmdType == 0xEB) {
        ST_EBHandleResult result = m_handler->handleEBCmd(cmd);

        QByteArray response;
        response.append(uchar(0xEB));
        response.append(result.status ? uchar(0x01) : uchar(0x00));
        sendFrame(makeFrame(static_cast<uchar>((seq & 0x0F) | 0x10), response));
        // 处理成功
        if (result.status) {
            QString mediaPath = saveVehicleMedia(result);
            updateVehicleInfo(result, mediaPath);
        }
    } else if (cmdType == 0xEC) {
        const QByteArray response = m_handler->handleECCmd(cmd);
        sendFrame(makeFrame(static_cast<uchar>((seq & 0x0F) | 0x10), response));

        resetHeartbeatWatchdog();
    } else if (cmdType == 0xAA || cmdType == 0xAB || cmdType == 0xAC || cmdType == 0xAD) {
        handleResponse(seq, cmd);
    } else {
        LOG_CERROR(L_CATE).noquote() << "未知指令类型:" << QString("%1").arg(cmdType, 2, 16, QLatin1Char('0'));
    }
}

void VehRecognizer::handleResponse(uchar seq, const QByteArray &cmd)
{
    uchar requestSeq = static_cast<uchar>((seq & 0x0F) | 0x10);
    uchar cmdType = m_handler->getCmdType(cmd);
    QByteArray requestKey = makeRequestKey(requestSeq, cmdType);
    auto it = m_pendingRequests.find(requestKey);
    if (it == m_pendingRequests.end()) {
        LOG_CWARNING(L_CATE).noquote() << "收到无对应请求的应答帧, Seq:" << QString("0x%1").arg(seq, 2, 16, QLatin1Char('0'));
        return;
    }

    bool success = m_handler->handleResponse(cmd);
    uchar requestType = it->type;
    m_pendingRequests.erase(it);

    if (requestType == 0xAA) {
        m_initialized = success;
        emit sigInitStateChanged(success);
    }
    emit sigCmdFinished(requestType, success);
}

void VehRecognizer::resetHeartbeatWatchdog()
{
    if (m_connected) {
        m_reconnectCount = 0;
        m_reconnectFailureNotified = false;
        m_heartbeatTimer->start(HEARTBEAT_TIMEOUT);
    }
}

void VehRecognizer::handleHeartbeatTimeout()
{
    if (!m_connected || m_isForceDisconnect)
        return;

    LOG_CERROR(L_CATE).noquote() << "连续" << HEARTBEAT_TIMEOUT / 1000 << "秒未收到有效心跳，连接异常";
    m_initialized = false;
    m_socket->abort();
}

QByteArray VehRecognizer::makeVehicleKey(const QString &vehPlate, const QDateTime &vehTime) const
{
    QByteArray key = vehPlate.toUtf8();
    key.append('\0');
    key.append(vehTime.toString("yyyyMMddhhmmss").toLatin1());
    return key;
}

QString VehRecognizer::saveVehicleMedia(const ST_EBHandleResult &result) const
{
    QString mediaType;
    QString extension;
    switch (result.imgType) {
    case 1:
        mediaType = "face";
        extension = "jpg";
        break;
    case 2:
        mediaType = "tail";
        extension = "jpg";
        break;
    case 3:
        mediaType = "body";
        extension = "jpg";
        break;
    case 4:
        mediaType = "video";
        extension = "mp4";
        break;
    default:
        return {};
    }

    QDir appDir(FileUtils::curApplicationDirPath());
    if (!appDir.mkpath("VehclassImage")) {
        LOG_CERROR(L_CATE).noquote() << "创建车型识别器图像目录失败:" << appDir.absoluteFilePath("VehclassImage");
        return "";
    }

    QDir mediaDir(appDir.absoluteFilePath("VehclassImage"));
    QString fileName;
    if (result.imgType == 4) {
        fileName = QString("%1_%2.%3").arg(result.vehTime.toString("yyyyMMddhhmmss")).arg(result.vehPlate).arg(extension);
    } else {
        fileName = QString("%1_%2_%3.%4").arg(result.vehTime.toString("yyyyMMddhhmmss")).arg(result.vehPlate).arg(mediaType).arg(extension);
    }
    const QString absolutePath = mediaDir.absoluteFilePath(fileName);

    FileSaver saver(absolutePath);
    if (!saver.write(result.imgInfo)) {
        LOG_CERROR(L_CATE).noquote() << "写入文件失败:" << absolutePath << saver.errorString();
        return "";
    }
    if (!saver.finalize()) {
        LOG_CERROR(L_CATE).noquote() << "保存文件失败:" << absolutePath << saver.errorString();
        return "";
    }

    LOG_CINFO(L_CATE).noquote() << "车型识别器媒体文件保存成功:"
                                << "Path:" << absolutePath << "Size:" << result.imgInfo.size();
    return absolutePath;
}

void VehRecognizer::removeVehicleMediaFiles(const ST_VehicleImageInfo &imageInfo) const
{
    const QString paths[] = {imageInfo.headImagePath, imageInfo.tailImagePath, imageInfo.bodyImagePath, imageInfo.shortVideoPath};
    for (const QString &path : paths) {
        if (!path.isEmpty() && QFile::exists(path) && !QFile::remove(path))
            LOG_CWARNING(L_CATE).noquote() << "删除过期车型识别器媒体文件失败:" << path;
    }
}

void VehRecognizer::updateVehicleInfo(const ST_EAHandleResult &result)
{
    const QByteArray key = makeVehicleKey(result.vehPlate, result.vehTime);
    ST_VehicleCacheEntry &entry = m_vehicleQueue[key];
    entry.info.vehPlate = result.vehPlate;
    entry.info.plateColor = result.plateColor;
    entry.info.vehTime = result.vehTime;
    entry.info.typeInfo.vehClass = result.vehClass;
    entry.info.typeInfo.axleType = result.axleType;
    entry.info.typeInfo.axleCount = result.axleCount;
    entry.info.typeInfo.totalLength = result.totalLength;
    entry.info.typeInfo.totalWidth = result.totalWidth;
    entry.info.typeInfo.totalHeight = result.totalHeight;
    entry.info.typeInfo.extFlag = result.extFlag;
    entry.info.typeInfo.direction = result.direction;
    entry.hasEA = true;

    emitVehicleInfoIfComplete(key);
}

void VehRecognizer::updateVehicleInfo(const ST_EBHandleResult &result, const QString &absolutePath)
{
    const QByteArray key = makeVehicleKey(result.vehPlate, result.vehTime);
    ST_VehicleCacheEntry &entry = m_vehicleQueue[key];
    entry.info.vehPlate = result.vehPlate;
    entry.info.plateColor = result.plateColor;
    entry.info.vehTime = result.vehTime;

    switch (result.imgType) {
    case 1:
        entry.info.imageInfo.headImagePath = absolutePath;
        entry.hasHeadImage = true;
        break;
    case 2:
        entry.info.imageInfo.tailImagePath = absolutePath;
        entry.hasTailImage = true;
        break;
    case 3:
        entry.info.imageInfo.bodyImagePath = absolutePath;
        entry.hasBodyImage = true;
        break;
    case 4:
        entry.info.imageInfo.shortVideoPath = absolutePath;
        entry.hasShortVideo = true;
        break;
    default:
        return;
    }

    emitVehicleInfoIfComplete(key);
}

void VehRecognizer::emitVehicleInfoIfComplete(const QByteArray &key)
{
    auto it = m_vehicleQueue.find(key);
    if (it == m_vehicleQueue.end())
        return;

    const ST_VehicleCacheEntry &entry = it.value();
    if (!entry.hasEA || !entry.hasHeadImage || !entry.hasTailImage || !entry.hasBodyImage || !entry.hasShortVideo)
        return;

    const ST_VehicleInfo vehicleInfo = entry.info;
    m_vehicleQueue.erase(it);

    LOG_CINFO(L_CATE).noquote() << "车辆信息已完整:" << "VehPlate:" << vehicleInfo.vehPlate
                                << "VehTime:" << vehicleInfo.vehTime.toString("yyyy-MM-dd HH:mm:ss");
    emit sigVehicleInfoReady(vehicleInfo);
}

void VehRecognizer::cleanupExpiredVehicles()
{
    const QDateTime currentTime = QDateTime::currentDateTime();

    auto vehicleIt = m_vehicleQueue.begin();
    while (vehicleIt != m_vehicleQueue.end()) {
        const ST_VehicleCacheEntry &entry = vehicleIt.value();
        if (entry.info.vehTime.secsTo(currentTime) > VEHICLE_CACHE_TIMEOUT_SECS) {
            LOG_CWARNING(L_CATE).noquote() << "清理过期的未完成车辆信息:"
                                           << "VehPlate:" << entry.info.vehPlate << "VehTime:" << entry.info.vehTime.toString("yyyy-MM-dd HH:mm:ss");
            removeVehicleMediaFiles(entry.info.imageInfo);
            vehicleIt = m_vehicleQueue.erase(vehicleIt);
        } else {
            ++vehicleIt;
        }
    }
}

// -------------------------------------------------------
IVehRecognizer *createVehRecognizer(const QString &stationID, const QString &stationName, uint laneID)
{
    return new VehRecognizer(stationID, stationName, laneID);
}

void destroyVehRecognizer(IVehRecognizer *rec)
{
    delete rec;
}
