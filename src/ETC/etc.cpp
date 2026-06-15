#include "etc.h"

#include <QJsonObject>
#include <QJsonValue>

#include "global/apis.h"
#include "global/globalmanager.h"
#include "middle/gateway.h"
#include "middle/signalctrl.h"

ETC::ETC(QObject *parent)
    : QObject{parent}
{}

ETC::~ETC() {}

int ETC::init(int argc, char *argv[])
{
    int ret = GM_INSTANCE->init(argc, argv);
    if (ret < 0 || !GM_INSTANCE->m_sigCtrl) {
        return ret;
    }

    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetStationInfo, this, &ETC::sigSetStationInfo);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetUserInfo, this, &ETC::sigSetUserInfo);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetLaneID, this, &ETC::sigSetLaneID);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetShiftInfo, this, &ETC::sigSetShiftInfo);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetModeText, this, &ETC::sigSetModeText);

    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetFullBlackVer, this, &ETC::sigSetFullBlackVer);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetPartBlackVer, this, &ETC::sigSetPartBlackVer);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetVirtualGantryInfo, this, &ETC::sigSetVirtualGantryInfo);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetAppVer, this, &ETC::sigSetAppVer);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetFeeRateVer, this, &ETC::sigSetFeeRateVer);

    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetCapImage, this, &ETC::sigSetCapImage);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetScrollTip, this, &ETC::sigSetScrollTip);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigLogAppend, this, &ETC::sigLogAppend);

    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetPlate, this, &ETC::sigSetPlate);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetVehClass, this, &ETC::sigSetVehClass);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetVehStatus, this, &ETC::sigSetVehStatus);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetSituation, this, &ETC::sigSetSituation);

    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetTradeHint, this, &ETC::sigSetTradeHint);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetObuHint, this, &ETC::sigSetObuHint);

    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigAppendTradeItem, this, &ETC::sigAppendTradeItem);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigClearTradeItems, this, &ETC::sigClearTradeItems);

    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetDeviceList, this, &ETC::sigSetDeviceList);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigUpdateDeviceStatus, this, &ETC::sigUpdateDeviceStatus);

    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigShowAuthDialog, this, &ETC::sigShowAuthDialog);

    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetTotalVehCnt, this, &ETC::sigSetTotalVehCnt);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetNormalVehCnt, this, &ETC::sigSetNormalVehCnt);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetFreeVehCnt, this, &ETC::sigSetFreeVehCnt);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetTotalToll, this, &ETC::sigSetTotalToll);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetCreditCardCnt, this, &ETC::sigSetCreditCardCnt);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetPrePayCardCnt, this, &ETC::sigSetPrePayCardCnt);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetHolidayFreeVehCnt, this, &ETC::sigSetHolidayFreeVehCnt);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetPeccanyVehCnt, this, &ETC::sigSetPeccanyVehCnt);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetLastShiftTotalVehCnt, this, &ETC::sigSetLastShiftTotalVehCnt);

    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetCardType, this, &ETC::sigSetCardType);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetProvince, this, &ETC::sigSetProvince);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetBalance, this, &ETC::sigSetBalance);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetTradeTime, this, &ETC::sigSetTradeTime);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetEnStationName, this, &ETC::sigSetEnStationName);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigSetToll, this, &ETC::sigSetToll);

    return ret;
}

void ETC::onKeyPress(int key)
{
    QJsonObject params;
    params["key"] = key;

    GM_INSTANCE->m_gate->send(API::KEY_PRESS, params);
}
