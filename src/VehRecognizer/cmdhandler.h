#pragma once

#include <QByteArray>
#include <QString>

// 策略接口
struct ST_EAHandleResult;
struct ST_EBHandleResult;
class ICmdHandler
{
public:
    ICmdHandler() = default;
    virtual ~ICmdHandler() = default;

    // 解析EA指令
    virtual ST_EAHandleResult handleEACmd(const QByteArray &cmd) = 0;

    // 解析EB指令
    virtual ST_EBHandleResult handleEBCmd(const QByteArray &cmd) = 0;

    // 解析EC指令
    virtual QByteArray handleECCmd(const QByteArray &cmd) = 0;

    // 组装AA指令
    virtual QByteArray assembleAACmd(const QString &stationID, const QString &stationName, int laneID) = 0;

    // 组装AB指令
    virtual QByteArray assembleABCmd(const QString &url, uchar minutes) = 0;

    // 组装AC指令
    virtual QByteArray assembleACCmd(uchar color, const QString &data) = 0;

    // 组装AD指令
    virtual QByteArray assembleADCmd(uchar count, const QString &text, int intervalMs) = 0;

    // 获取指令类型
    virtual uchar getCmdType(const QByteArray &cmd) = 0;

    // 处理设备应答
    virtual bool handleResponse(const QByteArray &cmd) = 0;
};

// 具体策略：针对版本1的命令处理
class CmdHandlerV1 : public ICmdHandler
{
public:
    CmdHandlerV1();
    ~CmdHandlerV1() override;

    ST_EAHandleResult handleEACmd(const QByteArray &cmd) override;

    ST_EBHandleResult handleEBCmd(const QByteArray &cmd) override;

    QByteArray handleECCmd(const QByteArray &cmd) override;

    QByteArray assembleAACmd(const QString &stationID, const QString &stationName, int laneID) override;

    QByteArray assembleABCmd(const QString &url, uchar minutes) override;

    QByteArray assembleACCmd(uchar color, const QString &data) override;

    QByteArray assembleADCmd(uchar count, const QString &text, int interval) override;

    uchar getCmdType(const QByteArray &cmd) override;

    bool handleResponse(const QByteArray &cmd) override;
};
