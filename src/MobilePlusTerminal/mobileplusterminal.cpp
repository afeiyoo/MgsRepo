#include "mobileplusterminal.h"

#include "Logger.h"
#include "cmdhandler.h"
#include "defines.h"
#include "utils/datadealutils.h"
#include "utils/stdafx.h"

using namespace Utils;

MobilePlusTerminal::MobilePlusTerminal(const QString &stationID, uint laneID, uint seq, QObject *parent)
    : IMobilePlusTerminal{parent}
{
    m_stationID = stationID;
    m_laneID = laneID;
    m_devSeq = seq;

    m_socket = new QTcpSocket(this);
    m_handler = new CmdHandlerV1(); // 默认版本号V1

    connect(m_socket, &QTcpSocket::stateChanged, this, &MobilePlusTerminal::onStageChanged);
    connect(m_socket, &QTcpSocket::readyRead, this, &MobilePlusTerminal::onReadyRead);
}

MobilePlusTerminal::~MobilePlusTerminal()
{
    SAFE_DELETE(m_handler);
}

void MobilePlusTerminal::connectServer(const QString &ip, quint16 port)
{
    m_isForceDisconnect = false;

    m_peerAddr = ip;
    m_peerPort = port;

    LOG_CINFO(L_CATE).noquote() << QString("开始连接手机+自助交易终端(IP: %1, Port: %2)").arg(ip).arg(port);
    m_socket->connectToHost(ip, port);
}

void MobilePlusTerminal::disconnectServer()
{
    m_isForceDisconnect = true;
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

    QByteArray cmd = m_handler->assembleA1Cmd(0, jsonData);
    QByteArray frame = makeFrame(getClientSeq(), cmd);

    LOG_INFO().noquote() << "发送指令 A1 Type:0";
    // 向服务端发送初始化指令
    if (!sendFrame(frame))
        return;
}

void MobilePlusTerminal::showQRCode(const QString &stationName, const QString &vehClass, const QString &vehPlate, const QString &barCode)
{
    QVariantMap aMap;
    aMap["stationname"] = stationName;
    aMap["vehclass"] = vehClass;
    aMap["vehplate"] = vehPlate;
    aMap["barcode"] = barCode;

    QByteArray jsonData = DataDealUtils::mapToJson(aMap);

    QByteArray cmd = m_handler->assembleA1Cmd(1, jsonData);
    QByteArray frame = makeFrame(getClientSeq(), cmd);

    LOG_INFO().noquote() << "发送指令 A1 Type:1";
    if (!sendFrame(frame))
        return;
}

void MobilePlusTerminal::showLED(const QString &text)
{
    QVariantMap aMap;
    aMap["text"] = text;

    QByteArray jsonData = DataDealUtils::mapToJson(aMap);

    QByteArray cmd = m_handler->assembleA1Cmd(2, jsonData);
    QByteArray frame = makeFrame(getClientSeq(), cmd);

    LOG_INFO().noquote() << "发送指令 A1 Type:2";
    if (!sendFrame(frame))
        return;
}

void MobilePlusTerminal::showPics(const QByteArray &data)
{
    QVariantMap aMap;
    aMap["pic"] = QString::fromLatin1(data.toBase64());

    QByteArray jsonData = DataDealUtils::mapToJson(aMap);

    QByteArray cmd = m_handler->assembleA1Cmd(3, jsonData);
    QByteArray frame = makeFrame(getClientSeq(), cmd);

    LOG_INFO().noquote() << "发送指令 A1 Type:3";
    if (!sendFrame(frame))
        return;
}

void MobilePlusTerminal::setUploadUrl(const QString &url, int time)
{
    QVariantMap aMap;
    aMap["url"] = url;
    aMap["time"] = time;

    QByteArray jsonData = DataDealUtils::mapToJson(aMap);

    QByteArray cmd = m_handler->assembleA1Cmd(4, jsonData);
    QByteArray frame = makeFrame(getClientSeq(), cmd);

    LOG_INFO().noquote() << "发送指令 A1 Type:4";
    if (!sendFrame(frame))
        return;
}

void MobilePlusTerminal::setVersion(uchar ver)
{
    if (ver == 0x01) {
        m_handler = new CmdHandlerV1();
    } else {
        LOG_CERROR(L_CATE).noquote() << "不支持的协议版本:" << ver;
        return;
    }
    m_ver = ver;
}

void MobilePlusTerminal::onStageChanged(QAbstractSocket::SocketState state)
{
    switch (state) {
    case QAbstractSocket::ConnectedState:
        LOG_CINFO("smartctrl").noquote() << "与手机+自助交易终端建立连接";
        m_isForceDisconnect = false;
        m_connected = true;

        initialize(m_stationID, m_laneID, m_devSeq);
        break;
    case QAbstractSocket::UnconnectedState:
        LOG_CERROR("smartctrl").noquote() << "与手机+自助交易终端断开连接";
        m_connected = false;
        m_buffer.clear(); // 清空数据缓冲区
        break;
    default:
        break;
    }
}

void MobilePlusTerminal::onReadyRead()
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

bool MobilePlusTerminal::sendFrame(const QByteArray &data)
{
    LOG_INFO().noquote() << QString("【TX】[%1:%2]").arg(m_peerAddr).arg(m_peerPort) << DataDealUtils::byteArrayToHexStr(data);

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
    uchar cmdType = static_cast<uchar>(cmd.at(0));
    if (cmdType == 0xB1) {
        QByteArray resp = m_handler->handleB1Cmd(cmd);
        sendFrame(makeFrame(static_cast<uchar>((seq & 0x0F) | 0x10), resp));
    } else if (cmdType == 0xF1) {
        m_handler->handleF1Cmd(cmd);
    } else {
        LOG_CERROR(L_CATE).noquote() << "未知指令类型:" << QString("%1").arg(cmdType, 2, 16, QLatin1Char('0'));
    }
}

// --------------------------------------------------------
IMobilePlusTerminal *createMobilePlusTerminal(const QString &stationID, uint laneID, uint seq)
{
    if (laneID > 99 || seq > 99)
        return nullptr;

    return new MobilePlusTerminal(stationID, laneID, seq);
}

void destroyMobilePlusTerminal(IMobilePlusTerminal *terminal)
{
    delete terminal;
}
