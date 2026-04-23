#pragma once

#include <QDateTime>
#include <QObject>

class T_LaneInputShift : public QObject
{
    Q_OBJECT
public:
    explicit T_LaneInputShift(QObject *parent = nullptr)
        : QObject{parent}
    {}
    ~T_LaneInputShift() override {}

    QString toString() const
    {
        QString result = QString("LaneID: %1, ShiftDate: %2, ShiftID: %3, Operator %4, NextOperator %5,"
                                 "Cash %6, DiscardFee %7, BackGoodCPCCards %8, BackPaperCards %9, CPCCards %10,"
                                 "SendPaperCards %11, NextNormalCards %12, NextCash %13, Remark %14, UpdateTime %15")
                             .arg(LaneID)
                             .arg(ShiftDate.toString("yyyy-MM-dd hh:mm:ss"))
                             .arg(ShiftID)
                             .arg(Operator)
                             .arg(NextOperator)
                             .arg(Cash)
                             .arg(DiscardFee)
                             .arg(BackGoodCPCCards)
                             .arg(BackPaperCards)
                             .arg(CPCCards)
                             .arg(SendPaperCards)
                             .arg(NextNormalCards)
                             .arg(NextCash)
                             .arg(Remark)
                             .arg(UpdateTime.toString("yyyy-MM-dd hh:mm:ss"));

        return result;
    }

public:
    QDateTime ShiftDate;
    Q_PROPERTY(QDateTime ShiftDate MEMBER ShiftDate)
    int ShiftID;
    Q_PROPERTY(int ShiftID MEMBER ShiftID)
    int LaneID;
    Q_PROPERTY(int LaneID MEMBER LaneID)
    QString Operator;
    Q_PROPERTY(QString Operator MEMBER Operator)
    QString NextOperator;
    Q_PROPERTY(QString NextOperator MEMBER NextOperator)
    int Cash;
    Q_PROPERTY(int Cash MEMBER Cash)
    int DiscardFee;
    Q_PROPERTY(int DiscardFee MEMBER DiscardFee)
    int BackGoodCPCCards;
    Q_PROPERTY(int BackGoodCPCCards MEMBER BackGoodCPCCards)
    int BackPaperCards;
    Q_PROPERTY(int BackPaperCards MEMBER BackPaperCards)
    int CPCCards;
    Q_PROPERTY(int CPCCards MEMBER CPCCards)
    int SendPaperCards;
    Q_PROPERTY(int SendPaperCards MEMBER SendPaperCards)
    int NextNormalCards;
    Q_PROPERTY(int NextNormalCards MEMBER NextNormalCards)
    int NextSpecialCards;
    Q_PROPERTY(int NextSpecialCards MEMBER NextSpecialCards)
    int NextCash;
    Q_PROPERTY(int NextCash MEMBER NextCash)
    QString Remark;
    Q_PROPERTY(QString Remark MEMBER Remark)
    QDateTime UpdateTime;
    Q_PROPERTY(QDateTime UpdateTime MEMBER UpdateTime)
};
