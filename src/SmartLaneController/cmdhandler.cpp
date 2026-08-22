#include "cmdhandler.h"

#include "Logger.h"
#include "defines.h"
#include "utils/datadealutils.h"

using namespace Utils;

CmdHandlerV0::CmdHandlerV0() {}

CmdHandlerV0::~CmdHandlerV0() {}

uchar CmdHandlerV0::getCmdType(const QByteArray &cmd)
{
    uchar cmdType = static_cast<uchar>(cmd.at(0));
    return cmdType;
}

QByteArray CmdHandlerV0::handleD2Cmd(uchar seq, const QByteArray &cmd)
{
    LOG_CINFO(L_CATE).noquote() << QString("处理指令[0xD2][0x%1]:").arg(seq, 2, 16, QLatin1Char('0')) << DataDealUtils::byteArrayToHexStr(cmd);

    QByteArray resp;
    resp.append(uchar(0xD2));
    resp.append(uchar(0x00));
    return resp;
}

QByteArray CmdHandlerV0::handleD3Cmd(uchar seq, const QByteArray &cmd)
{
    LOG_CINFO(L_CATE).noquote() << QString("处理指令[0xD3][0x%1]:").arg(seq, 2, 16, QLatin1Char('0')) << DataDealUtils::byteArrayToHexStr(cmd);

    QByteArray resp;
    resp.append(uchar(0xD3));
    resp.append(uchar(0x00));
    return resp;
}

QByteArray CmdHandlerV0::handleD6Cmd(uchar seq, const QByteArray &cmd)
{
    LOG_CINFO(L_CATE).noquote() << QString("处理指令[0xD6][0x%1]:").arg(seq, 2, 16, QLatin1Char('0')) << DataDealUtils::byteArrayToHexStr(cmd);

    QByteArray resp;
    resp.append(uchar(0xD6));
    resp.append(uchar(0x00));
    return resp;
}

bool CmdHandlerV0::handleA1Cmd(uchar seq, const QByteArray &cmd)
{
    LOG_CINFO(L_CATE).noquote() << QString("处理指令[0xA1][0x%1]:").arg(seq, 2, 16, QLatin1Char('0')) << DataDealUtils::byteArrayToHexStr(cmd);
    uchar status = static_cast<uchar>(cmd.at(1));
    if (status == 0x00) {
        LOG_CINFO(L_CATE).noquote() << QString("服务端返回指令[0xA1][0x%1]处理成功").arg(static_cast<uchar>(seq << 4), 2, 16, QLatin1Char('0'));
    } else {
        LOG_CINFO(L_CATE).noquote() << QString("服务端返回指令[0xA1][0x%1]处理失败").arg(static_cast<uchar>(seq << 4), 2, 16, QLatin1Char('0'));
    }
    return status == 0x00;
}

bool CmdHandlerV0::handleA2Cmd(uchar seq, const QByteArray &cmd)
{
    LOG_CINFO(L_CATE).noquote() << QString("处理指令[0xA2][0x%1]:").arg(seq, 2, 16, QLatin1Char('0')) << DataDealUtils::byteArrayToHexStr(cmd);
    uchar status = static_cast<uchar>(cmd.at(1));
    if (status == 0x00) {
        LOG_CINFO(L_CATE).noquote() << QString("服务端返回指令[0xA2][0x%1]处理成功").arg(static_cast<uchar>(seq << 4), 2, 16, QLatin1Char('0'));
    } else {
        LOG_CINFO(L_CATE).noquote() << QString("服务端返回指令[0xA2][0x%1]处理失败").arg(static_cast<uchar>(seq << 4), 2, 16, QLatin1Char('0'));
    }
    return status == 0x00;
}

bool CmdHandlerV0::handleA3Cmd(uchar seq, const QByteArray &cmd)
{
    LOG_CINFO(L_CATE).noquote() << QString("处理指令[0xA3][0x%1]:").arg(seq, 2, 16, QLatin1Char('0')) << DataDealUtils::byteArrayToHexStr(cmd);
    uchar status = static_cast<uchar>(cmd.at(1));
    if (status == 0x00) {
        LOG_CINFO(L_CATE).noquote() << QString("服务端返回指令[0xA3][0x%1]处理成功").arg(static_cast<uchar>(seq << 4), 2, 16, QLatin1Char('0'));
    } else {
        LOG_CINFO(L_CATE).noquote() << QString("服务端返回指令[0xA3][0x%1]处理失败").arg(static_cast<uchar>(seq << 4), 2, 16, QLatin1Char('0'));
    }
    return status == 0x00;
}

QByteArray CmdHandlerV0::assembleA1Cmd(const QMap<int, int> &relayMap, const QMap<int, int> &levelMap)
{
    QByteArray cmd;
    cmd.append(uchar(0xA1));
    cmd.append(uchar(0x10));
    for (int i = 1; i <= 16; ++i) {
        cmd.append(uchar(i));

        uchar enable = 0;
        int level = levelMap.value(i, 1); // 电平位，默认高电平触发
        int want = relayMap.value(i, 0);  // 控制位，默认关闭
        if (level == 1) {
            enable = want ? 1 : 0;
        } else {
            enable = want ? 0 : 1;
        }
        cmd.append(enable);
    }

    return cmd;
}

QByteArray CmdHandlerV0::assembleA2Cmd(const QByteArray &url, uchar time)
{
    QByteArray cmd;
    cmd.append(uchar(0xA2));
    cmd.append(time);
    cmd.append(DataDealUtils::intToByte(url.size()));
    cmd.append(url);

    return cmd;
}

QByteArray CmdHandlerV0::assembleA3Cmd(uchar type, const QByteArray &data)
{
    QByteArray cmd;
    cmd.append(uchar(0xA3));
    cmd.append(type);
    cmd.append(DataDealUtils::intToByte(data.size()));
    cmd.append(data);

    return cmd;
}
