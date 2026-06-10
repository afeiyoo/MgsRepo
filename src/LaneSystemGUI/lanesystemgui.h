#pragma once

#include <QImage>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

#include "lanesystemgui_global.h"

class QApplication;
class QWidget;
class BasePage;
class EtcPage;
class MtcInPage;
class MtcOutPage;
class MainWindow;

class MtcInPageController;
class MtcOutPageController;
class EtcPageController;
class LANESYSTEMGUI_EXPORT LaneSystemGUI : public QObject
{
    Q_OBJECT
public:
    // 页面类型
    enum EM_PageType {
        Etc,
        MtcIn,
        MtcOut,
    };

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

    explicit LaneSystemGUI(QObject *parent = nullptr);
    ~LaneSystemGUI() override;

    // 前端初始化
    void initFront(QApplication &app);

    // 混合入口界面操作句柄获取
    MtcInPageController *createMtcInWindow(bool isMaxShow, QObject *parent = nullptr);

    // 混合出口界面操作句柄获取
    MtcOutPageController *createMtcOutWindow(bool isMaxShow, bool isSptShow, QObject *parent = nullptr);

    // ETC界面操作句柄获取
    EtcPageController *createEtcWindow(bool isMaxShow, QObject *parent = nullptr);

private:
    MainWindow *m_mainWindow = nullptr;
};

class LANESYSTEMGUI_EXPORT PageController : public QObject
{
    Q_OBJECT
public:
    explicit PageController(BasePage *page, QObject *parent = nullptr);
    ~PageController() override;

    void setStationInfo(const QString &stationInfo);
    void setUserInfo(const QString &userInfo);
    void setLaneID(uint laneID);
    void setShiftInfo(const QString &shiftInfo);
    void setModeText(const QString &mode);
    void setFullBlackVer(const QString &ver);
    void setPartBlackVer(const QString &ver);
    void setVirtualGantryInfo(const QString &info);
    void setAppVer(const QString &ver);
    void setFeeRateVer(const QString &ver);
    void setCapImage(const QImage &img);
    void setScrollTip(const QString &tip);
    void logAppend(LaneSystemGUI::EM_LogLevel logLevel, const QString &log);
    void setPlate(const QString &plate);
    void setVehClass(const QString &vehClass);
    void setVehStatus(const QString &vehStatus);
    void setSituation(const QString &situation);
    void setTradeHint(const QString &tradeHint, const QString &color = "#000000");
    void setObuHint(const QString &obuHint, const QString &color = "#0C4E94");
    void setCurWeightInfo(const QString &curWeightInfo, const QString &color = "#0C4E94");
    void setCurWeightStatus(uint status);
    void setCurWeightInfoCount(uint curWeightInfoCount);
    void appendWeightInfoItem(const ST_WeightInfoItem &item);
    void removeWeightInfoItem(uint index);
    void refreshWeightInfoItem(uint index, const ST_WeightInfoItem &item);
    void clearWeightInfoItem();
    void setWeightLow(bool isLow);
    void appendTradeItem(const QStringList &trade);
    void clearTradeItems();
    void setDeviceList(const QList<uint> &devList);
    void updateDeviceStatus(LaneSystemGUI::EM_DeviceIcon dev, uint status);

    void showAuthDialog(const QString &id, const QString &name);

protected:
    BasePage *page() const;

private:
    BasePage *m_page = nullptr;
};

class LANESYSTEMGUI_EXPORT EtcPageController : public PageController
{
    Q_OBJECT
public:
    explicit EtcPageController(EtcPage *page, QObject *parent = nullptr);

    void setTotalVehCnt(int cnt);
    void setNormalVehCnt(int cnt);
    void setFreeVehCnt(int cnt);
    void setTotalToll(qreal fee);
    void setCreditCardCnt(int cnt);
    void setPrePayCardCnt(int cnt);
    void setHolidayFreeVehCnt(int cnt);
    void setPeccanyVehCnt(int cnt);
    void setLastShiftTotalVehCnt(int cnt);
    void setCardType(const QString &cardType);
    void setProvince(const QString &province);
    void setBalance(const QString &balance);
    void setTradeTime(const QString &time);
    void setEnStationName(const QString &enStationName);
    void setToll(const QString &toll);
};

class LANESYSTEMGUI_EXPORT MtcInPageController : public PageController
{
    Q_OBJECT
public:
    explicit MtcInPageController(MtcInPage *page, QObject *parent = nullptr);

    void setTotalVehCnt(uint cnt);
    void setTotalCardCnt(uint cnt);
    void setNoFlagCardCnt(uint cnt);
    void setPeccanyVehCnt(uint cnt);
    void setFleetVehCnt(uint cnt);
    void setCpcCardCnt(uint cnt);
    void setEtcCardCnt(uint cnt);
    void setBadCardCnt(uint cnt);
    void setPaperCardCnt(uint cnt);
    void setHolidayFreeVehCnt(uint cnt);
    void setRestTickCnt(uint cnt);
    void setStartTicketNum(uint num);
    void setEndTicketNum(uint num);
    void setCurTicketNum(uint num);
    void setCardType(const QString &cardType);
    void setCardNum(const QString &cardNum);
    void setBalance(const QString &balance);
    void setEnTime(const QString &enTime);
    void setEnStationName(const QString &enStationName);
    void setCardStatus(const QString &cardStatus);
    void setRobotStatus1(bool isOk, ushort restCard);
    void setRobotStatus2(bool isOk, ushort restCard);
    void setRobotStatus3(bool isOk, ushort restCard);
    void setRobotStatus4(bool isOk, ushort restCard);
};

class LANESYSTEMGUI_EXPORT MtcOutPageController : public PageController
{
    Q_OBJECT
public:
    explicit MtcOutPageController(MtcOutPage *page, QObject *parent = nullptr);

    void setPrevImage(const QImage &img);
    void enableSptShiftInfoShow(bool isSptShiftInfo);
    void setStartTicketNum(int num);
    void setCurTicketNum(int num);
    void setNormalTicketCnt(int cnt);
    void setScrapTicketCnt(int cnt);
    void setRestTicketCnt(int cnt);
    void setTotalVehCnt(int cnt);
    void setRecycleCardCnt(int cnt);
    void setEtcCardCnt(int cnt);
    void setFleetVehCnt(int cnt);
    void setThirdPayCnt(int cnt);
    void setUpStartTicketNum(int num);
    void setUpCurTicketNum(int num);
    void setUpRestTicketCnt(int cnt);
    void setUpRecycleCardCnt(int cnt);
    void setUpScrapTicketCnt(int cnt);
    void setUpWriteErrCnt(int cnt);
    void setDownStartTicketNum(int num);
    void setDownCurTicketNum(int num);
    void setDownRestTicketCnt(int cnt);
    void setDownRecycleCardCnt(int cnt);
    void setDownScrapTicketCnt(int cnt);
    void setDownWriteErrCnt(int cnt);
    void setCardType(const QString &cardType);
    void setCardNum(const QString &cardNum);
    void setEnTime(const QString &enTime);
    void setEnStationName(const QString &enStationName);
    void setWeightInfo(const QString &info);
    void setEnPlate(const QString &plate);
    void setLabel1(const QString &info);
    void setSplitProvincesInfo(const QString &info);
    void setTradeHint(const QString &tradeHint, const QString &color = "#000000");
    void setObuHint(const QString &obuHint, const QString &color = "#0C4E94");
    void appendHintButton(const QString &hint, const QString &fontColor = "#ffffff", const QString &bgColor = "#08c134");
};
