#include "cmdhandler.h"

#include "Logger.h"
#include "defines.h"
#include "utils/datadealutils.h"

using namespace Utils;

CmdHandlerV1::CmdHandlerV1() {}

CmdHandlerV1::~CmdHandlerV1() {}

QByteArray CmdHandlerV1::handleB1Cmd(const QByteArray &cmd)
{
    LOG_CINFO(L_CATE).noquote() << "处理指令 B1:" << DataDealUtils::byteArrayToHexStr(cmd);

    QString dateTime = DataDealUtils::byteArrayToBCDStr(cmd.mid(1, 7));
    uchar type = static_cast<uchar>(cmd.at(8));

    if (type == 1) {
        int dataLen = DataDealUtils::byteToInt(cmd.mid(9, 4));
        QByteArray data = cmd.mid(13, dataLen);

        bool jsonOk = false;
        QString jsonErr;
        QVariantMap aMap = DataDealUtils::jsonToMap(data, &jsonOk, &jsonErr);
        if (!jsonOk) {
            LOG_CERROR(L_CATE).noquote() << "JsonData解析错误:" << jsonErr;
            return assembleF1Cmd(dateTime, type, 1, "JsonData解析错误");
        }

        int deviceStatus = aMap["devicestatus"].toInt();
        QString errDesc = aMap["errdesc"].toString();
        QString deviceID = aMap["deviceid"].toString();
        QString appVersion = aMap["appversion"].toString();
        QString osVersion = aMap["osversion"].toString();
        QString hwModel = aMap["hwmodel"].toString();

        LOG_CINFO(L_CATE).noquote() << "设备状态信息: 状态" << deviceStatus << "异常描述" << errDesc << "设备编号" << deviceID << "软件版本"
                                    << appVersion << "系统版本" << osVersion << "硬件型号" << hwModel;
        return assembleF1Cmd(dateTime, type, 0, "");
    } else {
        LOG_CERROR(L_CATE).noquote() << "未知数据类型" << type;
        return assembleF1Cmd(dateTime, type, 1, "未知数据类型");
    }
}

bool CmdHandlerV1::handleF1Cmd(const QByteArray &cmd)
{
    LOG_CINFO(L_CATE).noquote() << "处理指令 F1:" << DataDealUtils::byteArrayToHexStr(cmd);
    uchar type = static_cast<uchar>(cmd.at(8));
    uchar status = static_cast<uchar>(cmd.at(9));
    QString desc = QString::fromUtf8(cmd.mid(10));

    if (type == 0) {
        // 初始化指令返回
        if (status == 0) {
            LOG_CINFO(L_CATE).noquote() << "服务端返回设备初始化成功";
        } else {
            LOG_CERROR(L_CATE).noquote() << "服务端返回设备初始化失败:" << desc;
        }
        return status == 0;
    } else if (type == 1) {
        // 二维码显示指令返回
        if (status == 0) {
            LOG_CINFO(L_CATE).noquote() << "服务端返回二维码显示指令执行成功";
        } else {
            LOG_CERROR(L_CATE).noquote() << "服务端返回二维码显示指令执行失败:" << desc;
        }
        return status == 0;
    } else if (type == 2) {
        // 费显显示指令返回
        if (status == 0) {
            LOG_CINFO(L_CATE).noquote() << "服务端返回费显显示指令执行成功";
        } else {
            LOG_CERROR(L_CATE).noquote() << "服务端返回费显显示指令执行失败:" << desc;
        }
        return status == 0;
    } else if (type == 3) {
        // 图片显示指令返回
        if (status == 0) {
            LOG_CINFO(L_CATE).noquote() << "服务端返回图片显示指令执行成功";
        } else {
            LOG_CERROR(L_CATE).noquote() << "服务端返回图片显示指令执行失败:" << desc;
        }
        return status == 0;
    } else if (type == 4) {
        // 设置URL指令返回
        if (status == 0) {
            LOG_CINFO(L_CATE).noquote() << "服务端返回设置URL指令执行成功";
        } else {
            LOG_CERROR(L_CATE).noquote() << "服务端返回设置URL指令执行失败:" << desc;
        }
        return status == 0;
    } else {
        // 未知指令返回
        LOG_CERROR(L_CATE).noquote() << "服务端返回未知指令";
        return false;
    }
}

QByteArray CmdHandlerV1::assembleA1Cmd(uchar type, const QByteArray &data)
{
    QByteArray cmd;
    cmd.append(uchar(0xA1));
    cmd.append(DataDealUtils::bcdStrToByteArray(DataDealUtils::curDateTimeStr("yyyyMMddhhmmss")));
    cmd.append(type);
    cmd.append(DataDealUtils::intToByte(data.size()));
    cmd.append(data);

    return cmd;
}

QByteArray CmdHandlerV1::assembleF1Cmd(const QString &dateTime, uchar type, uchar status, const QString &desc)
{
    QByteArray cmd;
    cmd.append(uchar(0xF1));
    cmd.append(DataDealUtils::bcdStrToByteArray(dateTime));
    cmd.append(type);
    cmd.append(status);
    cmd.append(desc.toUtf8());

    return cmd;
}

QByteArray CmdHandlerV1::makeRequestKey(uchar seq, const QByteArray &cmd)
{
    QByteArray key;
    key.append(seq);
    key.append(cmd.mid(1, 8)); // DateTime + Type
    return key;
}

uchar CmdHandlerV1::getB1Type(const QByteArray &cmd)
{
    uchar type = static_cast<uchar>(cmd.at(8));
    return type;
}
