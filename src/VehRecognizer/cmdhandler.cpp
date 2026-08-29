#include "cmdhandler.h"

#include <QDateTime>

#include "Logger.h"
#include "defines.h"
#include "utils/datadealutils.h"

using namespace Utils;

CmdHandlerV1::CmdHandlerV1() {}

CmdHandlerV1::~CmdHandlerV1() {}

ST_EAHandleResult CmdHandlerV1::handleEACmd(const QByteArray &cmd)
{
    LOG_CINFO(L_CATE).noquote() << "处理指令 EA:" << DataDealUtils::byteArrayToHexStr(cmd);

    ST_EAHandleResult result;

    const QByteArray vehPlateData = DataDealUtils::bufferToByteArray(cmd.constData() + 1, 20); // 车牌，GBK编码
    QString vehPlate = DataDealUtils::decodeByteArray(vehPlateData, 1).trimmed();
    if (vehPlate.isEmpty() || vehPlate.contains("无车牌")) {
        vehPlate = "整牌拒识";
    }

    const uchar plateColor = static_cast<uchar>(cmd.at(21)); // 车牌颜色

    const bool plateColorValid = plateColor <= 6 || plateColor == 9;
    if (!plateColorValid) {
        LOG_CERROR(L_CATE).noquote() << "EA指令字段非法:" << "VehColor:" << plateColor;

        return result;
    }

    const QString vehTimeText = DataDealUtils::byteArrayToBCDStr(cmd.mid(22, 7)); // 过车时间
    const QDateTime vehTime = QDateTime::fromString(vehTimeText, "yyyyMMddhhmmss");

    const uchar vehClass = static_cast<uchar>(cmd.at(29)); // 车型

    const QByteArray axleTypeData = DataDealUtils::bufferToByteArray(cmd.constData() + 30, 20);
    const QString axleType = QString::fromLatin1(axleTypeData); // 轴型，ASCII字符串

    const uchar axleCount = static_cast<uchar>(cmd.at(50)); // 轴数

    const QString totalLengthText = DataDealUtils::byteArrayToBCDStr(cmd.mid(51, 3));
    const QString totalWidthText = DataDealUtils::byteArrayToBCDStr(cmd.mid(54, 3));
    const QString totalHeightText = DataDealUtils::byteArrayToBCDStr(cmd.mid(57, 3));

    result.vehPlate = vehPlate;
    result.plateColor = plateColor;
    result.vehTime = vehTime;
    result.vehClass = vehClass;
    result.axleType = axleType;
    result.axleCount = axleCount;
    result.totalLength = totalLengthText.toInt();
    result.totalWidth = totalWidthText.toInt();
    result.totalHeight = totalHeightText.toInt();
    result.extFlag = DataDealUtils::byteToUInt(cmd.mid(60, 4));
    result.direction = static_cast<uchar>(cmd.at(64));
    result.status = true;

    LOG_CINFO(L_CATE).noquote() << "车型信息:"
                                << "VehPlate:" << result.vehPlate << "VehColor:" << result.plateColor
                                << "VehTime:" << result.vehTime.toString("yyyy-MM-dd HH:mm:ss") << "VehClass:" << result.vehClass
                                << "AxleType:" << result.axleType << "AxleCount:" << result.axleCount << "TotalLength:" << result.totalLength
                                << "TotalWidth:" << result.totalWidth << "TotalHeight:" << result.totalHeight
                                << "ExtFlag:" << QString("0x%1").arg(result.extFlag, 8, 16, QLatin1Char('0')).toUpper()
                                << "DirectionFlag:" << result.direction;

    return result;
}

ST_EBHandleResult CmdHandlerV1::handleEBCmd(const QByteArray &cmd)
{
    LOG_CINFO(L_CATE).noquote() << "处理指令 EB(前 2048 字节):" << DataDealUtils::byteArrayToHexStr(cmd.left(2048));

    ST_EBHandleResult result;

    const QByteArray vehPlateData = DataDealUtils::bufferToByteArray(cmd.constData() + 1, 20); // 车牌，GBK编码
    QString vehPlate = DataDealUtils::decodeByteArray(vehPlateData, 1).trimmed();
    if (vehPlate.isEmpty() || vehPlate.contains("无车牌")) {
        vehPlate = "整牌拒识";
    }

    const uchar plateColor = static_cast<uchar>(cmd.at(21)); // 车牌颜色

    const QString vehTimeText = DataDealUtils::byteArrayToBCDStr(cmd.mid(22, 7)); // 过车时间
    const QDateTime vehTime = QDateTime::fromString(vehTimeText, "yyyyMMddhhmmss");

    const uchar imageType = static_cast<uchar>(cmd.at(29));             // 图像类型
    const quint32 imageLen = DataDealUtils::byteToUInt(cmd.mid(30, 4)); // 图像长度

    const bool plateColorValid = plateColor <= 6 || plateColor == 9;
    const bool imageTypeValid = imageType >= 1 && imageType <= 4;
    const quint32 maxImageLen = imageType == 4 ? MAX_VED_SIZE : MAX_IMG_SIZE;
    if (!plateColorValid || !imageTypeValid || imageLen > maxImageLen) {
        LOG_CERROR(L_CATE).noquote() << "EB指令字段非法:"
                                     << "VehColor:" << plateColor << "ImageType:" << imageType << "ImageLen:" << imageLen;

        return result;
    }

    result.vehPlate = vehPlate;
    result.plateColor = plateColor;
    result.vehTime = vehTime;
    result.imgType = imageType;
    result.imgInfo = cmd.mid(34, static_cast<int>(imageLen));
    result.status = true;

    LOG_CINFO(L_CATE).noquote() << "图片信息:"
                                << "VehPlate:" << result.vehPlate << "VehColor:" << result.plateColor
                                << "VehTime:" << result.vehTime.toString("yyyy-MM-dd HH:mm:ss") << "ImageType:" << result.imgType
                                << "ImageLen:" << result.imgInfo.size();

    return result;
}

QByteArray CmdHandlerV1::handleECCmd(const QByteArray &cmd)
{
    LOG_CINFO(L_CATE).noquote() << "处理指令 EC:" << DataDealUtils::byteArrayToHexStr(cmd);

    const QByteArray cameraIpData = DataDealUtils::bufferToByteArray(cmd.constData() + 1, 16);
    const QString cameraIP = QString::fromLatin1(cameraIpData);
    const QString stationHex = DataDealUtils::byteArrayToBCDStr(cmd.mid(17, 4));

    const QString laneNumText = DataDealUtils::byteArrayToBCDStr(cmd.mid(21, 2));
    const uint laneNum = laneNumText.toUInt();

    const quint32 timestamp = DataDealUtils::byteToUInt(cmd.mid(23, 4));
    const QDateTime currentTime = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(timestamp), Qt::LocalTime);

    const uchar workStatus = static_cast<uchar>(cmd.at(27));

    LOG_CINFO(L_CATE).noquote() << "车型识别器状态信息:"
                                << "CameraIP:" << cameraIP << "StationHex:" << stationHex << "LaneNum:" << laneNum
                                << "CurrentTime:" << currentTime.toString("yyyy-MM-dd HH:mm:ss") << "WorkStatus:" << workStatus;

    QByteArray response;
    response.append(uchar(0xEC));
    response.append(uchar(0x01));
    return response;
}

QByteArray CmdHandlerV1::assembleAACmd(const QString &stationID, const QString &stationName, int laneID)
{
    QByteArray cmd;
    cmd.append(uchar(0xAA));

    const uint seconds = DataDealUtils::curUnixDateTime();
    cmd.append(DataDealUtils::intToByte(static_cast<int>(seconds)));

    const QString dateTime = DataDealUtils::curDateTimeStr("yyyyMMddhhmmss");
    cmd.append(DataDealUtils::bcdStrToByteArray(dateTime));

    const QString stationHex = QString("3501%1").arg(stationID);
    cmd.append(DataDealUtils::bcdStrToByteArray(stationHex));

    const QString laneNum = QString("%1").arg(laneID, 4, 10, QLatin1Char('0'));
    cmd.append(DataDealUtils::bcdStrToByteArray(laneNum));

    QByteArray reserve = DataDealUtils::encodeString(stationName, 1);
    reserve.truncate(20);
    reserve.append(QByteArray(20 - reserve.size(), '\0')); // 不足用0x00填充
    cmd.append(reserve);

    return cmd;
}

QByteArray CmdHandlerV1::assembleABCmd(const QString &url, uchar minutes)
{
    QByteArray cmd;
    cmd.append(uchar(0xAB));
    cmd.append(minutes);
    cmd.append(url.toLatin1());

    return cmd;
}

QByteArray CmdHandlerV1::assembleACCmd(uchar color, const QString &data)
{
    QByteArray cmd;
    cmd.append(uchar(0xAC));
    cmd.append(color);

    QByteArray encodeData = DataDealUtils::encodeString(data, 1);
    int dataSize = encodeData.size();
    cmd.append(DataDealUtils::intToByte(dataSize));
    cmd.append(encodeData);

    return cmd;
}

QByteArray CmdHandlerV1::assembleADCmd(uchar count, const QString &text, int interval)
{
    QByteArray cmd;
    cmd.append(uchar(0xAD));
    cmd.append(count);
    if (interval == 0)
        interval = 3000; // 如果为0时，默认为3000ms
    cmd.append(DataDealUtils::intToByte(interval));

    QByteArray data = DataDealUtils::encodeString(text, 1);
    int textLen = data.size();
    cmd.append(DataDealUtils::intToByte(textLen));
    cmd.append(data);

    QByteArray reserve(8, '\0');
    cmd.append(reserve);

    return cmd;
}

uchar CmdHandlerV1::getCmdType(const QByteArray &cmd)
{
    return static_cast<uchar>(cmd.at(0));
}

bool CmdHandlerV1::handleResponse(const QByteArray &cmd)
{
    uchar cmdType = getCmdType(cmd);
    QString cmdTypeStr = QString("%1").arg(cmdType, 2, 16, QLatin1Char('0')).toUpper();
    LOG_CINFO(L_CATE).noquote() << QString("处理应答 %1:").arg(cmdTypeStr) << DataDealUtils::byteArrayToHexStr(cmd);

    uchar status = static_cast<uchar>(cmd.at(1));

    if (cmdType == 0xAA) {
        if (status == 1) {
            LOG_CINFO(L_CATE).noquote() << "服务端返回设备初始化成功";
        } else {
            LOG_CERROR(L_CATE).noquote() << "服务端返回设备初始化失败";
        }
    } else if (cmdType == 0xAB) {
        if (status == 1) {
            LOG_CINFO(L_CATE).noquote() << "服务端返回设置状态上报URL成功";
        } else {
            LOG_CERROR(L_CATE).noquote() << "服务端返回设置状态上报URL失败";
        }
    } else if (cmdType == 0xAC) {
        if (status == 1) {
            LOG_CINFO(L_CATE).noquote() << "服务端返回LED显示成功";
        } else {
            LOG_CERROR(L_CATE).noquote() << "服务端返回LED显示失败";
        }
    } else if (cmdType == 0xAD) {
        if (status == 1) {
            LOG_CINFO(L_CATE).noquote() << "服务端返回语音播报成功";
        } else {
            LOG_CERROR(L_CATE).noquote() << "服务端返回语音播报失败";
        }
    } else {
        // 未知指令返回
        LOG_CERROR(L_CATE).noquote() << "服务端返回未知指令";
        return false;
    }

    return status == 1;
}
