#include "lanesystemmaster.h"

#include <QApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

LaneSystemMaster::LaneSystemMaster(QObject *parent)
    : QObject{parent}
{}

LaneSystemMaster::~LaneSystemMaster() {}

void LaneSystemMaster::init(QApplication &app)
{
    m_gui.initFront(app);
}

void LaneSystemMaster::createMtcIn() {}

void LaneSystemMaster::createMtcOut() {}

void LaneSystemMaster::createEtc(int argc, char *argv[])
{
    m_etcPageCtrl = m_gui.createEtcWindow(false, this);
    m_etcBizCtrl = new ETC(this);
    if (m_etcBizCtrl->init(argc, argv) < 0)
        return exit(-1);

    connect(m_etcPageCtrl, &EtcPageController::sigKeyPress, m_etcBizCtrl, &ETC::onKeyPress);
    connect(m_etcPageCtrl, &EtcPageController::sigShowFormResp, m_etcBizCtrl, &ETC::onShowFormResp);

    connect(m_etcBizCtrl, &ETC::sigSetStationInfo, m_etcPageCtrl, [=](const QString &stationInfo) { m_etcPageCtrl->setStationInfo(stationInfo); });
    connect(m_etcBizCtrl, &ETC::sigSetUserInfo, m_etcPageCtrl, [=](const QString &userInfo) { m_etcPageCtrl->setUserInfo(userInfo); });
    connect(m_etcBizCtrl, &ETC::sigSetLaneID, m_etcPageCtrl, [=](uint laneID) { m_etcPageCtrl->setLaneID(laneID); });
    connect(m_etcBizCtrl, &ETC::sigSetShiftInfo, m_etcPageCtrl, [=](const QString &shiftInfo) { m_etcPageCtrl->setShiftInfo(shiftInfo); });
    connect(m_etcBizCtrl, &ETC::sigSetModeText, m_etcPageCtrl, [=](const QString &mode) { m_etcPageCtrl->setModeText(mode); });

    connect(m_etcBizCtrl, &ETC::sigSetFullBlackVer, m_etcPageCtrl, [=](const QString &ver) { m_etcPageCtrl->setFullBlackVer(ver); });
    connect(m_etcBizCtrl, &ETC::sigSetPartBlackVer, m_etcPageCtrl, [=](const QString &ver) { m_etcPageCtrl->setPartBlackVer(ver); });
    connect(m_etcBizCtrl, &ETC::sigSetVirtualGantryInfo, m_etcPageCtrl, [=](const QString &info) { m_etcPageCtrl->setVirtualGantryInfo(info); });
    connect(m_etcBizCtrl, &ETC::sigSetAppVer, m_etcPageCtrl, [=](const QString &ver) { m_etcPageCtrl->setAppVer(ver); });
    connect(m_etcBizCtrl, &ETC::sigSetFeeRateVer, m_etcPageCtrl, [=](const QString &ver) { m_etcPageCtrl->setFeeRateVer(ver); });

    connect(m_etcBizCtrl, &ETC::sigSetCapImage, m_etcPageCtrl, [=](const QImage &img) { m_etcPageCtrl->setCapImage(img); });
    connect(m_etcBizCtrl, &ETC::sigSetScrollTip, m_etcPageCtrl, [=](const QString &tip) { m_etcPageCtrl->setScrollTip(tip); });
    connect(m_etcBizCtrl, &ETC::sigLogAppend, m_etcPageCtrl,
            [=](uint logLevel, const QString &log) { m_etcPageCtrl->logAppend(static_cast<LaneSystemGUI::EM_LogLevel>(logLevel), log); });

    connect(m_etcBizCtrl, &ETC::sigSetPlate, m_etcPageCtrl, [=](const QString &plate) { m_etcPageCtrl->setPlate(plate); });
    connect(m_etcBizCtrl, &ETC::sigSetVehClass, m_etcPageCtrl, [=](const QString &vehClass) { m_etcPageCtrl->setVehClass(vehClass); });
    connect(m_etcBizCtrl, &ETC::sigSetVehStatus, m_etcPageCtrl, [=](const QString &vehStatus) { m_etcPageCtrl->setVehStatus(vehStatus); });
    connect(m_etcBizCtrl, &ETC::sigSetSituation, m_etcPageCtrl, [=](const QString &situation) { m_etcPageCtrl->setSituation(situation); });

    connect(m_etcBizCtrl, &ETC::sigSetTradeHint, m_etcPageCtrl,
            [=](const QString &tradeHint, bool isWarn) { m_etcPageCtrl->setTradeHint(tradeHint, isWarn); });
    connect(m_etcBizCtrl, &ETC::sigSetObuHint, m_etcPageCtrl,
            [=](const QString &obuHint, bool isWarn) { m_etcPageCtrl->setObuHint(obuHint, isWarn); });

    connect(m_etcBizCtrl, &ETC::sigAppendTradeItem, m_etcPageCtrl, [=](const QStringList &trade) { m_etcPageCtrl->appendTradeItem(trade); });
    connect(m_etcBizCtrl, &ETC::sigClearTradeItems, m_etcPageCtrl, [=]() { m_etcPageCtrl->clearTradeItems(); });

    connect(m_etcBizCtrl, &ETC::sigSetDeviceList, m_etcPageCtrl, [=](const QList<uint> &devList) { m_etcPageCtrl->setDeviceList(devList); });
    connect(m_etcBizCtrl, &ETC::sigUpdateDeviceStatus, m_etcPageCtrl,
            [=](uint dev, uint status) { m_etcPageCtrl->updateDeviceStatus(static_cast<LaneSystemGUI::EM_DeviceIcon>(dev), status); });

    connect(m_etcBizCtrl, &ETC::sigSetTotalVehCnt, m_etcPageCtrl, [=](int cnt) { m_etcPageCtrl->setTotalVehCnt(cnt); });
    connect(m_etcBizCtrl, &ETC::sigSetNormalVehCnt, m_etcPageCtrl, [=](int cnt) { m_etcPageCtrl->setNormalVehCnt(cnt); });
    connect(m_etcBizCtrl, &ETC::sigSetFreeVehCnt, m_etcPageCtrl, [=](int cnt) { m_etcPageCtrl->setFreeVehCnt(cnt); });
    connect(m_etcBizCtrl, &ETC::sigSetTotalToll, m_etcPageCtrl, [=](qreal fee) { m_etcPageCtrl->setTotalToll(fee); });
    connect(m_etcBizCtrl, &ETC::sigSetCreditCardCnt, m_etcPageCtrl, [=](int cnt) { m_etcPageCtrl->setCreditCardCnt(cnt); });
    connect(m_etcBizCtrl, &ETC::sigSetPrePayCardCnt, m_etcPageCtrl, [=](int cnt) { m_etcPageCtrl->setPrePayCardCnt(cnt); });
    connect(m_etcBizCtrl, &ETC::sigSetHolidayFreeVehCnt, m_etcPageCtrl, [=](int cnt) { m_etcPageCtrl->setHolidayFreeVehCnt(cnt); });
    connect(m_etcBizCtrl, &ETC::sigSetPeccanyVehCnt, m_etcPageCtrl, [=](int cnt) { m_etcPageCtrl->setPeccanyVehCnt(cnt); });
    connect(m_etcBizCtrl, &ETC::sigSetLastShiftTotalVehCnt, m_etcPageCtrl, [=](int cnt) { m_etcPageCtrl->setLastShiftTotalVehCnt(cnt); });

    connect(m_etcBizCtrl, &ETC::sigSetCardType, m_etcPageCtrl, [=](const QString &cardType) { m_etcPageCtrl->setCardType(cardType); });
    connect(m_etcBizCtrl, &ETC::sigSetProvince, m_etcPageCtrl, [=](const QString &province) { m_etcPageCtrl->setProvince(province); });
    connect(m_etcBizCtrl, &ETC::sigSetBalance, m_etcPageCtrl, [=](const QString &balance) { m_etcPageCtrl->setBalance(balance); });
    connect(m_etcBizCtrl, &ETC::sigSetTradeTime, m_etcPageCtrl, [=](const QString &time) { m_etcPageCtrl->setTradeTime(time); });
    connect(m_etcBizCtrl, &ETC::sigSetEnStationName, m_etcPageCtrl,
            [=](const QString &enStationName) { m_etcPageCtrl->setEnStationName(enStationName); });
    connect(m_etcBizCtrl, &ETC::sigSetToll, m_etcPageCtrl, [=](const QString &toll) { m_etcPageCtrl->setToll(toll); });

    connect(m_etcBizCtrl, &ETC::sigShowFormRequest, m_etcPageCtrl, [=](int formType, int api, const QJsonValue &values) {
        if (formType == 1) {
            m_etcPageCtrl->setApi(api);
            QString title = values["title"].toString();

            QStringList strs;
            QJsonArray arr = values["strs"].toArray();

            for (const QJsonValue &v : arr) {
                strs << v.toString();
            }
            bool switchLine = values["switchLine"].toBool();

            m_etcPageCtrl->showInfoDialog(title, strs, switchLine);
        }
    });
}
