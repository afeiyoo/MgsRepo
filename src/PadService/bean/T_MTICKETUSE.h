#pragma once

#include <QDateTime>
#include <QObject>

class T_MTicketUse : public QObject
{
    Q_OBJECT
public:
    explicit T_MTicketUse(QObject *parent = nullptr)
        : QObject{parent}
    {
        tbl_pk = "DataID";
        auto_inc = "DataID";
    }

    QString toString() const
    {
        QString result = QString("DataID %1, TradeNum %2, TradeID %3, TicketType %4, FaceType %5, TicketYear %6, "
                                 "StartNum %7, EndNum %8")
                             .arg(DataID)
                             .arg(TradeNum)
                             .arg(TradeID)
                             .arg(TicketType)
                             .arg(FaceType)
                             .arg(TicketYear)
                             .arg(StartNum)
                             .arg(EndNum);
        return result;
    }

public:
    QString tbl_pk;
    Q_PROPERTY(QString tbl_pk MEMBER tbl_pk)
    QString auto_inc;
    Q_PROPERTY(QString auto_inc MEMBER auto_inc)
    int DataID;
    Q_PROPERTY(int DataID MEMBER DataID)
    int TradeNum;
    Q_PROPERTY(int TradeNum MEMBER TradeNum)
    QString TradeID;
    Q_PROPERTY(QString TradeID MEMBER TradeID)
    int TicketType;
    Q_PROPERTY(int TicketType MEMBER TicketType)
    int FaceType;
    Q_PROPERTY(int FaceType MEMBER FaceType)
    int TicketYear;
    Q_PROPERTY(int TicketYear MEMBER TicketYear)
    int StartNum;
    Q_PROPERTY(int StartNum MEMBER StartNum)
    int EndNum;
    Q_PROPERTY(int EndNum MEMBER EndNum)
    int Amount;
    Q_PROPERTY(int Amount MEMBER Amount)
    int LaneID;
    Q_PROPERTY(int LaneID MEMBER LaneID)
    QDateTime ShiftDate;
    Q_PROPERTY(QDateTime ShiftDate MEMBER ShiftDate)
    int ShiftNum;
    Q_PROPERTY(int ShiftNum MEMBER ShiftNum)
    QString ShiftUser;
    Q_PROPERTY(QString ShiftUser MEMBER ShiftUser)
    int IsValid;
    Q_PROPERTY(int IsValid MEMBER IsValid)
    QString OperaterID;
    Q_PROPERTY(QString OperaterID MEMBER OperaterID)
    QDateTime OperateTime;
    Q_PROPERTY(QDateTime OperateTime MEMBER OperateTime)
    QString BillCode;
    Q_PROPERTY(QString BillCode MEMBER BillCode)
};
