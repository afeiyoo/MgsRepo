#pragma once

#include <QDateTime>
#include <QObject>

class T_CollectLog : public QObject
{
    Q_OBJECT
public:
    explicit T_CollectLog(QObject *parent = nullptr)
        : QObject{parent}
    {
        tbl_pk = "TradeID,LogIdx";
        LogIdx = 0;
        LogTime = QDateTime::currentDateTime();
    }

public:
    QString tbl_pk;
    Q_PROPERTY(QString tbl_pk MEMBER tbl_pk)
    QString auto_inc;
    Q_PROPERTY(QString auto_inc MEMBER auto_inc)
    QString TradeID;
    Q_PROPERTY(QString TradeID MEMBER TradeID)
    int LogIdx;
    Q_PROPERTY(int LogIdx MEMBER LogIdx)
    int LaneID;
    Q_PROPERTY(int LaneID MEMBER LaneID)
    QDateTime ShiftDate;
    Q_PROPERTY(QDateTime ShiftDate MEMBER ShiftDate)
    int ShiftNum;
    Q_PROPERTY(int ShiftNum MEMBER ShiftNum)
    QString ShiftUser;
    Q_PROPERTY(QString ShiftUser MEMBER ShiftUser)
    QString LogContent;
    Q_PROPERTY(QString LogContent MEMBER LogContent)
    QDateTime LogTime;
    Q_PROPERTY(QDateTime LogTime MEMBER LogTime)
};
