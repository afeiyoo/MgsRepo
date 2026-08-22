#pragma once

#include <QByteArray>

// 策略接口
class ICmdHandler
{
public:
    ICmdHandler() = default;
    virtual ~ICmdHandler() = default;

    // 解析D2指令
    virtual QByteArray handleD2Cmd(uchar seq, const QByteArray &cmd) = 0;

    // 解析D3指令
    virtual QByteArray handleD3Cmd(uchar seq, const QByteArray &cmd) = 0;

    // 解析D6指令
    virtual QByteArray handleD6Cmd(uchar seq, const QByteArray &cmd) = 0;

    // 解析A1指令（应答）
    virtual bool handleA1Cmd(uchar seq, const QByteArray &cmd) = 0;

    // 解析A2指令（应答）
    virtual bool handleA2Cmd(uchar seq, const QByteArray &cmd) = 0;

    // 解析A3指令（应答）
    virtual bool handleA3Cmd(uchar seq, const QByteArray &cmd) = 0;

    // 获取指令类型
    virtual uchar getCmdType(const QByteArray &cmd) = 0;

    // 组装A1指令（发送）
    virtual QByteArray assembleA1Cmd(const QMap<int, int> &relayMap, const QMap<int, int> &levelMap) = 0;

    // 组装A2指令（发送）
    virtual QByteArray assembleA2Cmd(const QByteArray &url, uchar time) = 0;

    // 组装A3指令（发送）
    virtual QByteArray assembleA3Cmd(uchar type, const QByteArray &data) = 0;
};

// 具体策略：针对版本0的命令处理
class CmdHandlerV0 : public ICmdHandler
{
public:
    CmdHandlerV0();
    ~CmdHandlerV0() override;

    uchar getCmdType(const QByteArray &cmd) override;

    QByteArray handleD2Cmd(uchar seq, const QByteArray &cmd) override;

    QByteArray handleD3Cmd(uchar seq, const QByteArray &cmd) override;

    QByteArray handleD6Cmd(uchar seq, const QByteArray &cmd) override;

    bool handleA1Cmd(uchar seq, const QByteArray &cmd) override;

    bool handleA2Cmd(uchar seq, const QByteArray &cmd) override;

    bool handleA3Cmd(uchar seq, const QByteArray &cmd) override;

    QByteArray assembleA1Cmd(const QMap<int, int> &relayMap, const QMap<int, int> &levelMap) override;

    QByteArray assembleA2Cmd(const QByteArray &url, uchar time) override;

    QByteArray assembleA3Cmd(uchar type, const QByteArray &data) override;
};
