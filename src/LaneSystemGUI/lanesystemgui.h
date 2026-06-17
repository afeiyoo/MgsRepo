#pragma once

#include <QImage>
#include <QList>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QStringList>

#include "IPageController.h"
#include "lanesystemgui_global.h"

class QApplication;
class QWidget;
class BasePage;
class EtcPage;
class MtcInPage;
class MtcOutPage;
class MainWindow;

class LANESYSTEMGUI_EXPORT EtcPageController : public QObject, public IEtcPageController
{
    Q_OBJECT
public:
    explicit EtcPageController(EtcPage *page, QObject *parent = nullptr);
    ~EtcPageController() override;

    // 顶部信息栏API
    void setStationInfo(const QString &stationInfo) override;
    void setUserInfo(const QString &userInfo) override;
    void setLaneID(uint laneID) override;
    void setShiftInfo(const QString &shiftInfo) override;
    void setModeText(const QString &mode) override;

    // 底部状态栏API
    void setFullBlackVer(const QString &ver) override;
    void setPartBlackVer(const QString &ver) override;
    void setVirtualGantryInfo(const QString &info) override;
    void setAppVer(const QString &ver) override;
    void setFeeRateVer(const QString &ver) override;

    // 抓拍显示区域API
    void setCapImage(const QImage &img) override;

    // 滚动提示区域API
    void setScrollTip(const QString &tip) override;

    // 日志显示区域API
    void logAppend(EM_LogLevel logLevel, const QString &log) override;

    // 当前车辆与卡内信息显示区域API
    void setPlate(const QString &plate) override;
    void setVehClass(const QString &vehClass) override;
    void setVehStatus(const QString &vehStatus) override;
    void setSituation(const QString &situation) override;
    void setCardType(const QString &cardType) override;
    void setProvince(const QString &province) override;
    void setBalance(const QString &balance) override;
    void setTradeTime(const QString &time) override;
    void setEnStationName(const QString &enStationName) override;
    void setToll(const QString &toll) override;

    // 工班信息显示区域API
    void setTotalVehCnt(int cnt) override;
    void setNormalVehCnt(int cnt) override;
    void setFreeVehCnt(int cnt) override;
    void setTotalToll(qreal fee) override;
    void setCreditCardCnt(int cnt) override;
    void setPrePayCardCnt(int cnt) override;
    void setHolidayFreeVehCnt(int cnt) override;
    void setPeccanyVehCnt(int cnt) override;
    void setLastShiftTotalVehCnt(int cnt) override;

    // 交易提示区域API
    void setTradeHint(const QString &tradeHint, bool isWarn = false) override;
    void setObuHint(const QString &obuHint, bool isWarn = false) override;

    // 近期交易记录查看区域API
    void appendTradeItem(const QStringList &trade) override;
    void clearTradeItems() override;

    // 设备图标显示区域API
    void setDeviceList(const QList<uint> &devList) override;
    void updateDeviceStatus(EM_DeviceIcon dev, EM_DeviceIconStatus status) override;

    // 窗口API
    void showAuthDialog(const QString &id, const QString &name) override;
    void showInfoDialog(const QString &title, const QStringList &strs, bool switchLine = true) override;

    void setApi(int newApi) override;

signals:
    void sigKeyPress(int key);

private:
    EtcPage *m_page = nullptr;
};

class LANESYSTEMGUI_EXPORT MtcInPageController : public QObject, public IMtcInPageController
{
    Q_OBJECT
public:
    explicit MtcInPageController(MtcInPage *page, QObject *parent = nullptr);
    ~MtcInPageController() override;

    // 顶部信息栏API
    void setStationInfo(const QString &stationInfo) override;
    void setUserInfo(const QString &userInfo) override;
    void setLaneID(uint laneID) override;
    void setShiftInfo(const QString &shiftInfo) override;
    void setModeText(const QString &mode) override;

    // 底部状态栏API
    void setFullBlackVer(const QString &ver) override;
    void setPartBlackVer(const QString &ver) override;
    void setVirtualGantryInfo(const QString &info) override;
    void setAppVer(const QString &ver) override;
    void setFeeRateVer(const QString &ver) override;

    // 抓拍显示区域API
    void setCapImage(const QImage &img) override;

    // 滚动提示区域API
    void setScrollTip(const QString &tip) override;

    // 日志显示区域API
    void logAppend(EM_LogLevel logLevel, const QString &log) override;

    // 工班信息显示区域API
    void setTotalVehCnt(uint cnt) override;
    void setTotalCardCnt(uint cnt) override;
    void setNoFlagCardCnt(uint cnt) override;
    void setPeccanyVehCnt(uint cnt) override;
    void setFleetVehCnt(uint cnt) override;
    void setCpcCardCnt(uint cnt) override;
    void setEtcCardCnt(uint cnt) override;
    void setBadCardCnt(uint cnt) override;
    void setPaperCardCnt(uint cnt) override;
    void setHolidayFreeVehCnt(uint cnt) override;
    void setRestTickCnt(uint cnt) override;
    void setStartTicketNum(uint num) override;
    void setEndTicketNum(uint num) override;
    void setCurTicketNum(uint num) override;

    // 当前车辆与卡内信息显示区域API
    void setPlate(const QString &plate) override;
    void setVehClass(const QString &vehClass) override;
    void setVehStatus(const QString &vehStatus) override;
    void setSituation(const QString &situation) override;
    void setCardType(const QString &cardType) override;
    void setCardNum(const QString &cardNum) override;
    void setBalance(const QString &balance) override;
    void setEnTime(const QString &enTime) override;
    void setEnStationName(const QString &enStationName) override;
    void setCardStatus(const QString &cardStatus) override;

    // 交易提示区域API
    void setTradeHint(const QString &tradeHint, bool isWarn = false) override;
    void setObuHint(const QString &obuHint, bool isWarn = false) override;

    // 称重信息显示区域API
    void setCurWeightInfo(const QString &curWeightInfo, bool isWarn = false) override;
    void setCurWeightStatus(uint status) override;
    void setCurWeightInfoCount(uint curWeightInfoCount) override;
    void appendWeightInfoItem(const ST_WeightInfoItem &item) override;
    void removeWeightInfoItem(uint index) override;
    void refreshWeightInfoItem(uint index, const ST_WeightInfoItem &item) override;
    void clearWeightInfoItem() override;
    void setWeightLow(bool isLow) override;

    // 近期交易记录查看区域API
    void appendTradeItem(const QStringList &trade) override;
    void clearTradeItems() override;

    // 卡机状态区域API
    void setRobotStatus1(bool isOk, ushort restCard) override;
    void setRobotStatus2(bool isOk, ushort restCard) override;
    void setRobotStatus3(bool isOk, ushort restCard) override;
    void setRobotStatus4(bool isOk, ushort restCard) override;

    // 设备图标显示区域API
    void setDeviceList(const QList<uint> &devList) override;
    void updateDeviceStatus(EM_DeviceIcon dev, EM_DeviceIconStatus status) override;

    // 窗口API
    void showAuthDialog(const QString &id, const QString &name) override;
    void showInfoDialog(const QString &title, const QStringList &strs, bool switchLine = true) override;

    void setApi(int newApi) override;

signals:
    void sigKeyPress(int key);

private:
    MtcInPage *m_page = nullptr;
};

class LANESYSTEMGUI_EXPORT MtcOutPageController : public QObject, public IMtcOutPageController
{
    Q_OBJECT
public:
    explicit MtcOutPageController(MtcOutPage *page, QObject *parent = nullptr);
    ~MtcOutPageController() override;

    void setStationInfo(const QString &stationInfo) override;
    void setUserInfo(const QString &userInfo) override;
    void setLaneID(uint laneID) override;
    void setShiftInfo(const QString &shiftInfo) override;
    void setModeText(const QString &mode) override;
    void setFullBlackVer(const QString &ver) override;
    void setPartBlackVer(const QString &ver) override;
    void setVirtualGantryInfo(const QString &info) override;
    void setAppVer(const QString &ver) override;
    void setFeeRateVer(const QString &ver) override;
    void setCapImage(const QImage &img) override;
    void setScrollTip(const QString &tip) override;
    void logAppend(EM_LogLevel logLevel, const QString &log) override;
    void setPlate(const QString &plate) override;
    void setVehClass(const QString &vehClass) override;
    void setVehStatus(const QString &vehStatus) override;
    void setSituation(const QString &situation) override;
    void setTradeHint(const QString &tradeHint, bool isWarn = false) override;
    void setObuHint(const QString &obuHint, bool isWarn = false) override;
    void appendTradeItem(const QStringList &trade) override;
    void clearTradeItems() override;
    void setDeviceList(const QList<uint> &devList) override;
    void updateDeviceStatus(EM_DeviceIcon dev, EM_DeviceIconStatus status) override;
    void showAuthDialog(const QString &id, const QString &name) override;
    void showInfoDialog(const QString &title, const QStringList &strs, bool switchLine = true) override;
    void setApi(int newApi) override;
    void setPrevImage(const QImage &img) override;
    void enableSptShiftInfoShow(bool isSptShiftInfo) override;
    void setStartTicketNum(int num) override;
    void setCurTicketNum(int num) override;
    void setNormalTicketCnt(int cnt) override;
    void setScrapTicketCnt(int cnt) override;
    void setRestTicketCnt(int cnt) override;
    void setTotalVehCnt(int cnt) override;
    void setRecycleCardCnt(int cnt) override;
    void setEtcCardCnt(int cnt) override;
    void setFleetVehCnt(int cnt) override;
    void setThirdPayCnt(int cnt) override;
    void setUpStartTicketNum(int num) override;
    void setUpCurTicketNum(int num) override;
    void setUpRestTicketCnt(int cnt) override;
    void setUpRecycleCardCnt(int cnt) override;
    void setUpScrapTicketCnt(int cnt) override;
    void setUpWriteErrCnt(int cnt) override;
    void setDownStartTicketNum(int num) override;
    void setDownCurTicketNum(int num) override;
    void setDownRestTicketCnt(int cnt) override;
    void setDownRecycleCardCnt(int cnt) override;
    void setDownScrapTicketCnt(int cnt) override;
    void setDownWriteErrCnt(int cnt) override;
    void setCardType(const QString &cardType) override;
    void setCardNum(const QString &cardNum) override;
    void setEnTime(const QString &enTime) override;
    void setEnStationName(const QString &enStationName) override;
    void setWeightInfo(const QString &info) override;
    void setEnPlate(const QString &plate) override;
    void setLabel1(const QString &info) override;
    void setSplitProvincesInfo(const QString &info) override;
    void appendHintButton(const QString &hint, EM_PayMethod method) override;
    void setCurWeightInfo(const QString &curWeightInfo, bool isWarn = false) override;
    void setCurWeightStatus(uint status) override;
    void setCurWeightInfoCount(uint curWeightInfoCount) override;
    void appendWeightInfoItem(const ST_WeightInfoItem &item) override;
    void removeWeightInfoItem(uint index) override;
    void refreshWeightInfoItem(uint index, const ST_WeightInfoItem &item) override;
    void clearWeightInfoItem() override;
    void setWeightLow(bool isLow) override;

signals:
    void sigKeyPress(int key);

private:
    MtcOutPage *m_page = nullptr;
};

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
