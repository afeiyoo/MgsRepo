#pragma once

#include <QDateTime>
#include <QObject>

class T_SpecialCards : public QObject
{
    Q_OBJECT
public:
    explicit T_SpecialCards(QObject *parent = nullptr)
        : QObject{parent}
    {}
    ~T_SpecialCards() override {}

    QString toString() const
    {
        QString result
            = QString("DataID %1, ShiftDate %2, ShiftID %3, LaneID %4, Operator %5, OperatorName %6, InputOperator %7, "
                      "InputOperatorName %8, CardID %9, SpecialType %10, DealDesc %11, Remark %12, UpdateTime %13")
                  .arg(DataID)
                  .arg(ShiftDate.toString("yyyy-MM-dd hh:mm:ss"))
                  .arg(ShiftID)
                  .arg(LaneID)
                  .arg(Operator)
                  .arg(OperatorName)
                  .arg(InputOperator)
                  .arg(InputOperatorName)
                  .arg(CardID)
                  .arg(SpecialType)
                  .arg(DealDesc)
                  .arg(Remark)
                  .arg(UpdateTime.toString("yyyy-MM-dd hh:mm:ss"));

        return result;
    }

public:
    QString DataID;
    Q_PROPERTY(QString DataID MEMBER DataID)
    QDateTime ShiftDate;
    Q_PROPERTY(QDateTime ShiftDate MEMBER ShiftDate)
    int ShiftID;
    Q_PROPERTY(int ShiftID MEMBER ShiftID)
    int LaneID;
    Q_PROPERTY(int LaneID MEMBER LaneID)
    QString Operator;
    Q_PROPERTY(QString Operator MEMBER Operator)
    QString OperatorName;
    Q_PROPERTY(QString OperatorName MEMBER OperatorName)
    QString InputOperator;
    Q_PROPERTY(QString InputOperator MEMBER InputOperator)
    QString InputOperatorName;
    Q_PROPERTY(QString InputOperatorName MEMBER InputOperatorName)
    QString CardID;
    Q_PROPERTY(QString CardID MEMBER CardID)
    int SpecialType;
    Q_PROPERTY(int SpecialType MEMBER SpecialType)
    QString DealDesc;
    Q_PROPERTY(QString DealDesc MEMBER DealDesc)
    QString Remark;
    Q_PROPERTY(QString Remark MEMBER Remark)
    QDateTime UpdateTime;
    Q_PROPERTY(QDateTime UpdateTime MEMBER UpdateTime)
};
