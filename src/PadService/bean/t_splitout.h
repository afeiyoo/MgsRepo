#pragma once

#include <QDateTime>
#include <QObject>

class T_SplitOut : public QObject
{
    Q_OBJECT
public:
    explicit T_SplitOut(QObject *parent = nullptr)
        : QObject{parent}
    {}
    ~T_SplitOut() override {}

public:
    QString TradeID;
    Q_PROPERTY(QString TradeID MEMBER TradeID)
    int ProvinceNum;
    Q_PROPERTY(int ProvinceNum MEMBER ProvinceNum)
    QString ProvinceName;
    Q_PROPERTY(QString ProvinceName MEMBER ProvinceName)
    int DataIndex;
    Q_PROPERTY(int DataIndex MEMBER DataIndex)
    QString ExOperatorID;
    Q_PROPERTY(QString ExOperatorID MEMBER ExOperatorID)
    int ExShiftId;
    Q_PROPERTY(int ExShiftId MEMBER ExShiftId)
    QDateTime ExShiftDate;
    Q_PROPERTY(QDateTime ExShiftDate MEMBER ExShiftDate)
    int ExLane;
    Q_PROPERTY(int ExLane MEMBER ExLane)
    int TollFee;
    Q_PROPERTY(int TollFee MEMBER TollFee)
    int TollIntervalsCount;
    Q_PROPERTY(int TollIntervalsCount MEMBER TollIntervalsCount)
    QString TollIntervalsGroup;
    Q_PROPERTY(QString TollIntervalsGroup MEMBER TollIntervalsGroup)
    QString TransTimeGroup;
    Q_PROPERTY(QString TransTimeGroup MEMBER TransTimeGroup)
    QString ChargePayGroup;
    Q_PROPERTY(QString ChargePayGroup MEMBER ChargePayGroup)
    QString ChargefeeGroup;
    Q_PROPERTY(QString ChargefeeGroup MEMBER ChargefeeGroup)
    QString Reserve;
    Q_PROPERTY(QString Reserve MEMBER Reserve)
};
