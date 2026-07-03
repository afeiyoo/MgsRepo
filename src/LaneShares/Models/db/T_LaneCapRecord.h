#pragma once

#include <QDateTime>
#include <QObject>

class T_LaneCapRecord : public QObject
{
    Q_OBJECT
public:
    explicit T_LaneCapRecord(QObject *parent = nullptr)
        : QObject{parent}
    {
        tbl_pk = "StationID,LaneID,DataTime";
        VehClass = 0;
        VehSpeed = 0;
        CreateTime = QDateTime::currentDateTime();
    }

public:
    QString tbl_pk;
    Q_PROPERTY(QString tbl_pk MEMBER tbl_pk)
    QString auto_inc;
    Q_PROPERTY(QString auto_inc MEMBER auto_inc)
    QString StationID;
    Q_PROPERTY(QString StationID MEMBER StationID)
    int LaneID;
    Q_PROPERTY(int LaneID MEMBER LaneID)
    int LaneType;
    Q_PROPERTY(int LaneType MEMBER LaneType)
    QDateTime ShiftDate;
    Q_PROPERTY(QDateTime ShiftDate MEMBER ShiftDate)
    int ShiftID;
    Q_PROPERTY(int ShiftID MEMBER ShiftID)
    QDateTime DataTime;
    Q_PROPERTY(QDateTime DataTime MEMBER DataTime)
    QString TradeID;
    Q_PROPERTY(QString TradeID MEMBER TradeID)
    QString VehPlate;
    Q_PROPERTY(QString VehPlate MEMBER VehPlate)
    QString VehColor;
    Q_PROPERTY(QString VehColor MEMBER VehColor)
    int VehClass;
    Q_PROPERTY(int VehClass MEMBER VehClass)
    int VehSpeed;
    Q_PROPERTY(int VehSpeed MEMBER VehSpeed)
    QString VehFeatureCode;
    Q_PROPERTY(QString VehFeatureCode MEMBER VehFeatureCode)
    QString FaceFeatureCode;
    Q_PROPERTY(QString FaceFeatureCode MEMBER FaceFeatureCode)
    QDateTime CreateTime;
    Q_PROPERTY(QDateTime CreateTime MEMBER CreateTime)
    QString Reserve;
    Q_PROPERTY(QString Reserve MEMBER Reserve)
};
