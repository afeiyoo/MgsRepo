#include "lanesystemgui.h"

#include <QApplication>
#include <QCoreApplication>
#include <QFont>
#include <QFontDatabase>
#include <QImage>
#include <QWidget>

#include "ElaApplication.h"
#include "global/globalmanager.h"
#include "global/signalmanager.h"
#include "global/uiconst.h"
#include "pages/etcpage.h"
#include "pages/mainwindow.h"
#include "pages/mtcinpage.h"
#include "pages/mtcoutpage.h"

namespace {
QFont resolveAppFont()
{
    static const QString kFont = Path::DISPLAY_FONT;

    int fontId = QFontDatabase::addApplicationFont(kFont);

    if (fontId >= 0) {
        QStringList families = QFontDatabase::applicationFontFamilies(fontId);

        if (!families.isEmpty())
            return QFont(families.first());
    }

    return QFont(QStringLiteral("Sans Serif"));
}
} // namespace

LaneSystemGUI::LaneSystemGUI(QObject *parent)
    : QObject{parent}
{}

LaneSystemGUI::~LaneSystemGUI()
{
    delete m_mainWindow;
}

void LaneSystemGUI::initFront(QApplication &app)
{
    eApp->init();
    GM_INSTANCE->init();

    app.setFont(resolveAppFont());
}

SignalManager *LaneSystemGUI::uiSignalMan() const
{
    return GM_INSTANCE->m_signalMan;
}

MtcInPageController *LaneSystemGUI::createMtcInWindow(bool isMaxShow, QObject *parent)
{
    if (m_mainWindow)
        delete m_mainWindow;

    m_mainWindow = new MainWindow();
    m_mainWindow->initMtcIn();
    if (isMaxShow) {
        m_mainWindow->showMaximized();
    } else {
        m_mainWindow->showNormal();
    }

    return new MtcInPageController(m_mainWindow->mtcInPage(), parent);
}

MtcOutPageController *LaneSystemGUI::createMtcOutWindow(bool isMaxShow, bool isSptShow, QObject *parent)
{
    if (m_mainWindow)
        delete m_mainWindow;

    m_mainWindow = new MainWindow();
    m_mainWindow->initMtcOut(isSptShow);
    if (isMaxShow) {
        m_mainWindow->showMaximized();
    } else {
        m_mainWindow->showNormal();
    }

    return new MtcOutPageController(m_mainWindow->mtcOutPage(), parent);
}

EtcPageController *LaneSystemGUI::createEtcWindow(bool isMaxShow, QObject *parent)
{
    if (m_mainWindow)
        delete m_mainWindow;

    m_mainWindow = new MainWindow();
    m_mainWindow->initEtc();
    if (isMaxShow) {
        m_mainWindow->showMaximized();
    } else {
        m_mainWindow->showNormal();
    }

    return new EtcPageController(m_mainWindow->etcPage(), parent);
}

EtcPageController::EtcPageController(EtcPage *page, QObject *parent)
    : QObject{parent}
    , m_page{page}
{}

EtcPageController::~EtcPageController() {}

void EtcPageController::setStationInfo(const QString &stationInfo)
{
    if (m_page)
        m_page->setStationInfo(stationInfo);
}

void EtcPageController::setUserInfo(const QString &userInfo)
{
    if (m_page)
        m_page->setUserInfo(userInfo);
}

void EtcPageController::setLaneID(uint laneID)
{
    if (m_page)
        m_page->setLaneID(laneID);
}

void EtcPageController::setShiftInfo(const QString &shiftInfo)
{
    if (m_page)
        m_page->setShiftInfo(shiftInfo);
}

void EtcPageController::setModeText(const QString &mode)
{
    if (m_page)
        m_page->setModeText(mode);
}

void EtcPageController::setFullBlackVer(const QString &ver)
{
    if (m_page)
        m_page->setFullBlackVer(ver);
}

void EtcPageController::setPartBlackVer(const QString &ver)
{
    if (m_page)
        m_page->setPartBlackVer(ver);
}

void EtcPageController::setVirtualGantryInfo(const QString &info)
{
    if (m_page)
        m_page->setVirtualGantryInfo(info);
}

void EtcPageController::setAppVer(const QString &ver)
{
    if (m_page)
        m_page->setAppVer(ver);
}

void EtcPageController::setFeeRateVer(const QString &ver)
{
    if (m_page)
        m_page->setFeeRateVer(ver);
}

void EtcPageController::setCapImage(const QImage &img)
{
    if (m_page)
        m_page->setCapImage(img);
}

void EtcPageController::setScrollTip(const QString &tip)
{
    if (m_page)
        m_page->setScrollTip(tip);
}

void EtcPageController::logAppend(EM_LogLevel logLevel, const QString &log)
{
    if (m_page)
        m_page->logAppend(logLevel, log);
}

void EtcPageController::setPlate(const QString &plate)
{
    if (m_page)
        m_page->setPlate(plate);
}

void EtcPageController::setVehClass(const QString &vehClass)
{
    if (m_page)
        m_page->setVehClass(vehClass);
}

void EtcPageController::setVehStatus(const QString &vehStatus)
{
    if (m_page)
        m_page->setVehStatus(vehStatus);
}

void EtcPageController::setSituation(const QString &situation)
{
    if (m_page)
        m_page->setSituation(situation);
}

void EtcPageController::setCardType(const QString &cardType)
{
    if (m_page)
        m_page->setCardType(cardType);
}

void EtcPageController::setProvince(const QString &province)
{
    if (m_page)
        m_page->setProvince(province);
}

void EtcPageController::setBalance(const QString &balance)
{
    if (m_page)
        m_page->setBalance(balance);
}

void EtcPageController::setTradeTime(const QString &time)
{
    if (m_page)
        m_page->setTradeTime(time);
}

void EtcPageController::setEnStationName(const QString &enStationName)
{
    if (m_page)
        m_page->setEnStationName(enStationName);
}

void EtcPageController::setToll(const QString &toll)
{
    if (m_page)
        m_page->setToll(toll);
}

void EtcPageController::setTotalVehCnt(int cnt)
{
    if (m_page)
        m_page->setTotalVehCnt(cnt);
}

void EtcPageController::setNormalVehCnt(int cnt)
{
    if (m_page)
        m_page->setNormalVehCnt(cnt);
}

void EtcPageController::setFreeVehCnt(int cnt)
{
    if (m_page)
        m_page->setFreeVehCnt(cnt);
}

void EtcPageController::setTotalToll(qreal fee)
{
    if (m_page)
        m_page->setTotalToll(fee);
}

void EtcPageController::setCreditCardCnt(int cnt)
{
    if (m_page)
        m_page->setCreditCardCnt(cnt);
}

void EtcPageController::setPrePayCardCnt(int cnt)
{
    if (m_page)
        m_page->setPrePayCardCnt(cnt);
}

void EtcPageController::setHolidayFreeVehCnt(int cnt)
{
    if (m_page)
        m_page->setHolidayFreeVehCnt(cnt);
}

void EtcPageController::setPeccanyVehCnt(int cnt)
{
    if (m_page)
        m_page->setPeccanyVehCnt(cnt);
}

void EtcPageController::setLastShiftTotalVehCnt(int cnt)
{
    if (m_page)
        m_page->setLastShiftTotalVehCnt(cnt);
}

void EtcPageController::setTradeHint(const QString &tradeHint, bool isWarn)
{
    if (!m_page)
        return;
    if (!isWarn) {
        m_page->setTradeHint(tradeHint);
    } else {
        m_page->setTradeHint(tradeHint, Color::WARN_TC);
    }
}

void EtcPageController::setObuHint(const QString &obuHint, bool isWarn)
{
    if (!m_page)
        return;
    if (!isWarn) {
        m_page->setObuHint(obuHint);
    } else {
        m_page->setObuHint(obuHint, Color::WARN_TC);
    }
}

void EtcPageController::appendTradeItem(const QStringList &trade)
{
    if (m_page)
        m_page->appendTradeItem(trade);
}

void EtcPageController::clearTradeItems()
{
    if (m_page)
        m_page->clearTradeItems();
}

void EtcPageController::setDeviceList(const QList<uint> &devList)
{
    if (m_page)
        m_page->setDeviceList(devList);
}

void EtcPageController::updateDeviceStatus(EM_DeviceIcon dev, EM_DeviceIconStatus status)
{
    if (m_page)
        m_page->updateDeviceStatus(dev, static_cast<uint>(status));
}

void EtcPageController::showAuthDialog(const QString &id, const QString &name)
{
    if (m_page)
        m_page->showAuthDialog(id, name);
}

void EtcPageController::showInfoDialog(const QString &title, const QStringList &strs, bool switchLine)
{
    if (m_page)
        m_page->showInfoDialog(title, strs, switchLine);
}

void EtcPageController::setApi(int newApi)
{
    if (m_page)
        m_page->setApi(newApi);
}

MtcInPageController::MtcInPageController(MtcInPage *page, QObject *parent)
    : QObject{parent}
    , m_page{page}
{}

MtcInPageController::~MtcInPageController() {}

void MtcInPageController::setStationInfo(const QString &stationInfo)
{
    if (m_page)
        m_page->setStationInfo(stationInfo);
}

void MtcInPageController::setUserInfo(const QString &userInfo)
{
    if (m_page)
        m_page->setUserInfo(userInfo);
}

void MtcInPageController::setLaneID(uint laneID)
{
    if (m_page)
        m_page->setLaneID(laneID);
}

void MtcInPageController::setShiftInfo(const QString &shiftInfo)
{
    if (m_page)
        m_page->setShiftInfo(shiftInfo);
}

void MtcInPageController::setModeText(const QString &mode)
{
    if (m_page)
        m_page->setModeText(mode);
}

void MtcInPageController::setFullBlackVer(const QString &ver)
{
    if (m_page)
        m_page->setFullBlackVer(ver);
}

void MtcInPageController::setPartBlackVer(const QString &ver)
{
    if (m_page)
        m_page->setPartBlackVer(ver);
}

void MtcInPageController::setVirtualGantryInfo(const QString &info)
{
    if (m_page)
        m_page->setVirtualGantryInfo(info);
}

void MtcInPageController::setAppVer(const QString &ver)
{
    if (m_page)
        m_page->setAppVer(ver);
}

void MtcInPageController::setFeeRateVer(const QString &ver)
{
    if (m_page)
        m_page->setFeeRateVer(ver);
}

void MtcInPageController::setCapImage(const QImage &img)
{
    if (m_page)
        m_page->setCapImage(img);
}

void MtcInPageController::setScrollTip(const QString &tip)
{
    if (m_page)
        m_page->setScrollTip(tip);
}

void MtcInPageController::logAppend(EM_LogLevel logLevel, const QString &log)
{
    if (m_page)
        m_page->logAppend(logLevel, log);
}

void MtcInPageController::setTotalVehCnt(uint cnt)
{
    if (m_page)
        m_page->setTotalVehCnt(cnt);
}

void MtcInPageController::setTotalCardCnt(uint cnt)
{
    if (m_page)
        m_page->setTotalCardCnt(cnt);
}

void MtcInPageController::setNoFlagCardCnt(uint cnt)
{
    if (m_page)
        m_page->setNoFlagCardCnt(cnt);
}

void MtcInPageController::setPeccanyVehCnt(uint cnt)
{
    if (m_page)
        m_page->setPeccanyVehCnt(cnt);
}

void MtcInPageController::setFleetVehCnt(uint cnt)
{
    if (m_page)
        m_page->setFleetVehCnt(cnt);
}

void MtcInPageController::setCpcCardCnt(uint cnt)
{
    if (m_page)
        m_page->setCpcCardCnt(cnt);
}

void MtcInPageController::setEtcCardCnt(uint cnt)
{
    if (m_page)
        m_page->setEtcCardCnt(cnt);
}

void MtcInPageController::setBadCardCnt(uint cnt)
{
    if (m_page)
        m_page->setBadCardCnt(cnt);
}

void MtcInPageController::setPaperCardCnt(uint cnt)
{
    if (m_page)
        m_page->setPaperCardCnt(cnt);
}

void MtcInPageController::setHolidayFreeVehCnt(uint cnt)
{
    if (m_page)
        m_page->setHolidayFreeVehCnt(cnt);
}

void MtcInPageController::setRestTickCnt(uint cnt)
{
    if (m_page)
        m_page->setRestTickCnt(cnt);
}

void MtcInPageController::setStartTicketNum(uint num)
{
    if (m_page)
        m_page->setStartTicketNum(num);
}

void MtcInPageController::setEndTicketNum(uint num)
{
    if (m_page)
        m_page->setEndTicketNum(num);
}

void MtcInPageController::setCurTicketNum(uint num)
{
    if (m_page)
        m_page->setCurTicketNum(num);
}

void MtcInPageController::setPlate(const QString &plate)
{
    if (m_page)
        m_page->setPlate(plate);
}

void MtcInPageController::setVehClass(const QString &vehClass)
{
    if (m_page)
        m_page->setVehClass(vehClass);
}

void MtcInPageController::setVehStatus(const QString &vehStatus)
{
    if (m_page)
        m_page->setVehStatus(vehStatus);
}

void MtcInPageController::setSituation(const QString &situation)
{
    if (m_page)
        m_page->setSituation(situation);
}

void MtcInPageController::setCardType(const QString &cardType)
{
    if (m_page)
        m_page->setCardType(cardType);
}

void MtcInPageController::setCardNum(const QString &cardNum)
{
    if (m_page)
        m_page->setCardNum(cardNum);
}

void MtcInPageController::setBalance(const QString &balance)
{
    if (m_page)
        m_page->setBalance(balance);
}

void MtcInPageController::setEnTime(const QString &enTime)
{
    if (m_page)
        m_page->setEnTime(enTime);
}

void MtcInPageController::setEnStationName(const QString &enStationName)
{
    if (m_page)
        m_page->setEnStationName(enStationName);
}

void MtcInPageController::setCardStatus(const QString &cardStatus)
{
    if (m_page)
        m_page->setCardStatus(cardStatus);
}

void MtcInPageController::setTradeHint(const QString &tradeHint, bool isWarn)
{
    if (!m_page)
        return;
    if (!isWarn) {
        m_page->setTradeHint(tradeHint);
    } else {
        m_page->setTradeHint(tradeHint, Color::WARN_TC);
    }
}

void MtcInPageController::setObuHint(const QString &obuHint, bool isWarn)
{
    if (!m_page)
        return;
    if (!isWarn) {
        m_page->setObuHint(obuHint);
    } else {
        m_page->setObuHint(obuHint, Color::WARN_TC);
    }
}

void MtcInPageController::setCurWeightInfo(const QString &curWeightInfo, bool isWarn)
{
    if (!m_page)
        return;
    if (!isWarn) {
        m_page->setCurWeightInfo(curWeightInfo);
    } else {
        m_page->setCurWeightInfo(curWeightInfo, Color::WARN_TC);
    }
}

void MtcInPageController::setCurWeightStatus(uint status)
{
    if (m_page)
        m_page->setCurWeightStatus(status);
}

void MtcInPageController::setCurWeightInfoCount(uint curWeightInfoCount)
{
    if (m_page)
        m_page->setCurWeightInfoCount(curWeightInfoCount);
}

void MtcInPageController::appendWeightInfoItem(const ST_WeightInfoItem &item)
{
    if (m_page)
        m_page->appendWeightInfoItem(item);
}

void MtcInPageController::removeWeightInfoItem(uint index)
{
    if (m_page)
        m_page->removeWeightInfoItem(index);
}

void MtcInPageController::refreshWeightInfoItem(uint index, const ST_WeightInfoItem &item)
{
    if (m_page)
        m_page->refreshWeightInfoItem(index, item);
}

void MtcInPageController::clearWeightInfoItem()
{
    if (m_page)
        m_page->clearWeightInfoItem();
}

void MtcInPageController::setWeightLow(bool isLow)
{
    if (m_page)
        m_page->setWeightLow(isLow);
}

void MtcInPageController::appendTradeItem(const QStringList &trade)
{
    if (m_page)
        m_page->appendTradeItem(trade);
}

void MtcInPageController::clearTradeItems()
{
    if (m_page)
        m_page->clearTradeItems();
}

void MtcInPageController::setRobotStatus1(bool isOk, ushort restCard)
{
    if (m_page)
        m_page->setRobotStatus1(isOk, restCard);
}

void MtcInPageController::setRobotStatus2(bool isOk, ushort restCard)
{
    if (m_page)
        m_page->setRobotStatus2(isOk, restCard);
}

void MtcInPageController::setRobotStatus3(bool isOk, ushort restCard)
{
    if (m_page)
        m_page->setRobotStatus3(isOk, restCard);
}

void MtcInPageController::setRobotStatus4(bool isOk, ushort restCard)
{
    if (m_page)
        m_page->setRobotStatus4(isOk, restCard);
}

void MtcInPageController::setDeviceList(const QList<uint> &devList)
{
    if (m_page)
        m_page->setDeviceList(devList);
}

void MtcInPageController::updateDeviceStatus(EM_DeviceIcon dev, EM_DeviceIconStatus status)
{
    if (m_page)
        m_page->updateDeviceStatus(dev, static_cast<uint>(status));
}

void MtcInPageController::showAuthDialog(const QString &id, const QString &name)
{
    if (m_page)
        m_page->showAuthDialog(id, name);
}

void MtcInPageController::showInfoDialog(const QString &title, const QStringList &strs, bool switchLine)
{
    if (m_page)
        m_page->showInfoDialog(title, strs, switchLine);
}

void MtcInPageController::setApi(int newApi)
{
    if (m_page)
        m_page->setApi(newApi);
}

MtcOutPageController::MtcOutPageController(MtcOutPage *page, QObject *parent)
    : QObject{parent}
    , m_page{page}
{}

MtcOutPageController::~MtcOutPageController() {}

void MtcOutPageController::setStationInfo(const QString &stationInfo)
{
    if (m_page)
        m_page->setStationInfo(stationInfo);
}

void MtcOutPageController::setUserInfo(const QString &userInfo)
{
    if (m_page)
        m_page->setUserInfo(userInfo);
}

void MtcOutPageController::setLaneID(uint laneID)
{
    if (m_page)
        m_page->setLaneID(laneID);
}

void MtcOutPageController::setShiftInfo(const QString &shiftInfo)
{
    if (m_page)
        m_page->setShiftInfo(shiftInfo);
}

void MtcOutPageController::setModeText(const QString &mode)
{
    if (m_page)
        m_page->setModeText(mode);
}

void MtcOutPageController::setFullBlackVer(const QString &ver)
{
    if (m_page)
        m_page->setFullBlackVer(ver);
}

void MtcOutPageController::setPartBlackVer(const QString &ver)
{
    if (m_page)
        m_page->setPartBlackVer(ver);
}

void MtcOutPageController::setVirtualGantryInfo(const QString &info)
{
    if (m_page)
        m_page->setVirtualGantryInfo(info);
}

void MtcOutPageController::setAppVer(const QString &ver)
{
    if (m_page)
        m_page->setAppVer(ver);
}

void MtcOutPageController::setFeeRateVer(const QString &ver)
{
    if (m_page)
        m_page->setFeeRateVer(ver);
}

void MtcOutPageController::setCapImage(const QImage &img)
{
    if (m_page)
        m_page->setCapImage(img);
}

void MtcOutPageController::setScrollTip(const QString &tip)
{
    if (m_page)
        m_page->setScrollTip(tip);
}

void MtcOutPageController::logAppend(EM_LogLevel logLevel, const QString &log)
{
    if (m_page)
        m_page->logAppend(logLevel, log);
}

void MtcOutPageController::setPlate(const QString &plate)
{
    if (m_page)
        m_page->setPlate(plate);
}

void MtcOutPageController::setVehClass(const QString &vehClass)
{
    if (m_page)
        m_page->setVehClass(vehClass);
}

void MtcOutPageController::setVehStatus(const QString &vehStatus)
{
    if (m_page)
        m_page->setVehStatus(vehStatus);
}

void MtcOutPageController::setSituation(const QString &situation)
{
    if (m_page)
        m_page->setSituation(situation);
}

void MtcOutPageController::setTradeHint(const QString &tradeHint, bool isWarn)
{
    if (!m_page)
        return;
    if (!isWarn) {
        m_page->setTradeHint(tradeHint);
    } else {
        m_page->setTradeHint(tradeHint, Color::WARN_TC);
    }
}

void MtcOutPageController::setObuHint(const QString &obuHint, bool isWarn)
{
    if (!m_page)
        return;
    if (!isWarn) {
        m_page->setObuHint(obuHint);
    } else {
        m_page->setObuHint(obuHint, Color::WARN_TC);
    }
}

void MtcOutPageController::appendTradeItem(const QStringList &trade)
{
    if (m_page)
        m_page->appendTradeItem(trade);
}

void MtcOutPageController::clearTradeItems()
{
    if (m_page)
        m_page->clearTradeItems();
}

void MtcOutPageController::setDeviceList(const QList<uint> &devList)
{
    if (m_page)
        m_page->setDeviceList(devList);
}

void MtcOutPageController::updateDeviceStatus(EM_DeviceIcon dev, EM_DeviceIconStatus status)
{
    if (m_page)
        m_page->updateDeviceStatus(dev, static_cast<uint>(status));
}

void MtcOutPageController::showAuthDialog(const QString &id, const QString &name)
{
    if (m_page)
        m_page->showAuthDialog(id, name);
}

void MtcOutPageController::showInfoDialog(const QString &title, const QStringList &strs, bool switchLine)
{
    if (m_page)
        m_page->showInfoDialog(title, strs, switchLine);
}

void MtcOutPageController::setApi(int newApi)
{
    if (m_page)
        m_page->setApi(newApi);
}

void MtcOutPageController::setPrevImage(const QImage &img)
{
    if (m_page)
        m_page->setPrevImage(img);
}

void MtcOutPageController::setStartTicketNum(int num)
{
    if (m_page)
        m_page->setStartTicketNum(num);
}

void MtcOutPageController::setCurTicketNum(int num)
{
    if (m_page)
        m_page->setCurTicketNum(num);
}

void MtcOutPageController::setNormalTicketCnt(int cnt)
{
    if (m_page)
        m_page->setNormalTicketCnt(cnt);
}

void MtcOutPageController::setScrapTicketCnt(int cnt)
{
    if (m_page)
        m_page->setScrapTicketCnt(cnt);
}

void MtcOutPageController::setRestTicketCnt(int cnt)
{
    if (m_page)
        m_page->setRestTicketCnt(cnt);
}

void MtcOutPageController::setTotalVehCnt(int cnt)
{
    if (m_page)
        m_page->setTotalVehCnt(cnt);
}

void MtcOutPageController::setRecycleCardCnt(int cnt)
{
    if (m_page)
        m_page->setRecycleCardCnt(cnt);
}

void MtcOutPageController::setEtcCardCnt(int cnt)
{
    if (m_page)
        m_page->setEtcCardCnt(cnt);
}

void MtcOutPageController::setFleetVehCnt(int cnt)
{
    if (m_page)
        m_page->setFleetVehCnt(cnt);
}

void MtcOutPageController::setThirdPayCnt(int cnt)
{
    if (m_page)
        m_page->setThirdPayCnt(cnt);
}

void MtcOutPageController::setUpStartTicketNum(int num)
{
    if (m_page)
        m_page->setUpStartTicketNum(num);
}

void MtcOutPageController::setUpCurTicketNum(int num)
{
    if (m_page)
        m_page->setUpCurTicketNum(num);
}

void MtcOutPageController::setUpRestTicketCnt(int cnt)
{
    if (m_page)
        m_page->setUpRestTicketCnt(cnt);
}

void MtcOutPageController::setUpRecycleCardCnt(int cnt)
{
    if (m_page)
        m_page->setUpRecycleCardCnt(cnt);
}

void MtcOutPageController::setUpScrapTicketCnt(int cnt)
{
    if (m_page)
        m_page->setUpScrapTicketCnt(cnt);
}

void MtcOutPageController::setUpWriteErrCnt(int cnt)
{
    if (m_page)
        m_page->setUpWriteErrCnt(cnt);
}

void MtcOutPageController::setDownStartTicketNum(int num)
{
    if (m_page)
        m_page->setDownStartTicketNum(num);
}

void MtcOutPageController::setDownCurTicketNum(int num)
{
    if (m_page)
        m_page->setDownCurTicketNum(num);
}

void MtcOutPageController::setDownRestTicketCnt(int cnt)
{
    if (m_page)
        m_page->setDownRestTicketCnt(cnt);
}

void MtcOutPageController::setDownRecycleCardCnt(int cnt)
{
    if (m_page)
        m_page->setDownRecycleCardCnt(cnt);
}

void MtcOutPageController::setDownScrapTicketCnt(int cnt)
{
    if (m_page)
        m_page->setDownScrapTicketCnt(cnt);
}

void MtcOutPageController::setDownWriteErrCnt(int cnt)
{
    if (m_page)
        m_page->setDownWriteErrCnt(cnt);
}

void MtcOutPageController::setCardType(const QString &cardType)
{
    if (m_page)
        m_page->setCardType(cardType);
}

void MtcOutPageController::setCardNum(const QString &cardNum)
{
    if (m_page)
        m_page->setCardNum(cardNum);
}

void MtcOutPageController::setEnTime(const QString &enTime)
{
    if (m_page)
        m_page->setEnTime(enTime);
}

void MtcOutPageController::setEnStationName(const QString &enStationName)
{
    if (m_page)
        m_page->setEnStationName(enStationName);
}

void MtcOutPageController::setWeightInfo(const QString &info)
{
    if (m_page)
        m_page->setWeightInfo(info);
}

void MtcOutPageController::setEnPlate(const QString &plate)
{
    if (m_page)
        m_page->setEnPlate(plate);
}

void MtcOutPageController::setLabel1(const QString &info)
{
    if (m_page)
        m_page->setLabel1(info);
}

void MtcOutPageController::setSplitProvincesInfo(const QString &info)
{
    if (m_page)
        m_page->setSplitProvincesInfo(info);
}

void MtcOutPageController::appendHintButton(const QString &hint, EM_PayMethod method)
{
    if (!m_page)
        return;
    if (method == ALI) {
        m_page->appendHintButton(hint, "#2b66e1");
    } else if (method == WECHAT) {
        m_page->appendHintButton(hint, "#489748");
    } else if (method == THRIDPAY) {
        m_page->appendHintButton(hint, "#4890a9");
    } else if (method == CASH) {
        m_page->appendHintButton(hint, "#333ee7");
    } else {
        m_page->appendHintButton(hint, "#d5552a");
    }
}

void MtcOutPageController::setCurWeightInfo(const QString &curWeightInfo, bool isWarn)
{
    if (!m_page)
        return;
    if (!isWarn) {
        m_page->setCurWeightInfo(curWeightInfo);
    } else {
        m_page->setCurWeightInfo(curWeightInfo, Color::WARN_TC);
    }
}

void MtcOutPageController::setCurWeightStatus(uint status)
{
    if (m_page)
        m_page->setCurWeightStatus(status);
}

void MtcOutPageController::setCurWeightInfoCount(uint curWeightInfoCount)
{
    if (m_page)
        m_page->setCurWeightInfoCount(curWeightInfoCount);
}

void MtcOutPageController::appendWeightInfoItem(const ST_WeightInfoItem &item)
{
    if (m_page)
        m_page->appendWeightInfoItem(item);
}

void MtcOutPageController::removeWeightInfoItem(uint index)
{
    if (m_page)
        m_page->removeWeightInfoItem(index);
}

void MtcOutPageController::refreshWeightInfoItem(uint index, const ST_WeightInfoItem &item)
{
    if (m_page)
        m_page->refreshWeightInfoItem(index, item);
}

void MtcOutPageController::clearWeightInfoItem()
{
    if (m_page)
        m_page->clearWeightInfoItem();
}

void MtcOutPageController::setWeightLow(bool isLow)
{
    if (m_page)
        m_page->setWeightLow(isLow);
}
