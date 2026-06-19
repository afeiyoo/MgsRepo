#pragma once

#include <QImage>
#include <QList>
#include <QString>
#include <QStringList>

// 界面称重信息结构体
struct ST_WeightInfoItem
{
    int index = 0; // 队列号
    QString plate;
    uint axisType = 0;
    uint axisNum = 0;
    qreal tollWeight = 0.0;
    uint status = 0; // 0-等待中 1-已交易
    bool isManual = false;
    qreal limitWeight = 0.0;
    bool allowPass = false; // 是否允许过车
    qreal exceedRate = 0.0;
};

class IPageController
{
public:
    // 设备状态图标
    enum EM_DeviceIcon {
        CAPTURE = 1,
        CELLING_LAMP,
        FIRST_COIL,
        SECOND_COIL,
        THIRD_COIL,
        FOURTH_COIL,
        CAP_COIL,
        RAILLING_COIL,
        PASSING_LAMP,
        RAILING_MACHINE,
        RSU,
        WEIGHT
    };

    // 设备状态
    enum EM_DeviceIconStatus {
        FAIL = 1,     // 设备异常
        NORMAL_FALSE, // 设备正常，但设备状态false
        NORMAL_TRUE   // 设备正常，且设备状态true
    };

    // 界面显示日志等级
    enum EM_LogLevel { DEBUG = 1, INFO, WARN, ERROR };

public:
    IPageController() = default;
    virtual ~IPageController() = default;

    // 顶部信息栏API
    virtual void setStationInfo(const QString &stationInfo) = 0;
    virtual void setUserInfo(const QString &userInfo) = 0;
    virtual void setLaneID(uint laneID) = 0;
    virtual void setShiftInfo(const QString &shiftInfo) = 0;
    virtual void setModeText(const QString &mode) = 0;

    // 底部状态栏API
    virtual void setFullBlackVer(const QString &ver) = 0;
    virtual void setPartBlackVer(const QString &ver) = 0;
    virtual void setVirtualGantryInfo(const QString &info) = 0;
    virtual void setAppVer(const QString &ver) = 0;
    virtual void setFeeRateVer(const QString &ver) = 0;

    // 抓拍显示区域API
    virtual void setCapImage(const QImage &img) = 0;

    // 滚动提示区域API
    virtual void setScrollTip(const QString &tip) = 0;

    // 日志显示区域API
    virtual void logAppend(EM_LogLevel logLevel, const QString &log) = 0;

    // 当前车辆与卡内信息显示区域API
    virtual void setPlate(const QString &plate) = 0;
    virtual void setVehClass(const QString &vehClass) = 0;
    virtual void setVehStatus(const QString &vehStatus) = 0;
    virtual void setSituation(const QString &situation) = 0;

    // 交易提示区域API
    virtual void setTradeHint(const QString &tradeHint, bool isWarn = false) = 0;
    virtual void setObuHint(const QString &obuHint, bool isWarn = false) = 0;

    // 近期交易记录查看区域API
    virtual void appendTradeItem(const QStringList &trade) = 0;
    virtual void clearTradeItems() = 0;

    // 设备图标显示区域API
    virtual void setDeviceList(const QList<uint> &devList) = 0;
    virtual void updateDeviceStatus(EM_DeviceIcon dev, EM_DeviceIconStatus status) = 0;

    // 窗口API
    virtual void showAuthDialog(const QString &id, const QString &name) = 0;
    virtual void showInfoDialog(const QString &title, const QStringList &strs, bool switchLine = true) = 0;

    virtual void setApi(int newApi) = 0;
};

// ETC 界面控制接口
class IEtcPageController : public IPageController
{
public:
    IEtcPageController() = default;
    ~IEtcPageController() override = default;

    // 当前车辆与卡内信息显示区域API
    virtual void setCardType(const QString &cardType) = 0;
    virtual void setProvince(const QString &province) = 0;
    virtual void setBalance(const QString &balance) = 0;
    virtual void setTradeTime(const QString &time) = 0;
    virtual void setEnStationName(const QString &enStationName) = 0;
    virtual void setToll(const QString &toll) = 0;

    // 工班信息显示区域API
    virtual void setTotalVehCnt(int cnt) = 0;
    virtual void setNormalVehCnt(int cnt) = 0;
    virtual void setFreeVehCnt(int cnt) = 0;
    virtual void setTotalToll(qreal fee) = 0;
    virtual void setCreditCardCnt(int cnt) = 0;
    virtual void setPrePayCardCnt(int cnt) = 0;
    virtual void setHolidayFreeVehCnt(int cnt) = 0;
    virtual void setPeccanyVehCnt(int cnt) = 0;
    virtual void setLastShiftTotalVehCnt(int cnt) = 0;
};

// 混合入口界面控制接口
class IMtcInPageController : public IPageController
{
public:
    IMtcInPageController() = default;
    ~IMtcInPageController() override = default;

    // 当前车辆与卡内信息显示区域API
    virtual void setCardType(const QString &cardType) = 0;
    virtual void setCardNum(const QString &cardNum) = 0;
    virtual void setBalance(const QString &balance) = 0;
    virtual void setEnTime(const QString &enTime) = 0;
    virtual void setEnStationName(const QString &enStationName) = 0;
    virtual void setCardStatus(const QString &cardStatus) = 0;

    // 工班信息显示区域API
    virtual void setTotalVehCnt(uint cnt) = 0;
    virtual void setTotalCardCnt(uint cnt) = 0;
    virtual void setNoFlagCardCnt(uint cnt) = 0;
    virtual void setPeccanyVehCnt(uint cnt) = 0;
    virtual void setFleetVehCnt(uint cnt) = 0;
    virtual void setCpcCardCnt(uint cnt) = 0;
    virtual void setEtcCardCnt(uint cnt) = 0;
    virtual void setBadCardCnt(uint cnt) = 0;
    virtual void setPaperCardCnt(uint cnt) = 0;
    virtual void setHolidayFreeVehCnt(uint cnt) = 0;
    virtual void setRestTickCnt(uint cnt) = 0;
    virtual void setStartTicketNum(uint num) = 0;
    virtual void setEndTicketNum(uint num) = 0;
    virtual void setCurTicketNum(uint num) = 0;

    // 称重信息显示区域API
    virtual void setCurWeightInfo(const QString &curWeightInfo, bool isWarn = false) = 0;
    virtual void setCurWeightStatus(uint status) = 0;
    virtual void setCurWeightInfoCount(uint curWeightInfoCount) = 0;
    virtual void appendWeightInfoItem(const ST_WeightInfoItem &item) = 0;
    virtual void removeWeightInfoItem(uint index) = 0;
    virtual void refreshWeightInfoItem(uint index, const ST_WeightInfoItem &item) = 0;
    virtual void clearWeightInfoItem() = 0;
    virtual void setWeightLow(bool isLow) = 0;

    // 卡机状态显示区域API
    virtual void setRobotStatus1(bool isOk, ushort restCard) = 0;
    virtual void setRobotStatus2(bool isOk, ushort restCard) = 0;
    virtual void setRobotStatus3(bool isOk, ushort restCard) = 0;
    virtual void setRobotStatus4(bool isOk, ushort restCard) = 0;
};

// 混合出口界面控制接口
class IMtcOutPageController : public IPageController
{
public:
    // 支付类型
    enum EM_PayMethod { ALI, WECHAT, THRIDPAY, ELECTRIC, CASH };

public:
    IMtcOutPageController() = default;
    ~IMtcOutPageController() override = default;

    // 抓拍显示区域API
    virtual void setPrevImage(const QImage &img) = 0;

    // 工班信息显示区域API
    virtual void setStartTicketNum(int num) = 0;
    virtual void setCurTicketNum(int num) = 0;
    virtual void setNormalTicketCnt(int cnt) = 0;
    virtual void setScrapTicketCnt(int cnt) = 0;
    virtual void setRestTicketCnt(int cnt) = 0;
    virtual void setTotalVehCnt(int cnt) = 0;
    virtual void setRecycleCardCnt(int cnt) = 0;
    virtual void setEtcCardCnt(int cnt) = 0;
    virtual void setFleetVehCnt(int cnt) = 0;
    virtual void setThirdPayCnt(int cnt) = 0;
    virtual void setUpStartTicketNum(int num) = 0;
    virtual void setUpCurTicketNum(int num) = 0;
    virtual void setUpRestTicketCnt(int cnt) = 0;
    virtual void setUpRecycleCardCnt(int cnt) = 0;
    virtual void setUpScrapTicketCnt(int cnt) = 0;
    virtual void setUpWriteErrCnt(int cnt) = 0;
    virtual void setDownStartTicketNum(int num) = 0;
    virtual void setDownCurTicketNum(int num) = 0;
    virtual void setDownRestTicketCnt(int cnt) = 0;
    virtual void setDownRecycleCardCnt(int cnt) = 0;
    virtual void setDownScrapTicketCnt(int cnt) = 0;
    virtual void setDownWriteErrCnt(int cnt) = 0;

    // 当前车辆与卡内信息显示区域API
    virtual void setCardType(const QString &cardType) = 0;
    virtual void setCardNum(const QString &cardNum) = 0;
    virtual void setEnTime(const QString &enTime) = 0;
    virtual void setEnStationName(const QString &enStationName) = 0;
    virtual void setWeightInfo(const QString &info) = 0;
    virtual void setEnPlate(const QString &plate) = 0;
    virtual void setLabel1(const QString &info) = 0;
    virtual void setSplitProvincesInfo(const QString &info) = 0;

    // 交易提示区域API
    virtual void appendHintButton(const QString &hint, EM_PayMethod method) = 0;

    // 称重信息显示区域API
    virtual void setCurWeightInfo(const QString &curWeightInfo, bool isWarn = false) = 0;
    virtual void setCurWeightStatus(uint status) = 0;
    virtual void setCurWeightInfoCount(uint curWeightInfoCount) = 0;
    virtual void appendWeightInfoItem(const ST_WeightInfoItem &item) = 0;
    virtual void removeWeightInfoItem(uint index) = 0;
    virtual void refreshWeightInfoItem(uint index, const ST_WeightInfoItem &item) = 0;
    virtual void clearWeightInfoItem() = 0;
    virtual void setWeightLow(bool isLow) = 0;
};
