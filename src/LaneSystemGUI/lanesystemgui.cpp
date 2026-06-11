#include "lanesystemgui.h"

#include <QApplication>
#include <QCoreApplication>
#include <QFont>
#include <QFontDatabase>
#include <QImage>
#include <QWidget>

#include "ElaApplication.h"
#include "Logger.h"
#include "global/globalmanager.h"
#include "global/signalmanager.h"
#include "global/uiconst.h"
#include "pages/basepage.h"
#include "pages/etcpage.h"
#include "pages/mainwindow.h"
#include "pages/mtcinpage.h"
#include "pages/mtcoutpage.h"
#include "utils/datadealutils.h"

using namespace Utils;

static QFont resolveAppFont()
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

PageController::PageController(BasePage *page, QObject *parent)
    : QObject{parent}
    , m_page(page)
{
    connect(GM_SIG, &SignalManager::sigKeyPress, this, &PageController::sigKeyPress);
}

PageController::~PageController() {}

BasePage *PageController::page() const
{
    return m_page;
}

void PageController::setStationInfo(const QString &stationInfo)
{
    if (m_page)
        m_page->setStationInfo(stationInfo);
}

void PageController::setUserInfo(const QString &userInfo)
{
    if (m_page)
        m_page->setUserInfo(userInfo);
}

void PageController::setLaneID(uint laneID)
{
    if (m_page)
        m_page->setLaneID(laneID);
}

void PageController::setShiftInfo(const QString &shiftInfo)
{
    if (m_page)
        m_page->setShiftInfo(shiftInfo);
}

void PageController::setModeText(const QString &mode)
{
    if (m_page)
        m_page->setModeText(mode);
}

void PageController::setFullBlackVer(const QString &ver)
{
    if (m_page)
        m_page->setFullBlackVer(ver);
}

void PageController::setPartBlackVer(const QString &ver)
{
    if (m_page)
        m_page->setPartBlackVer(ver);
}

void PageController::setVirtualGantryInfo(const QString &info)
{
    if (m_page)
        m_page->setVirtualGantryInfo(info);
}

void PageController::setAppVer(const QString &ver)
{
    if (m_page)
        m_page->setAppVer(ver);
}

void PageController::setFeeRateVer(const QString &ver)
{
    if (m_page)
        m_page->setFeeRateVer(ver);
}

void PageController::setCapImage(const QImage &img)
{
    if (m_page)
        m_page->setCapImage(img);
}

void PageController::setScrollTip(const QString &tip)
{
    if (m_page)
        m_page->setScrollTip(tip);
}

void PageController::logAppend(LaneSystemGUI::EM_LogLevel logLevel, const QString &log)
{
    if (m_page)
        m_page->logAppend(logLevel, log);
}

void PageController::setPlate(const QString &plate)
{
    if (m_page)
        m_page->setPlate(plate);
}

void PageController::setVehClass(const QString &vehClass)
{
    if (m_page)
        m_page->setVehClass(vehClass);
}

void PageController::setVehStatus(const QString &vehStatus)
{
    if (m_page)
        m_page->setVehStatus(vehStatus);
}

void PageController::setSituation(const QString &situation)
{
    if (m_page)
        m_page->setSituation(situation);
}

void PageController::setTradeHint(const QString &tradeHint, const QString &color)
{
    if (m_page)
        m_page->setTradeHint(tradeHint, color);
}

void PageController::setObuHint(const QString &obuHint, const QString &color)
{
    if (m_page)
        m_page->setObuHint(obuHint, color);
}

void PageController::setCurWeightInfo(const QString &curWeightInfo, const QString &color)
{
    if (m_page)
        m_page->setCurWeightInfo(curWeightInfo, color);
}

void PageController::setCurWeightStatus(uint status)
{
    if (m_page)
        m_page->setCurWeightStatus(status);
}

void PageController::setCurWeightInfoCount(uint curWeightInfoCount)
{
    if (m_page)
        m_page->setCurWeightInfoCount(curWeightInfoCount);
}

void PageController::appendWeightInfoItem(const ST_WeightInfoItem &item)
{
    if (m_page)
        m_page->appendWeightInfoItem(item);
}

void PageController::removeWeightInfoItem(uint index)
{
    if (m_page)
        m_page->removeWeightInfoItem(index);
}

void PageController::refreshWeightInfoItem(uint index, const ST_WeightInfoItem &item)
{
    if (m_page)
        m_page->refreshWeightInfoItem(index, item);
}

void PageController::clearWeightInfoItem()
{
    if (m_page)
        m_page->clearWeightInfoItem();
}

void PageController::setWeightLow(bool isLow)
{
    if (m_page)
        m_page->setWeightLow(isLow);
}

void PageController::appendTradeItem(const QStringList &trade)
{
    if (m_page)
        m_page->appendTradeItem(trade);
}

void PageController::clearTradeItems()
{
    if (m_page)
        m_page->clearTradeItems();
}

void PageController::setDeviceList(const QList<uint> &devList)
{
    if (m_page)
        m_page->setDeviceList(devList);
}

void PageController::updateDeviceStatus(LaneSystemGUI::EM_DeviceIcon dev, uint status)
{
    if (m_page)
        m_page->updateDeviceStatus(dev, status);
}

void PageController::showAuthDialog(const QString &id, const QString &name)
{
    if (m_page)
        m_page->showAuthDialog(id, name);
}

EtcPageController::EtcPageController(EtcPage *page, QObject *parent)
    : PageController(page, parent)
{}

void EtcPageController::setTotalVehCnt(int cnt)
{
    if (auto *p = qobject_cast<EtcPage *>(page()))
        p->setTotalVehCnt(cnt);
}

void EtcPageController::setNormalVehCnt(int cnt)
{
    if (auto *p = qobject_cast<EtcPage *>(page()))
        p->setNormalVehCnt(cnt);
}

void EtcPageController::setFreeVehCnt(int cnt)
{
    if (auto *p = qobject_cast<EtcPage *>(page()))
        p->setFreeVehCnt(cnt);
}

void EtcPageController::setTotalToll(qreal fee)
{
    if (auto *p = qobject_cast<EtcPage *>(page()))
        p->setTotalToll(fee);
}

void EtcPageController::setCreditCardCnt(int cnt)
{
    if (auto *p = qobject_cast<EtcPage *>(page()))
        p->setCreditCardCnt(cnt);
}

void EtcPageController::setPrePayCardCnt(int cnt)
{
    if (auto *p = qobject_cast<EtcPage *>(page()))
        p->setPrePayCardCnt(cnt);
}

void EtcPageController::setHolidayFreeVehCnt(int cnt)
{
    if (auto *p = qobject_cast<EtcPage *>(page()))
        p->setHolidayFreeVehCnt(cnt);
}

void EtcPageController::setPeccanyVehCnt(int cnt)
{
    if (auto *p = qobject_cast<EtcPage *>(page()))
        p->setPeccanyVehCnt(cnt);
}

void EtcPageController::setLastShiftTotalVehCnt(int cnt)
{
    if (auto *p = qobject_cast<EtcPage *>(page()))
        p->setLastShiftTotalVehCnt(cnt);
}

void EtcPageController::setCardType(const QString &cardType)
{
    if (auto *p = qobject_cast<EtcPage *>(page()))
        p->setCardType(cardType);
}

void EtcPageController::setProvince(const QString &province)
{
    if (auto *p = qobject_cast<EtcPage *>(page()))
        p->setProvince(province);
}

void EtcPageController::setBalance(const QString &balance)
{
    if (auto *p = qobject_cast<EtcPage *>(page()))
        p->setBalance(balance);
}

void EtcPageController::setTradeTime(const QString &time)
{
    if (auto *p = qobject_cast<EtcPage *>(page()))
        p->setTradeTime(time);
}

void EtcPageController::setEnStationName(const QString &enStationName)
{
    if (auto *p = qobject_cast<EtcPage *>(page()))
        p->setEnStationName(enStationName);
}

void EtcPageController::setToll(const QString &toll)
{
    if (auto *p = qobject_cast<EtcPage *>(page()))
        p->setToll(toll);
}

MtcInPageController::MtcInPageController(MtcInPage *page, QObject *parent)
    : PageController(page, parent)
{}

void MtcInPageController::setTotalVehCnt(uint cnt)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setTotalVehCnt(cnt);
}

void MtcInPageController::setTotalCardCnt(uint cnt)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setTotalCardCnt(cnt);
}

void MtcInPageController::setNoFlagCardCnt(uint cnt)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setNoFlagCardCnt(cnt);
}

void MtcInPageController::setPeccanyVehCnt(uint cnt)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setPeccanyVehCnt(cnt);
}

void MtcInPageController::setFleetVehCnt(uint cnt)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setFleetVehCnt(cnt);
}

void MtcInPageController::setCpcCardCnt(uint cnt)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setCpcCardCnt(cnt);
}

void MtcInPageController::setEtcCardCnt(uint cnt)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setEtcCardCnt(cnt);
}

void MtcInPageController::setBadCardCnt(uint cnt)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setBadCardCnt(cnt);
}

void MtcInPageController::setPaperCardCnt(uint cnt)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setPaperCardCnt(cnt);
}

void MtcInPageController::setHolidayFreeVehCnt(uint cnt)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setHolidayFreeVehCnt(cnt);
}

void MtcInPageController::setRestTickCnt(uint cnt)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setRestTickCnt(cnt);
}

void MtcInPageController::setStartTicketNum(uint num)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setStartTicketNum(num);
}

void MtcInPageController::setEndTicketNum(uint num)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setEndTicketNum(num);
}

void MtcInPageController::setCurTicketNum(uint num)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setCurTicketNum(num);
}

void MtcInPageController::setCardType(const QString &cardType)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setCardType(cardType);
}

void MtcInPageController::setCardNum(const QString &cardNum)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setCardNum(cardNum);
}

void MtcInPageController::setBalance(const QString &balance)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setBalance(balance);
}

void MtcInPageController::setEnTime(const QString &enTime)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setEnTime(enTime);
}

void MtcInPageController::setEnStationName(const QString &enStationName)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setEnStationName(enStationName);
}

void MtcInPageController::setCardStatus(const QString &cardStatus)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setCardStatus(cardStatus);
}

void MtcInPageController::setRobotStatus1(bool isOk, ushort restCard)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setRobotStatus1(isOk, restCard);
}

void MtcInPageController::setRobotStatus2(bool isOk, ushort restCard)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setRobotStatus2(isOk, restCard);
}

void MtcInPageController::setRobotStatus3(bool isOk, ushort restCard)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setRobotStatus3(isOk, restCard);
}

void MtcInPageController::setRobotStatus4(bool isOk, ushort restCard)
{
    if (auto *p = qobject_cast<MtcInPage *>(page()))
        p->setRobotStatus4(isOk, restCard);
}

MtcOutPageController::MtcOutPageController(MtcOutPage *page, QObject *parent)
    : PageController(page, parent)
{}

void MtcOutPageController::setPrevImage(const QImage &img)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setPrevImage(img);
}

void MtcOutPageController::enableSptShiftInfoShow(bool isSptShiftInfo)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->enableSptShiftInfoShow(isSptShiftInfo);
}

void MtcOutPageController::setStartTicketNum(int num)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setStartTicketNum(num);
}

void MtcOutPageController::setCurTicketNum(int num)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setCurTicketNum(num);
}

void MtcOutPageController::setNormalTicketCnt(int cnt)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setNormalTicketCnt(cnt);
}

void MtcOutPageController::setScrapTicketCnt(int cnt)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setScrapTicketCnt(cnt);
}

void MtcOutPageController::setRestTicketCnt(int cnt)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setRestTicketCnt(cnt);
}

void MtcOutPageController::setTotalVehCnt(int cnt)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setTotalVehCnt(cnt);
}

void MtcOutPageController::setRecycleCardCnt(int cnt)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setRecycleCardCnt(cnt);
}

void MtcOutPageController::setEtcCardCnt(int cnt)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setEtcCardCnt(cnt);
}

void MtcOutPageController::setFleetVehCnt(int cnt)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setFleetVehCnt(cnt);
}

void MtcOutPageController::setThirdPayCnt(int cnt)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setThirdPayCnt(cnt);
}

void MtcOutPageController::setUpStartTicketNum(int num)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setUpStartTicketNum(num);
}

void MtcOutPageController::setUpCurTicketNum(int num)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setUpCurTicketNum(num);
}

void MtcOutPageController::setUpRestTicketCnt(int cnt)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setUpRestTicketCnt(cnt);
}

void MtcOutPageController::setUpRecycleCardCnt(int cnt)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setUpRecycleCardCnt(cnt);
}

void MtcOutPageController::setUpScrapTicketCnt(int cnt)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setUpScrapTicketCnt(cnt);
}

void MtcOutPageController::setUpWriteErrCnt(int cnt)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setUpWriteErrCnt(cnt);
}

void MtcOutPageController::setDownStartTicketNum(int num)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setDownStartTicketNum(num);
}

void MtcOutPageController::setDownCurTicketNum(int num)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setDownCurTicketNum(num);
}

void MtcOutPageController::setDownRestTicketCnt(int cnt)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setDownRestTicketCnt(cnt);
}

void MtcOutPageController::setDownRecycleCardCnt(int cnt)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setDownRecycleCardCnt(cnt);
}

void MtcOutPageController::setDownScrapTicketCnt(int cnt)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setDownScrapTicketCnt(cnt);
}

void MtcOutPageController::setDownWriteErrCnt(int cnt)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setDownWriteErrCnt(cnt);
}

void MtcOutPageController::setCardType(const QString &cardType)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setCardType(cardType);
}

void MtcOutPageController::setCardNum(const QString &cardNum)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setCardNum(cardNum);
}

void MtcOutPageController::setEnTime(const QString &enTime)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setEnTime(enTime);
}

void MtcOutPageController::setEnStationName(const QString &enStationName)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setEnStationName(enStationName);
}

void MtcOutPageController::setWeightInfo(const QString &info)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setWeightInfo(info);
}

void MtcOutPageController::setEnPlate(const QString &plate)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setEnPlate(plate);
}

void MtcOutPageController::setLabel1(const QString &info)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setLabel1(info);
}

void MtcOutPageController::setSplitProvincesInfo(const QString &info)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setSplitProvincesInfo(info);
}

void MtcOutPageController::setTradeHint(const QString &tradeHint, const QString &color)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setTradeHint(tradeHint, color);
}

void MtcOutPageController::setObuHint(const QString &obuHint, const QString &color)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->setObuHint(obuHint, color);
}

void MtcOutPageController::appendHintButton(const QString &hint, const QString &fontColor, const QString &bgColor)
{
    if (auto *p = qobject_cast<MtcOutPage *>(page()))
        p->appendHintButton(hint, fontColor, bgColor);
}

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

    QObject::connect(&app, &QCoreApplication::aboutToQuit, []() { LOG_INFO().noquote() << "LaneSystemGUI 退出:" << DataDealUtils::curDateTimeStr(); });

    LOG_INFO().noquote() << "LaneSystemGUI 启动:" << DataDealUtils::curDateTimeStr();
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
