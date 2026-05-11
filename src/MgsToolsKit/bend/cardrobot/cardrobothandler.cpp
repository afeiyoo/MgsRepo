#include "cardrobothandler.h"

#include "Logger.h"
#include "NlohmannJson/nlojson.hpp"
#include "global/constant.h"
#include "global/globalmanager.h"
#include "global/signalmanager.h"
#include "utils/datadealutils.h"

#define MAX_BUFF_SIZE 5 * 1024

using namespace Utils;

CardRobotHandler::CardRobotHandler(QObject *parent)
    : QObject{parent}
{
    m_server = new QTcpServer(this);

    connect(m_server, &QTcpServer::newConnection, this, &CardRobotHandler::onNewConnection);
    connect(GM_INSTANCE->m_signalMan, &SignalManager::sigRobotStatusChanged, this, &CardRobotHandler::onRobotStatusChanged);
    connect(GM_INSTANCE->m_signalMan, &SignalManager::sigIsAutoRespCommand52, this, &CardRobotHandler::onIsAutoRespCommand52);
    connect(GM_INSTANCE->m_signalMan, &SignalManager::sigRobotSendCommand, this, &CardRobotHandler::onRobotSendCommand);
}

CardRobotHandler::~CardRobotHandler() {}

bool CardRobotHandler::startListen(quint16 port)
{
    bool ok = m_server->listen(QHostAddress::AnyIPv4, port);
    if (ok) {
        LOG_CINFO("cardrobot").noquote() << "在指定端口" << port << "启动监听成功";
        LOG_CINFO("cardrobot").noquote() << "卡机自检完成";
    } else {
        LOG_CERROR("cardrobot").noquote() << "在指定端口" << port << "启动监听失败";
    }

    return ok;
}

void CardRobotHandler::stopListen()
{
    if (m_server->isListening()) {
        LOG_CINFO("cardrobot").noquote() << "停止监听成功";
        m_server->close();
    }
}

void CardRobotHandler::setDefInfo(uchar defWorkStation, uchar defUpBoxNo, uchar defDownBoxNo)
{
    m_defWorkStation = defWorkStation;
    m_defDownBoxNo = defDownBoxNo;
    m_defUpBoxNo = defUpBoxNo;
}

void CardRobotHandler::onIsAutoRespCommand52(bool res)
{
    m_isAutoRespCommand52 = res;
}

void CardRobotHandler::onNewConnection()
{
    QTcpSocket *socket = m_server->nextPendingConnection();

    QString clientIP = socket->peerAddress().toString();
    quint16 clientPort = socket->peerPort();

    if (m_client && m_connected) {
        LOG_CINFO("cardrobot").noquote() << QString("当前已有连接建立，拒绝新客户端(%1:%2)建立连接").arg(clientIP).arg(clientPort);

        socket->disconnectFromHost();
        socket->deleteLater();
        return;
    }

    connect(socket, &QTcpSocket::disconnected, this, &CardRobotHandler::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &CardRobotHandler::onReadyRead);

    m_connected = true; // 建立连接
    m_client = socket;
    LOG_CINFO("cardrobot").noquote() << QString("客户端(%1:%2)已上线").arg(socket->peerAddress().toString()).arg(socket->peerPort());
}

void CardRobotHandler::onDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    if (!client)
        return;

    QString clientIP = client->peerAddress().toString();
    quint16 clientPort = client->peerPort();

    LOG_CINFO("cardrobot").noquote() << QString("客户端(%1:%2)已下线").arg(clientIP).arg(clientPort);
    m_connected = false; // 连接断开

    client->deleteLater();
    m_client = nullptr;

    emit GM_INSTANCE->m_signalMan->sigRobotScreenInit();
}

void CardRobotHandler::onReadyRead()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    if (!client)
        return;

    QByteArray recvData = client->readAll();
    LOG_CDEBUG("cardrobot").noquote() << "Get <==※" << DataDealUtils::byteArrayToHexStr(recvData);

    if (m_recvBuffer.size() + recvData.size() >= MAX_BUFF_SIZE) {
        LOG_CWARNING("cardrobot").noquote() << QString("数据缓冲区剩余大小不足，无法存储接收的数据(%1B)，忽略处理").arg(recvData.size());
        return;
    }

    m_recvBuffer.append(recvData);

    while (1) {
        // 查找帧头。找到，则0~idx-1前的数据为垃圾数据。没有找到帧头，则缓冲区数据无效，直接丢弃
        int idx = m_recvBuffer.indexOf(Constant::CardRobot::START);
        if (idx >= 0) {
            m_recvBuffer.remove(0, idx);
        } else {
            m_recvBuffer.clear(); // 无帧头，则为垃圾数据
            return;
        }

        // 验证版本号是否为 0x01
        uchar ver = static_cast<uchar>(m_recvBuffer.at(2));
        if (ver != Constant::CardRobot::VER) {
            LOG_CERROR("cardrobot").noquote() << "版本号错误";
            m_recvBuffer.remove(0, Constant::CardRobot::START_SIZE);
            continue;
        }

        // 验证序列号是否正确
        uchar seq = static_cast<uchar>(m_recvBuffer.at(3));
        uchar high = (seq & 0xF0) >> 4; // 高4位
        uchar low = (seq & 0x0F);       // 低4位
        if (!(high == 1 && low >= 1 && low <= 9) && !(low == 8 && high >= 1 && high <= 9)) {
            LOG_CERROR("cardrobot").noquote() << "序列号错误";
            m_recvBuffer.remove(0, Constant::CardRobot::START_SIZE);
            continue;
        }

        // 解析数据长度
        quint32 dataLen = DataDealUtils::byteToInt(
            m_recvBuffer.mid(Constant::CardRobot::START_SIZE + Constant::CardRobot::VER_SIZE + Constant::CardRobot::SEQ_SIZE,
                             Constant::CardRobot::LEN_SIZE));

        // 计算数据总大小（单位：B）
        quint32 totalSize = Constant::CardRobot::START_SIZE + Constant::CardRobot::VER_SIZE + Constant::CardRobot::SEQ_SIZE
                            + Constant::CardRobot::LEN_SIZE + dataLen + Constant::CardRobot::CRC_SIZE;
        // 数据长度不足，等待更多后续数据到来
        if (static_cast<quint32>(m_recvBuffer.size()) < totalSize)
            return;

        QByteArray fullFrame = m_recvBuffer.left(totalSize);
        // 进行CRC校验
        QByteArray remoteCrc = fullFrame.right(Constant::CardRobot::CRC_SIZE);
        QByteArray localCrc = DataDealUtils::getCRCCode(
            fullFrame.mid(Constant::CardRobot::START_SIZE, totalSize - Constant::CardRobot::START_SIZE - Constant::CardRobot::CRC_SIZE));
        if (remoteCrc != localCrc) {
            QString localCrcStr = QString::fromLatin1(localCrc.toHex()).toUpper();
            QString remoteCrcStr = QString::fromLatin1(remoteCrc.toHex()).toUpper();
            LOG_CERROR("cardrobot").noquote() << QString("CRC校验失败(localCrc: %1, remoteCrc: %2)").arg(localCrcStr).arg(remoteCrcStr);
            m_recvBuffer.remove(0, Constant::CardRobot::START_SIZE);
            continue;
        }

        QByteArray command = fullFrame.mid(Constant::CardRobot::START_SIZE + Constant::CardRobot::VER_SIZE + Constant::CardRobot::SEQ_SIZE
                                               + Constant::CardRobot::LEN_SIZE,
                                           dataLen);
        // 处理单条指令
        dealCommand(seq, command);
        m_recvBuffer.remove(0, totalSize);
        continue;
    }
}

void CardRobotHandler::onRobotStatusChanged(ST_CardRobotStatusInfo info1, ST_CardRobotStatusInfo info2, ST_CardRobotStatusInfo info3,
                                            ST_CardRobotStatusInfo info4)
{
    m_statusInfo1 = info1;
    m_statusInfo2 = info2;
    m_statusInfo3 = info3;
    m_statusInfo4 = info4;
}

void CardRobotHandler::onRobotSendCommand(uchar cmdType, QByteArray json)
{
    NloJson nloJson;

    bool ok = false;
    QVariantMap dataMap = nloJson.parse(json, &ok).toMap();
    if (!ok) {
        LOG_CERROR("cardrobot").noquote() << "Json解析失败，数据内容错误";
        return;
    }

    switch (cmdType) {
    case 0x41: {
        sendCommand65();
    } break;
    case 0x42: {
        sendCommand66();
    } break;
    case 0x43: {
        uchar workStation = static_cast<uchar>(dataMap["workStation"].toInt());
        uchar boxNo = static_cast<uchar>(dataMap["boxNo"].toInt());
        sendCommand67(workStation, boxNo);
    } break;
    case 0x44: {
        uchar workStation = static_cast<uchar>(dataMap["workStation"].toInt());
        uchar boxNo = static_cast<uchar>(dataMap["boxNo"].toInt());
        sendCommand68(workStation, boxNo);
    } break;
    case 0x45: {
        uchar workStation = static_cast<uchar>(dataMap["workStation"].toInt());
        uchar boxNo = static_cast<uchar>(dataMap["boxNo"].toInt());
        sendCommand69(workStation, boxNo);
    } break;
    case 0x46: {
        ushort cardNum1 = static_cast<ushort>(dataMap["cardNum1"].toInt());
        ushort cardNum2 = static_cast<ushort>(dataMap["cardNum2"].toInt());
        ushort cardNum3 = static_cast<ushort>(dataMap["cardNum3"].toInt());
        ushort cardNum4 = static_cast<ushort>(dataMap["cardNum4"].toInt());
        sendCommand70(cardNum1, cardNum2, cardNum3, cardNum4);
    } break;
    case 0x47: {
        uchar workStation = static_cast<uchar>(dataMap["workStation"].toInt());
        uchar boxNo = static_cast<uchar>(dataMap["boxNo"].toInt());
        sendCommand71(workStation, boxNo);
    } break;
    case 0x48: {
        uchar workStation = static_cast<uchar>(dataMap["workStation"].toInt());
        uchar cause = static_cast<uchar>(dataMap["cause"].toInt());
        sendCommand72(workStation, cause);
    } break;
    case 0x49: {
        uchar workStation = static_cast<uchar>(dataMap["workStation"].toInt());
        uchar cause = static_cast<uchar>(dataMap["cause"].toInt());
        QString paperNum = dataMap["paperNum"].toString();
        uchar statusUp = static_cast<uchar>(dataMap["statusUp"].toInt());
        uchar statusDown = static_cast<uchar>(dataMap["statusDown"].toInt());
        sendCommand73(workStation, cause, paperNum, statusUp, statusDown);
    } break;
    default:
        break;
    }
}

void CardRobotHandler::dealCommand(uchar seq, const QByteArray &command)
{
    uchar cmdType = static_cast<uchar>(command.at(0));
    LOG_CDEBUG("cardrobot").noquote() << QString("开始处理指令0x%1[%2]: %3")
                                             .arg(QString("%1").arg(cmdType, 2, 16, QLatin1Char('0')).toUpper())
                                             .arg(seq, 2, 16, QLatin1Char('0'))
                                             .arg(DataDealUtils::byteArrayToHexStr(command));

    switch (cmdType) {
    case 0x31: { // 初始化指令
        QByteArray timeData = command.mid(1, 4);
        QByteArray dateTimeData = command.mid(5, 7);
        QByteArray cardData = command.mid(12, 3);

        quint32 time = DataDealUtils::byteToUInt(timeData);
        QString dateTime = QDateTime::fromString(DataDealUtils::byteArrayToBCDStr(dateTimeData), "yyyyMMddhhmmss").toString("yyyy-MM-dd hh:mm:ss");
        QString card = DataDealUtils::byteArrayToAsciiStr(cardData);

        LOG_CINFO("cardrobot").noquote() << QString("收到初始化指令: 当前时间戳 %1, 当前日期 %2, 卡夹最大数 %3").arg(time).arg(dateTime, card);
        sendResponse(cmdType, 1, (seq << 4 | seq >> 4));
    } break;
    case 0x32: { // 0x32 出卡指令
        QByteArray workStationData = command.mid(1, 1);
        uchar workStation = static_cast<uchar>(workStationData.at(0));

        LOG_CINFO("cardrobot").noquote() << QString("收到出卡指令: 工位 0x%1").arg(workStation, 2, 16, QLatin1Char('0'));
        sendResponse(cmdType, 1, (seq << 4 | seq >> 4));
        // NOTE 收到出卡指令后，接下来发送出卡信息
    } break;
    case 0x33: { // 0x33 坏卡指令
        QByteArray workStationData = command.mid(1, 1);
        uchar workStation = static_cast<uchar>(workStationData.at(0));

        LOG_CINFO("cardrobot").noquote() << QString("收到坏卡指令: 工位 0x%1").arg(workStation, 2, 16, QLatin1Char('0'));
        sendResponse(cmdType, 1, (seq << 4 | seq >> 4));
        // NOTE 收到坏卡指令后，接下来发送卡回收完成信息
    } break;
    case 0x34: { // 触发按键指令
        QByteArray workStationData = command.mid(1, 1);
        uchar workStation = static_cast<uchar>(workStationData.at(0));

        LOG_CINFO("cardrobot").noquote() << QString("收到触发按键指令: 工位 0x%1").arg(workStation, 2, 16, QLatin1Char('0'));
        sendResponse(cmdType, 1, (seq << 4 | seq >> 4));
        if (m_isAutoRespCommand52) {
            if (workStation == 0x30) {
                sendCommand68(m_defWorkStation, m_defWorkStation == 0x31 ? m_defUpBoxNo : m_defDownBoxNo);
            } else {
                sendCommand68(workStation, workStation == 0x31 ? m_defUpBoxNo : m_defDownBoxNo);
            }
        }
    } break;
    case 0x35: { // 卡回收到天线区
        QByteArray workStationData = command.mid(1, 4);

        QStringList parts;
        for (int i = 0; i < workStationData.size(); ++i) {
            uchar val = static_cast<uchar>(workStationData.at(i));
            parts << QString("卡机%1 0x%2").arg(i + 1).arg(val, 2, 16, QLatin1Char('0'));
        }

        QString msg = parts.join(", ");
        LOG_CINFO("cardrobot").noquote() << QString("收到卡回收到天线区指令: %1").arg(msg);
        sendResponse(cmdType, 1, (seq << 4 | seq >> 4));
    } break;
    case 0x36: { // 车道设备状态指令
        QByteArray statusData = command.mid(1, 1);
        uchar status = static_cast<uchar>(statusData.at(0));

        LOG_CINFO("cardrobot").noquote() << QString("收到车道设备状态指令: 状态 0x%1").arg(status, 2, 16, QLatin1Char('0'));
        sendResponse(cmdType, 1, (seq << 4 | seq >> 4));
    } break;
    case 0x37: { // 按键取消指令
        LOG_CINFO("cardrobot").noquote() << QString("收到按键取消指令");
        sendResponse(cmdType, 1, (seq << 4 | seq >> 4));
    } break;
    case 0x39: { // 界面显示指令
        QByteArray uiNoData = command.mid(1, 1);
        QByteArray uiInfoData = command.mid(2).trimmed();

        uchar uiNo = static_cast<uchar>(uiNoData.at(0));
        QString uiInfo = DataDealUtils::decodeByteArray(uiInfoData, 1);

        LOG_CINFO("cardrobot").noquote() << QString("收到界面显示指令: 编号 %1, 显示内容 %2").arg(uiNo).arg(uiInfo);
        sendResponse(cmdType, 1, (seq << 4 | seq >> 4));

        QVariantMap oneMap;
        oneMap["uiInfo"] = uiInfo;
        oneMap["uiNo"] = uiNo;

        NloJson nloJson;
        QByteArray json = nloJson.serialize(oneMap);
        emit GM_INSTANCE->m_signalMan->sigRobotReceiveCommand(0x39, json);
    } break;
    case 0x3A: { // 语音播放指令
        QByteArray voiceNumData = command.mid(1, 1);
        QByteArray playCountData = command.mid(2, 1);

        uchar voiceNum = static_cast<uchar>(voiceNumData.at(0));
        uchar playCount = static_cast<uchar>(playCountData.at(0));

        QString content = getVoiceContent(voiceNum);

        LOG_CINFO("cardrobot").noquote() << QString("收到语音播放指令: 编号 %1(%2), 播放次数 %3").arg(voiceNum).arg(content).arg(playCount);
        sendResponse(cmdType, 1, (seq << 4 | seq >> 4));

        QVariantMap oneMap;
        oneMap["voiceNum"] = voiceNum;
        oneMap["playCount"] = playCount;

        NloJson nloJson;
        QByteArray json = nloJson.serialize(oneMap);
        emit GM_INSTANCE->m_signalMan->sigRobotReceiveCommand(0x3A, json);
    } break;
    case 0x3B: { // 设置状态上报URL指令
        QByteArray upMinuteData = command.mid(1, 1);
        QByteArray urlData = command.mid(2);

        uchar upMinute = static_cast<uchar>(upMinuteData.at(0));
        QString url = QString::fromLatin1(urlData);

        LOG_CINFO("cardrobot").noquote() << QString("收到设置状态上报URL指令: 上传间隔分钟数 %1, URL地址 %2").arg(upMinute).arg(url);
        sendResponse(cmdType, 1, (seq << 4 | seq >> 4));
    } break;
    case 0x3D: { // 打印纸券
        QByteArray workStationData = command.mid(1, 1);
        QByteArray dataLenData = command.mid(2, 4);
        QByteArray dataData = command.mid(6);

        uchar workStation = static_cast<uchar>(workStationData.at(0));
        int dataLen = DataDealUtils::byteToInt(dataLenData);
        QString data = DataDealUtils::decodeByteArray(dataData, 1);

        LOG_CINFO("cardrobot").noquote() << QString("收到打印纸券指令: 工位 %1, 内容长度 %2, 内容 %3").arg(workStation).arg(dataLen).arg(data);
        sendResponse(cmdType, 1, (seq << 4 | seq >> 4));
    } break;
    case 0x50: { // 副屏显示
        QByteArray textLenData = command.mid(1, 2);
        ushort textLen = DataDealUtils::byteToUShort(textLenData);

        QByteArray textData = command.mid(3, textLen);
        QString text = DataDealUtils::decodeByteArray(textData, 1);

        QByteArray reserveData = command.mid(3 + textLen, 8);
        QString reserve = QString::fromLatin1(reserveData);

        LOG_CINFO("cardrobot").noquote() << QString("收到副屏显示指令: 文本长度 %1, 内容 %2, 预留 %3").arg(textLen).arg(text, reserve);
        sendResponse(cmdType, 1, (seq << 4 | seq >> 4));

        QVariantMap oneMap;
        oneMap["text"] = text;

        NloJson nloJson;
        QByteArray json = nloJson.serialize(oneMap);
        emit GM_INSTANCE->m_signalMan->sigRobotReceiveCommand(0x50, json);
    } break;
    case 0x51: { // 二维码显示
        QByteArray textLenData = command.mid(1, 2);
        ushort textLen = DataDealUtils::byteToUShort(textLenData);

        QByteArray textData = command.mid(3, textLen);
        QString text = DataDealUtils::decodeByteArray(textData, 1);

        QByteArray reserveData = command.mid(3 + textLen, 8);
        QString reserve = QString::fromLatin1(reserveData);

        LOG_CINFO("cardrobot").noquote() << QString("收到二维码显示指令: 文本长度 %1, 内容 %2, 预留 %3").arg(textLen).arg(text, reserve);
        sendResponse(cmdType, 1, (seq << 4 | seq >> 4));

        QVariantMap oneMap;
        oneMap["text"] = text;

        NloJson nloJson;
        QByteArray json = nloJson.serialize(oneMap);
        emit GM_INSTANCE->m_signalMan->sigRobotReceiveCommand(0x51, json);
    } break;
    case 0x41:   // 上电信息（响应）
    case 0x42:   // 状态信息（响应）
    case 0x43:   // 出卡信息（响应）
    case 0x44:   // 按键取卡信息（响应）
    case 0x45:   // 卡被取走信息（响应）
    case 0x46:   // 卡夹信息（响应）
    case 0x47:   // 卡回收完成信息（响应）
    case 0x48:   // 按键忽略信息（响应）
    case 0x49: { // 纸券取走及状态信息（响应）
        QByteArray statusData = command.mid(1, 1);
        uchar status = static_cast<uchar>(statusData.at(0));

        QString msg;
        if (cmdType == 0x41) {
            msg = QString("收到上电信息[0x%1]返回: 状态 0x%2");
        } else if (cmdType == 0x42) {
            msg = QString("收到状态信息[0x%1]返回: 状态 0x%2");
        } else if (cmdType == 0x43) {
            msg = QString("收到出卡信息[0x%1]返回: 状态 0x%2");
        } else if (cmdType == 0x44) {
            msg = QString("收到按键取卡信息[0x%1]返回: 状态 0x%2");
        } else if (cmdType == 0x45) {
            msg = QString("收到卡被取走信息[0x%1]返回: 状态 0x%2");
        } else if (cmdType == 0x46) {
            msg = QString("收到卡夹信息[0x%1]返回: 状态 0x%2");
        } else if (cmdType == 0x47) {
            msg = QString("收到卡回收完成信息[0x%1]返回: 状态 0x%2");
        } else if (cmdType == 0x48) {
            msg = QString("收到按键忽略信息[0x%1]返回: 状态 0x%2");
        } else {
            msg = QString("收到纸券取走及状态信息[0x%1]返回: 状态 0x%2");
        }
        if (cmdType == 0x42) {
            LOG_CDEBUG("cardrobot").noquote() << msg.arg(seq, 2, 16, QLatin1Char('0')).arg(status, 2, 16, QLatin1Char('0'));
        } else {
            LOG_CINFO("cardrobot").noquote() << msg.arg(seq, 2, 16, QLatin1Char('0')).arg(status, 2, 16, QLatin1Char('0'));
        }
    } break;
    default: {
        LOG_CWARNING("cardrobot").noquote() << "未知命令，忽略处理";
    } break;
    }
}

void CardRobotHandler::sendResponse(uchar cmdType, uchar status, uchar seq)
{
    QByteArray command;
    if (cmdType == 0x31) {
        command.append(cmdType);
        command.append(status);

        uchar hasWorkNoPsam1 = 0x00;
        QByteArray termNo1 = QByteArray(6, 0x00);
        QByteArray psamNo1 = QByteArray(10, 0x00);
        uchar isNeedAuth1 = 0x00;
        QByteArray random1 = QByteArray(16, 0x00);
        uchar slot1 = 0x00;

        command.append(hasWorkNoPsam1);
        command.append(termNo1);
        command.append(psamNo1);
        command.append(isNeedAuth1);
        command.append(random1);
        command.append(slot1);

        uchar hasWorkNoPsam2 = 0x00;
        QByteArray termNo2 = QByteArray(6, 0x00);
        QByteArray psamNo2 = QByteArray(10, 0x00);
        uchar isNeedAuth2 = 0x00;
        QByteArray random2 = QByteArray(16, 0x00);
        uchar slot2 = 0x00;

        command.append(hasWorkNoPsam2);
        command.append(termNo2);
        command.append(psamNo2);
        command.append(isNeedAuth2);
        command.append(random2);
        command.append(slot2);
    } else {
        command.append(cmdType);
        command.append(status);
    }

    // 组装发送帧
    QByteArray buffer;
    buffer.append(Constant::CardRobot::START);
    buffer.append(Constant::CardRobot::VER);
    buffer.append(seq);
    buffer.append(DataDealUtils::intToByte(command.size()));
    buffer.append(command);
    QByteArray crc = DataDealUtils::getCRCCode(buffer.mid(Constant::CardRobot::START_SIZE));
    buffer.append(crc);

    LOG_CDEBUG("cardrobot").noquote() << QString("返回应答0x%1[%2]: %3")
                                             .arg(QString("%1").arg(cmdType, 2, 16, QLatin1Char('0')).toUpper())
                                             .arg(seq)
                                             .arg(DataDealUtils::byteArrayToHexStr(buffer));

    sendPacket(cmdType, buffer);
}

QByteArray CardRobotHandler::constructSendData(const QByteArray &data)
{
    QByteArray sendData;
    sendData.append(Constant::CardRobot::START);
    sendData.append(Constant::CardRobot::VER);
    sendData.append(getServerSeq());
    sendData.append(DataDealUtils::intToByte(data.size()));
    sendData.append(data);
    QByteArray crc = DataDealUtils::getCRCCode(sendData.mid(Constant::CardRobot::START_SIZE));
    sendData.append(crc);

    LOG_CDEBUG("cardrobot").noquote() << "Put ※==>" << DataDealUtils::byteArrayToHexStr(sendData);

    return sendData;
}

QString CardRobotHandler::getVoiceContent(uchar voiceNum)
{
    QString res;
    switch (voiceNum) {
    case 10:
        res = "";
        break;
    case 11:
        res = "你好，请按键取卡";
        break;
    case 12:
        res = "请稍后";
        break;
    case 13:
        res = "请取卡";
        break;
    case 14:
        res = "谢谢一路平安";
        break;
    case 21:
        res = "卡机故障，请按求助键";
        break;
    case 22:
        res = "卡机无卡，请按求助键";
        break;
    case 23:
        res = "下工位故障，请从上工位取卡";
        break;
    case 24:
        res = "下工位无卡，请从上工位取卡";
        break;
    case 25:
        res = "上工位故障，请从下工位取卡";
        break;
    case 26:
        res = "上工位无卡，请从下工位取卡";
        break;
    case 27:
        res = "出卡异常";
        break;
    case 31:
        res = "车型无法识别，请稍后";
        break;
    case 32:
        res = "无车牌，请稍后";
        break;
    case 33:
        res = "轴型无法识别，请稍后";
        break;
    case 34:
        res = "欠费车辆，请按求助键";
        break;
    case 35:
        res = "ETC账户异常，请按键取卡";
        break;
    case 36:
        res = "路政黑名单，请按求助键";
        break;
    case 37:
        res = "超限车，请按求助键";
        break;
    case 38:
        res = "无称重数据，请按求助键";
        break;
    case 39:
        res = "异常车辆，请按求助键";
        break;
    case 40:
        res = "拖挂车，请稍后";
        break;
    }

    return res;
}

uchar CardRobotHandler::getServerSeq()
{
    static uchar x = 1;         // X 范围 1~9
    uchar ret = (0x8 << 4) | x; // 高位固定8，低位是x

    x++;
    if (x > 9) {
        x = 1; // 回到 1
    }
    return ret;
}

bool CardRobotHandler::sendCommand65()
{
    LOG_CINFO("cardrobot").noquote() << QString("发送卡机上电报文");

    QByteArray data;
    data.append('\x41');

    QByteArray sendData = constructSendData(data);
    return sendPacket(0x41, sendData);
}

bool CardRobotHandler::sendCommand66()
{
    LOG_CDEBUG("cardrobot").noquote() << QString("发送卡机状态报文: 上工位当前卡机编号 0x%1, 下工位当前卡机编号 0x%2, 卡机1状态 0x%3,"
                                                 "卡夹1状态 0x%4, 卡机1中的卡数量 %5, 卡机1的轨道有卡状态 0x%6, 卡机2状态 0x%7,"
                                                 "卡夹2状态 0x%8, 卡机2中的卡数量 %9, 卡机2的轨道有卡状态 0x%10, 卡机3状态 0x%11,"
                                                 "卡夹3状态 0x%12, 卡机3中的卡数量 %13, 卡机3的轨道有卡状态 0x%14, 卡机4状态 0x%15,"
                                                 "卡夹4状态 0x%16, 卡机4中的卡数量 %17, 卡机4的轨道有卡状态 0x%18")
                                             .arg(m_defUpBoxNo, 2, 16, QLatin1Char('0'))
                                             .arg(m_defDownBoxNo, 2, 16, QLatin1Char('0'))
                                             .arg(m_statusInfo1.boxStatus, 2, 16, QLatin1Char('0'))
                                             .arg(m_statusInfo1.hasCardBag, 2, 16, QLatin1Char('0'))
                                             .arg(m_statusInfo1.restCard)
                                             .arg(m_statusInfo1.hasAnnCard, 2, 16, QLatin1Char('0'))
                                             .arg(m_statusInfo2.boxStatus, 2, 16, QLatin1Char('0'))
                                             .arg(m_statusInfo2.hasCardBag, 2, 16, QLatin1Char('0'))
                                             .arg(m_statusInfo2.restCard)
                                             .arg(m_statusInfo2.hasAnnCard, 2, 16, QLatin1Char('0'))
                                             .arg(m_statusInfo3.boxStatus, 2, 16, QLatin1Char('0'))
                                             .arg(m_statusInfo3.hasCardBag, 2, 16, QLatin1Char('0'))
                                             .arg(m_statusInfo3.restCard)
                                             .arg(m_statusInfo3.hasAnnCard, 2, 16, QLatin1Char('0'))
                                             .arg(m_statusInfo4.boxStatus, 2, 16, QLatin1Char('0'))
                                             .arg(m_statusInfo4.hasCardBag, 2, 16, QLatin1Char('0'))
                                             .arg(m_statusInfo4.restCard)
                                             .arg(m_statusInfo4.hasAnnCard, 2, 16, QLatin1Char('0'));

    QByteArray data;
    data.append('\x42');
    data.append(m_defUpBoxNo);
    data.append(m_defDownBoxNo);
    data.append(m_statusInfo1.boxStatus);
    data.append(m_statusInfo1.hasCardBag);
    data.append(QString("%1").arg(m_statusInfo1.restCard, 3, 10, QLatin1Char('0')).toLatin1());
    data.append(m_statusInfo1.hasAnnCard);
    data.append(m_statusInfo2.boxStatus);
    data.append(m_statusInfo2.hasCardBag);
    data.append(QString("%1").arg(m_statusInfo2.restCard, 3, 10, QLatin1Char('0')).toLatin1());
    data.append(m_statusInfo2.hasAnnCard);
    data.append(m_statusInfo3.boxStatus);
    data.append(m_statusInfo3.hasCardBag);
    data.append(QString("%1").arg(m_statusInfo3.restCard, 3, 10, QLatin1Char('0')).toLatin1());
    data.append(m_statusInfo3.hasAnnCard);
    data.append(m_statusInfo4.boxStatus);
    data.append(m_statusInfo4.hasCardBag);
    data.append(QString("%1").arg(m_statusInfo4.restCard, 3, 10, QLatin1Char('0')).toLatin1());
    data.append(m_statusInfo4.hasAnnCard);

    QByteArray sendData = constructSendData(data);
    if (!m_connected || !m_client) {
        LOG_CDEBUG("cardrobot").noquote() << "网络异常，发送失败";
        return false;
    }

    qint64 ret = m_client->write(sendData);

    if (ret == -1) {
        LOG_CDEBUG("cardrobot").noquote() << "数据写入失败";
        return false;
    }

    m_client->flush();
    LOG_CDEBUG("cardrobot").noquote() << "数据已进入发送队列";

    return true;
}

bool CardRobotHandler::sendCommand67(uchar workStation, uchar boxNo)
{
    LOG_CINFO("cardrobot").noquote() << QString("发送出卡报文: 工位 0x%1, 卡机号 0x%2")
                                            .arg(workStation, 2, 16, QLatin1Char('0'))
                                            .arg(boxNo, 2, 16, QLatin1Char('0'));

    QByteArray data;
    data.append('\x43');
    data.append(workStation);
    data.append(boxNo);

    QByteArray sendData = constructSendData(data);
    return sendPacket(0x43, sendData);
}

bool CardRobotHandler::sendCommand68(uchar workStation, uchar boxNo)
{
    LOG_CINFO("cardrobot").noquote() << QString("发送按键取卡报文: 工位 0x%1, 卡机号 0x%2")
                                            .arg(workStation, 2, 16, QLatin1Char('0'))
                                            .arg(boxNo, 2, 16, QLatin1Char('0'));

    QByteArray data;
    data.append('\x44');
    data.append(workStation);
    data.append(boxNo);

    QByteArray sendData = constructSendData(data);
    return sendPacket(0x44, sendData);
}

bool CardRobotHandler::sendCommand69(uchar workStation, uchar boxNo)
{
    LOG_CINFO("cardrobot").noquote() << QString("发送卡被取走报文: 工位 0x%1, 卡机号 0x%2")
                                            .arg(workStation, 2, 16, QLatin1Char('0'))
                                            .arg(boxNo, 2, 16, QLatin1Char('0'));

    QByteArray data;
    data.append('\x45');
    data.append(workStation);
    data.append(boxNo);

    QByteArray sendData = constructSendData(data);
    return sendPacket(0x45, sendData);
}

bool CardRobotHandler::sendCommand70(ushort cardNum1, ushort cardNum2, ushort cardNum3, ushort cardNum4)
{
    LOG_CINFO("cardrobot").noquote() << QString("发送卡夹信息报文: 卡机1卡夹编号 %1, 卡机1卡数量 %2, 卡机2卡夹编号 %3, 卡机2卡数量 %4, 卡机3卡夹编号 "
                                                "%5, 卡机3卡数量 %6, 卡机4卡夹编号 %7, 卡机4卡数量 %8")
                                            .arg(m_bagNo1)
                                            .arg(cardNum1)
                                            .arg(m_bagNo2)
                                            .arg(cardNum2)
                                            .arg(m_bagNo3)
                                            .arg(cardNum3)
                                            .arg(m_bagNo4)
                                            .arg(cardNum4);

    QByteArray data;
    data.append('\x46');
    data.append(DataDealUtils::intToByte(m_bagNo1));
    data.append(DataDealUtils::ushortToByte(cardNum1));
    data.append(DataDealUtils::intToByte(m_bagNo2));
    data.append(DataDealUtils::ushortToByte(cardNum2));
    data.append(DataDealUtils::intToByte(m_bagNo3));
    data.append(DataDealUtils::ushortToByte(cardNum3));
    data.append(DataDealUtils::intToByte(m_bagNo4));
    data.append(DataDealUtils::ushortToByte(cardNum4));

    QByteArray sendData = constructSendData(data);
    return sendPacket(0x46, sendData);
}

bool CardRobotHandler::sendCommand71(uchar workStation, uchar boxNo)
{
    LOG_CINFO("cardrobot").noquote() << QString("发送卡回收完成报文: 工位 0x%1, 卡机编号 0x%2")
                                            .arg(workStation, 2, 16, QLatin1Char('0'))
                                            .arg(boxNo, 2, 16, QLatin1Char('0'));

    QByteArray data;
    data.append('\x47');
    data.append(workStation);
    data.append(boxNo);

    QByteArray sendData = constructSendData(data);
    return sendPacket(0x47, sendData);
}

bool CardRobotHandler::sendCommand72(uchar workStation, uchar cause)
{
    LOG_CINFO("cardrobot").noquote() << QString("发送按键忽略报文: 工位 0x%1, 原因 0x%2")
                                            .arg(workStation, 2, 16, QLatin1Char('0'))
                                            .arg(cause, 2, 16, QLatin1Char('0'));

    QByteArray data;
    data.append('\x48');
    data.append(workStation);
    data.append(cause);

    QByteArray sendData = constructSendData(data);
    return sendPacket(0x48, sendData);
}

bool CardRobotHandler::sendCommand73(uchar workStation, uchar cause, QString paperNum, uchar statusUp, uchar statusDown)
{
    LOG_CINFO("cardrobot").noquote() << QString(
                                            "发送纸券取走及状态报文: 工位 0x%1, 原因 0x%2, 纸券号 %3, 上工位打印机状态 0x%4, 下工位打印机状态 0x%5")
                                            .arg(workStation, 2, 16, QLatin1Char('0'))
                                            .arg(cause, 2, 16, QLatin1Char('0'))
                                            .arg(paperNum)
                                            .arg(statusUp, 2, 16, QLatin1Char('0'))
                                            .arg(statusDown, 2, 16, QLatin1Char('0'));

    QByteArray data;
    data.append('\x49');
    data.append(workStation);
    data.append(cause);
    data.append(paperNum.toLatin1().leftJustified(20, '\x00', true));
    data.append(statusUp);
    data.append(statusDown);

    QByteArray sendData = constructSendData(data);
    return sendPacket(0x49, sendData);
}

bool CardRobotHandler::sendPacket(uchar cmdType, const QByteArray &sendData)
{
    if (!m_connected || !m_client) {
        LOG_CERROR("cardrobot").noquote() << "网络异常，发送失败";
        emit GM_INSTANCE->m_signalMan->sigRobotSendCommandFinish(cmdType, false);
        return false;
    }

    qint64 ret = m_client->write(sendData);

    if (ret == -1) {
        LOG_CWARNING("cardrobot").noquote() << "数据写入失败";
        emit GM_INSTANCE->m_signalMan->sigRobotSendCommandFinish(cmdType, false);
        return false;
    }

    m_client->flush();
    LOG_CINFO("cardrobot").noquote() << "数据已进入发送队列";

    return true;
}
