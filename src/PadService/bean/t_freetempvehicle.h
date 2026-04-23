#pragma once

#include <QDateTime>
#include <QObject>

class T_FreeTempVehicle : public QObject
{
    Q_OBJECT
public:
    explicit T_FreeTempVehicle(QObject *parent = nullptr)
        : QObject{parent}
    {}

public:
    int TradeNum;
    Q_PROPERTY(int TradeNum MEMBER TradeNum)
    QString VehPlate;
    Q_PROPERTY(QString VehPlate MEMBER VehPlate)
    QString VehColor;
    Q_PROPERTY(QString VehColor MEMBER VehColor)
    int VehClass;
    Q_PROPERTY(int VehClass MEMBER VehClass)
    int Capacity;
    Q_PROPERTY(int Capacity MEMBER Capacity)
    QString Brands;
    Q_PROPERTY(QString Brands MEMBER Brands)
    QString EngineID;
    Q_PROPERTY(QString EngineID MEMBER EngineID)
    QString ChassisID;
    Q_PROPERTY(QString ChassisID MEMBER ChassisID)
    QDateTime EnableTime;
    Q_PROPERTY(QDateTime EnableTime MEMBER EnableTime)
    QDateTime DownTime;
    Q_PROPERTY(QDateTime DownTime MEMBER DownTime)
    int IsValid;
    Q_PROPERTY(int IsValid MEMBER IsValid)
    int FreeType;
    Q_PROPERTY(int FreeType MEMBER FreeType)
    QDateTime ManuDate;
    Q_PROPERTY(QDateTime ManuDate MEMBER ManuDate)
    QDateTime PlateDate;
    Q_PROPERTY(QDateTime PlateDate MEMBER PlateDate)
    QString VehOwner;
    Q_PROPERTY(QString VehOwner MEMBER VehOwner)
    QString OwnerAddr;
    Q_PROPERTY(QString OwnerAddr MEMBER OwnerAddr)
    QString ContactUser;
    Q_PROPERTY(QString ContactUser MEMBER ContactUser)
    QString ContactTel;
    Q_PROPERTY(QString ContactTel MEMBER ContactTel)
    QDateTime UpdateTime;
    Q_PROPERTY(QDateTime UpdateTime MEMBER UpdateTime)
    QString EnStations;
    Q_PROPERTY(QString EnStations MEMBER EnStations)
    QString ExStations;
    Q_PROPERTY(QString ExStations MEMBER ExStations)
    int DiscountType;
    Q_PROPERTY(int DiscountType MEMBER DiscountType)
    int VerifyFlag;
    Q_PROPERTY(int VerifyFlag MEMBER VerifyFlag)
};
