#pragma once

#include <QObject>

class ST_CapVehInfo;
class ST_TradeInfo;
class DevsHandler : public QObject
{
    Q_OBJECT
public:
    explicit DevsHandler(QObject *parent = nullptr);
    ~DevsHandler() override;

public slots:
    // 抓拍数据解析槽
    void onCaptureInfo(const ST_CapVehInfo &info);
    // 线圈数据解析槽
    void onVDInfo();

private:
    ST_TradeInfo getTradeInfo(const QString &plate, bool useFirst = true);
};
