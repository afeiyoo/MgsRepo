#pragma once

#include <QDateTime>
#include <QObject>

class T_FF_CapRecord : public QObject
{
    Q_OBJECT
public:
    explicit T_FF_CapRecord(QObject *parent = nullptr)
        : QObject{parent}
    {
        tbl_pk = "CaptureID";
        FlagType = 0;
        FlagIndex = 1;
        ShiftID = 1;
        CapType = 0;
        Channel = 0;
    }

public:
    QString tbl_pk;
    Q_PROPERTY(QString tbl_pk MEMBER tbl_pk)
    QString auto_inc;
    Q_PROPERTY(QString auto_inc MEMBER auto_inc)
    QString CaptureID;
    Q_PROPERTY(QString CaptureID MEMBER CaptureID)
    int FlagType;
    Q_PROPERTY(int FlagType MEMBER FlagType)
    QString FlagID;
    Q_PROPERTY(QString FlagID MEMBER FlagID)
    QString FlagName;
    Q_PROPERTY(QString FlagName MEMBER FlagName)
    int FlagIndex;
    Q_PROPERTY(int FlagIndex MEMBER FlagIndex)
    QDateTime CapTime;
    Q_PROPERTY(QDateTime CapTime MEMBER CapTime)
    QDateTime Shiftdate;
    Q_PROPERTY(QDateTime Shiftdate MEMBER Shiftdate)
    int ShiftID;
    Q_PROPERTY(int ShiftID MEMBER ShiftID)
    int CapType;
    Q_PROPERTY(int CapType MEMBER CapType)
    QString CapDevice;
    Q_PROPERTY(QString CapDevice MEMBER CapDevice)
    int Channel;
    Q_PROPERTY(int Channel MEMBER Channel)
    QString VehPlate;
    Q_PROPERTY(QString VehPlate MEMBER VehPlate)
    QString PictureID;
    Q_PROPERTY(QString PictureID MEMBER PictureID)
    QString TradeID;
    Q_PROPERTY(QString TradeID MEMBER TradeID)
    int VehClass;
    Q_PROPERTY(int VehClass MEMBER VehClass)
    QString VehBrand;
    Q_PROPERTY(QString VehBrand MEMBER VehBrand)
    QString Reserve1;
    Q_PROPERTY(QString Reserve1 MEMBER Reserve1)
    QString Reserve2;
    Q_PROPERTY(QString Reserve2 MEMBER Reserve2)
};
