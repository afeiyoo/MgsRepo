#pragma once

#include <QDateTime>
#include <QObject>

class T_EtcOut : public QObject
{
    Q_OBJECT
public:
    explicit T_EtcOut(QObject *parent = nullptr)
        : QObject{parent}
    {}
    ~T_EtcOut() override {}

public:
    QString TradeID;
    Q_PROPERTY(QString TradeID MEMBER TradeID)
    int SeqNum;
    Q_PROPERTY(int SeqNum MEMBER SeqNum)
    int TollRateVer;
    Q_PROPERTY(int TollRateVer MEMBER TollRateVer)
    int CardNetID;
    Q_PROPERTY(int CardNetID MEMBER CardNetID)
    int CardType;
    Q_PROPERTY(int CardType MEMBER CardType)
    int CardBizType;
    Q_PROPERTY(int CardBizType MEMBER CardBizType)
    QString CardID;
    Q_PROPERTY(QString CardID MEMBER CardID)
    int EnNetID;
    Q_PROPERTY(int EnNetID MEMBER EnNetID)
    QString EnStation;
    Q_PROPERTY(QString EnStation MEMBER EnStation)
    QString EnStationName;
    Q_PROPERTY(QString EnStationName MEMBER EnStationName)
    QDateTime EnTime;
    Q_PROPERTY(QDateTime EnTime MEMBER EnTime)
    QString EnOperatorID;
    Q_PROPERTY(QString EnOperatorID MEMBER EnOperatorID)
    int EnVehClass;
    Q_PROPERTY(int EnVehClass MEMBER EnVehClass)
    QString EnVehPlate;
    Q_PROPERTY(QString EnVehPlate MEMBER EnVehPlate)
    int ExNetID;
    Q_PROPERTY(int ExNetID MEMBER ExNetID)
    QString ExStation;
    Q_PROPERTY(QString ExStation MEMBER ExStation)
    QString ExStationName;
    Q_PROPERTY(QString ExStationName MEMBER ExStationName)
    int ExLane;
    Q_PROPERTY(int ExLane MEMBER ExLane)
    QDateTime ExTime;
    Q_PROPERTY(QDateTime ExTime MEMBER ExTime)
    QString ExOperatorID;
    Q_PROPERTY(QString ExOperatorID MEMBER ExOperatorID)
    QString ExOperator;
    Q_PROPERTY(QString ExOperator MEMBER ExOperator)
    int ExShiftID;
    Q_PROPERTY(int ExShiftID MEMBER ExShiftID)
    QDateTime ExShiftDate;
    Q_PROPERTY(QDateTime ExShiftDate MEMBER ExShiftDate)
    int ExVehClass;
    Q_PROPERTY(int ExVehClass MEMBER ExVehClass)
    int ExVehStatus;
    Q_PROPERTY(int ExVehStatus MEMBER ExVehStatus)
    QString ExVehPlate;
    Q_PROPERTY(QString ExVehPlate MEMBER ExVehPlate)
    QString OBUPlate;
    Q_PROPERTY(QString OBUPlate MEMBER OBUPlate)
    QString CardPlate;
    Q_PROPERTY(QString CardPlate MEMBER CardPlate)
    int VehSpeed;
    Q_PROPERTY(int VehSpeed MEMBER VehSpeed)
    int AxisGroups;
    Q_PROPERTY(int AxisGroups MEMBER AxisGroups)
    int Axises;
    Q_PROPERTY(int Axises MEMBER Axises)
    int OrigAxisType;
    Q_PROPERTY(int OrigAxisType MEMBER OrigAxisType)
    int AxisType;
    Q_PROPERTY(int AxisType MEMBER AxisType)
    QString AxisWeight;
    Q_PROPERTY(QString AxisWeight MEMBER AxisWeight)
    int OrigWeight;
    Q_PROPERTY(int OrigWeight MEMBER OrigWeight)
    int TotalWeight;
    Q_PROPERTY(int TotalWeight MEMBER TotalWeight)
    int TollWeight;
    Q_PROPERTY(int TollWeight MEMBER TollWeight)
    int AxisLimit;
    Q_PROPERTY(int AxisLimit MEMBER AxisLimit)
    double ExceedRate;
    Q_PROPERTY(double ExceedRate MEMBER ExceedRate)
    int WeightClass;
    Q_PROPERTY(int WeightClass MEMBER WeightClass)
    int TollType;
    Q_PROPERTY(int TollType MEMBER TollType)
    double ShouldPay;
    Q_PROPERTY(double ShouldPay MEMBER ShouldPay)
    int DiscountType;
    Q_PROPERTY(int DiscountType MEMBER DiscountType)
    double Discount;
    Q_PROPERTY(double Discount MEMBER Discount)
    double FactPay;
    Q_PROPERTY(double FactPay MEMBER FactPay)
    int MerRebateType;
    Q_PROPERTY(int MerRebateType MEMBER MerRebateType)
    double RebateRatio;
    Q_PROPERTY(double RebateRatio MEMBER RebateRatio)
    double MerShouldPay;
    Q_PROPERTY(double MerShouldPay MEMBER MerShouldPay)
    double RebatedPay;
    Q_PROPERTY(double RebatedPay MEMBER RebatedPay)
    double SpecialPay;
    Q_PROPERTY(double SpecialPay MEMBER SpecialPay)
    int TicketNum;
    Q_PROPERTY(int TicketNum MEMBER TicketNum)
    int PayType;
    Q_PROPERTY(int PayType MEMBER PayType)
    int DealStatus;
    Q_PROPERTY(int DealStatus MEMBER DealStatus)
    int DeviceStatus;
    Q_PROPERTY(int DeviceStatus MEMBER DeviceStatus)
    int RecordType;
    Q_PROPERTY(int RecordType MEMBER RecordType)
    int ChangeType;
    Q_PROPERTY(int ChangeType MEMBER ChangeType)
    double ChangedFee;
    Q_PROPERTY(double ChangedFee MEMBER ChangedFee)
    int PeccType;
    Q_PROPERTY(int PeccType MEMBER PeccType)
    int ChkPoint;
    Q_PROPERTY(int ChkPoint MEMBER ChkPoint)
    int SpendTime;
    Q_PROPERTY(int SpendTime MEMBER SpendTime)
    int IsValid;
    Q_PROPERTY(int IsValid MEMBER IsValid)
    int ScrapTickets;
    Q_PROPERTY(int ScrapTickets MEMBER ScrapTickets)
    int OBUID;
    Q_PROPERTY(int OBUID MEMBER OBUID)
    QString TermCode;
    Q_PROPERTY(QString TermCode MEMBER TermCode)
    int CardTradeNo;
    Q_PROPERTY(int CardTradeNo MEMBER CardTradeNo)
    int TermTradeNo;
    Q_PROPERTY(int TermTradeNo MEMBER TermTradeNo)
    uint RestMoney;
    Q_PROPERTY(uint RestMoney MEMBER RestMoney)
    double TAC;
    Q_PROPERTY(double TAC MEMBER TAC)
    QString OBUSerialNum;
    Q_PROPERTY(QString OBUSerialNum MEMBER OBUSerialNum)
    int OBUStatus;
    Q_PROPERTY(int OBUStatus MEMBER OBUStatus)
    int ProvinceID;
    Q_PROPERTY(int ProvinceID MEMBER ProvinceID)
    int Verifycode;
    Q_PROPERTY(int Verifycode MEMBER Verifycode)
    double TSVehicle;
    Q_PROPERTY(double TSVehicle MEMBER TSVehicle)
    QString Reserve;
    Q_PROPERTY(QString Reserve MEMBER Reserve)
    int DataMonth;
    Q_PROPERTY(int DataMonth MEMBER DataMonth)
    int EnLane;
    Q_PROPERTY(int EnLane MEMBER EnLane)
    uint PostBalance;
    Q_PROPERTY(uint PostBalance MEMBER PostBalance)
    int Algorithm;
    Q_PROPERTY(int Algorithm MEMBER Algorithm)
    QString CPCCardID;
    Q_PROPERTY(QString CPCCardID MEMBER CPCCardID)
    int CPCElec;
    Q_PROPERTY(int CPCElec MEMBER CPCElec)
    double OPFactPay;
    Q_PROPERTY(double OPFactPay MEMBER OPFactPay)
    int ETCCardVer;
    Q_PROPERTY(int ETCCardVer MEMBER ETCCardVer)
    int OBUContractVer;
    Q_PROPERTY(int OBUContractVer MEMBER OBUContractVer)
    QString Reserve2;
    Q_PROPERTY(QString Reserve2 MEMBER Reserve2)
    QString AppVersion;
    Q_PROPERTY(QString AppVersion MEMBER AppVersion)
    int RecordType2;
    Q_PROPERTY(int RecordType2 MEMBER RecordType2)
    int TollSupport;
    Q_PROPERTY(int TollSupport MEMBER TollSupport)
    int EnAxisCount;
    Q_PROPERTY(int EnAxisCount MEMBER EnAxisCount)
    int EnTotalWeight;
    Q_PROPERTY(int EnTotalWeight MEMBER EnTotalWeight)
    int UserType;
    Q_PROPERTY(int UserType MEMBER UserType)
    int OBUTotalTradeSuccNum;
    Q_PROPERTY(int OBUTotalTradeSuccNum MEMBER OBUTotalTradeSuccNum)
    int ProvinceNum;
    Q_PROPERTY(int ProvinceNum MEMBER ProvinceNum)
    int NoCardTimes;
    Q_PROPERTY(int NoCardTimes MEMBER NoCardTimes)
    int OBUpayFeeSum;
    Q_PROPERTY(int OBUpayFeeSum MEMBER OBUpayFeeSum)
    int OBUdiscountFeeSum;
    Q_PROPERTY(int OBUdiscountFeeSum MEMBER OBUdiscountFeeSum)
    int OBUMileage;
    Q_PROPERTY(int OBUMileage MEMBER OBUMileage)
    QString ProvEnHex;
    Q_PROPERTY(QString ProvEnHex MEMBER ProvEnHex)
    int OBUProvfeeSum;
    Q_PROPERTY(int OBUProvfeeSum MEMBER OBUProvfeeSum)
    int OBUProvfactSum;
    Q_PROPERTY(int OBUProvfactSum MEMBER OBUProvfactSum)
    int OBUProvTradeSuccNum;
    Q_PROPERTY(int OBUProvTradeSuccNum MEMBER OBUProvTradeSuccNum)
    QString FFOBUCardNo;
    Q_PROPERTY(QString FFOBUCardNo MEMBER FFOBUCardNo)
    int CardfeeSum;
    Q_PROPERTY(int CardfeeSum MEMBER CardfeeSum)
    QString PassID;
    Q_PROPERTY(QString PassID MEMBER PassID)
    int ShortFee;
    Q_PROPERTY(int ShortFee MEMBER ShortFee)
    int ShortFeeMileage;
    Q_PROPERTY(int ShortFeeMileage MEMBER ShortFeeMileage)
    double FeeRate;
    Q_PROPERTY(double FeeRate MEMBER FeeRate)
    int ExitFeeType;
    Q_PROPERTY(int ExitFeeType MEMBER ExitFeeType)
    int TransPayType;
    Q_PROPERTY(int TransPayType MEMBER TransPayType)
    QString SplitProvince;
    Q_PROPERTY(QString SplitProvince MEMBER SplitProvince)
    QString SpecialType;
    Q_PROPERTY(QString SpecialType MEMBER SpecialType)
    QString Merchant;
    Q_PROPERTY(QString Merchant MEMBER Merchant)
    QString CNLaneID;
    Q_PROPERTY(QString CNLaneID MEMBER CNLaneID)
    QString EnRoadNet;
    Q_PROPERTY(QString EnRoadNet MEMBER EnRoadNet)
    int OpenFlag;
    Q_PROPERTY(int OpenFlag MEMBER OpenFlag)
    int LocalFeeMileage;
    Q_PROPERTY(int LocalFeeMileage MEMBER LocalFeeMileage)
    int OBUVehClass;
    Q_PROPERTY(int OBUVehClass MEMBER OBUVehClass)
};
