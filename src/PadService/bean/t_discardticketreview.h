#pragma once

#include <QDateTime>
#include <QObject>

class T_DiscardTicketReview : public QObject
{
    Q_OBJECT
public:
    explicit T_DiscardTicketReview(QObject *parent = nullptr)
        : QObject{parent}
    {}
    ~T_DiscardTicketReview() override {}

public:
    QString StationID;
    Q_PROPERTY(QString StationID MEMBER StationID)
    QString TradeID;
    Q_PROPERTY(QString TradeID MEMBER TradeID)
    int Flag;
    Q_PROPERTY(int Flag MEMBER Flag)
    QString PicData;
    Q_PROPERTY(QString PicData MEMBER PicData)
    QDateTime OperateTime;
    Q_PROPERTY(QDateTime OperateTime MEMBER OperateTime)
};
