#pragma once

#include <QObject>

class ST_CapVehInfo;
class ST_TradeInfo;
class Environment;
class BizHandler : public QObject
{
    Q_OBJECT
public:
    explicit BizHandler(QObject *parent = nullptr);
    ~BizHandler() override;

    // 返回当前是否上班
    bool isInShifted();

    void quitSystemRequest();
    void quitSystemQuery();

public slots:
    // 抓拍数据解析槽
    void onCaptureInfo(const ST_CapVehInfo &info);

private:
    ST_TradeInfo getTradeInfo(const QString &plate, bool useFirst = true);

private:
};
