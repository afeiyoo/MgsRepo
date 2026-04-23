#pragma once

#include <QDateTime>
#include <QObject>

class T_AuditPayBack : public QObject
{
    Q_OBJECT
public:
    explicit T_AuditPayBack(QObject *parent = nullptr)
        : QObject{parent}
    {
        tbl_pk = "TradeNum";
    }

public:
    QString tbl_pk;
    Q_PROPERTY(QString tbl_pk MEMBER tbl_pk)
    QString auto_inc;
    Q_PROPERTY(QString auto_inc MEMBER auto_inc)
    int TradeNum;
    Q_PROPERTY(int TradeNum MEMBER TradeNum)
    QString StationID;
    Q_PROPERTY(QString StationID MEMBER StationID)
    QDateTime ShiftDate;
    Q_PROPERTY(QDateTime ShiftDate MEMBER ShiftDate)
    int ShiftID;
    Q_PROPERTY(int ShiftID MEMBER ShiftID)
    QString VehPlate;
    Q_PROPERTY(QString VehPlate MEMBER VehPlate)
    int VehPlateColor;
    Q_PROPERTY(int VehPlateColor MEMBER VehPlateColor)
    double PayFee;
    Q_PROPERTY(double PayFee MEMBER PayFee)
    int VehClass;
    Q_PROPERTY(int VehClass MEMBER VehClass)
    int DataType;
    Q_PROPERTY(int DataType MEMBER DataType)
    QString Remark;
    Q_PROPERTY(QString Remark MEMBER Remark)
    QString Operator;
    Q_PROPERTY(QString Operator MEMBER Operator)
    QString OperatorName;
    Q_PROPERTY(QString OperatorName MEMBER OperatorName)
    QDateTime OperateTime;
    Q_PROPERTY(QDateTime OperateTime MEMBER OperateTime)
    int Status;
    Q_PROPERTY(int Status MEMBER Status)
    QString PaybackUser;
    Q_PROPERTY(QString PaybackUser MEMBER PaybackUser)
    QString PaybackUserPhone;
    Q_PROPERTY(QString PaybackUserPhone MEMBER PaybackUserPhone)
    int TransactionType;
    Q_PROPERTY(int TransactionType MEMBER TransactionType)
    int PaybackWay;
    Q_PROPERTY(int PaybackWay MEMBER PaybackWay)
    QString EtcCardId;
    Q_PROPERTY(QString EtcCardId MEMBER EtcCardId)
    QString AuditId;
    Q_PROPERTY(QString AuditId MEMBER AuditId)
    int PayType;
    Q_PROPERTY(int PayType MEMBER PayType)
    int PayMeans;
    Q_PROPERTY(int PayMeans MEMBER PayMeans)
    QString PayCode;
    Q_PROPERTY(QString PayCode MEMBER PayCode)
    QString OrderNum;
    Q_PROPERTY(QString OrderNum MEMBER OrderNum)
    QDateTime ReqPayTime;
    Q_PROPERTY(QDateTime ReqPayTime MEMBER ReqPayTime)
    int SyncFlag;
    Q_PROPERTY(int SyncFlag MEMBER SyncFlag)
    int IsPreBlack;
    Q_PROPERTY(int IsPreBlack MEMBER IsPreBlack)
    QString ExStation;
    Q_PROPERTY(QString ExStation MEMBER ExStation)
    QString EnStation;
    Q_PROPERTY(QString EnStation MEMBER EnStation)
    QString PassID;
    Q_PROPERTY(QString PassID MEMBER PassID)
    QString EscapeType;
    Q_PROPERTY(QString EscapeType MEMBER EscapeType)
    QString EscapeTypeDesc;
    Q_PROPERTY(QString EscapeTypeDesc MEMBER EscapeTypeDesc)
};
