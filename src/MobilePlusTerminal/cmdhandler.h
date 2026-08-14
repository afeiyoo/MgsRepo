#pragma once

#include <QByteArray>
#include <QtGlobal>

// 策略接口
class ICmdHandler
{
public:
    ICmdHandler() = default;
    virtual ~ICmdHandler() = default;

    // 解析B1指令
    virtual QByteArray handleB1Cmd(const QByteArray &cmd) = 0;

    // 解析F1指令
    virtual bool handleF1Cmd(const QByteArray &cmd) = 0;

    // 组装A1指令
    virtual QByteArray assembleA1Cmd(uchar type, const QByteArray &data) = 0;

    // 组装F1指令
    virtual QByteArray assembleF1Cmd(const QString &dateTime, uchar type, uchar status, const QString &desc) = 0;

    // 设置版本号
    void setVersion(const QByteArray &ver);

protected:
    QByteArray m_ver = QByteArray::fromHex("01"); // 版本号
};

// 具体策略：针对版本1的命令处理
class CmdHandlerV1 : public ICmdHandler
{
public:
    CmdHandlerV1();
    ~CmdHandlerV1() override;

    QByteArray handleB1Cmd(const QByteArray &cmd) override;

    bool handleF1Cmd(const QByteArray &cmd) override;

    QByteArray assembleA1Cmd(uchar type, const QByteArray &data) override;

    QByteArray assembleF1Cmd(const QString &dateTime, uchar type, uchar status, const QString &desc) override;
};
