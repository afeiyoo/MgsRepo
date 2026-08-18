#pragma once

#include <QByteArray>
#include <QString>
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

    // 生成唯一请求标识键
    virtual QByteArray makeRequestKey(uchar seq, const QByteArray &cmd) = 0;

    // 获取B1指令类型
    virtual uchar getB1Type(const QByteArray &cmd) = 0;
};

// 具体策略：针对版本1的命令处理
class CmdHandlerV1 : public ICmdHandler
{
public:
    explicit CmdHandlerV1(uint devSeq);
    ~CmdHandlerV1() override;

    QByteArray handleB1Cmd(const QByteArray &cmd) override;

    bool handleF1Cmd(const QByteArray &cmd) override;

    QByteArray assembleA1Cmd(uchar type, const QByteArray &data) override;

    QByteArray assembleF1Cmd(const QString &dateTime, uchar type, uchar status, const QString &desc) override;

    QByteArray makeRequestKey(uchar seq, const QByteArray &cmd) override;

    uchar getB1Type(const QByteArray &cmd) override;

private:
    QString deviceLogTag() const;

private:
    uint m_devSeq = 0;
};
