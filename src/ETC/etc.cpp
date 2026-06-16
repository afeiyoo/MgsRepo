#include "etc.h"

#include <QJsonObject>
#include <QJsonValue>

#include "Logger.h"
#include "global/apis.h"
#include "global/globalmanager.h"
#include "middle/gateway.h"
#include "middle/signalctrl.h"
#include "utils/bizutils.h"

using namespace Utils;

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

    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigShowFormRequest, this, &ETC::sigShowFormRequest);

    return ret;
}

void ETC::onKeyPress(int key)
{
    LOG_INFO().noquote() << "按键:" << BizUtils::getKeyDescByCode(GM_INSTANCE->m_keyBoard, key);
    switch (key) {
    case Qt::Key_F1: { // 抓拍测试
    } break;
    case Qt::Key_F7: { // 上下班
    } break;
    case Qt::Key_F8: { // 节假日模式启用/关闭
    } break;
    case Qt::Key_F10: { // 维护
    } break;
    case Qt::Key_F11: { // 班次过车数据查看
    } break;
    case Qt::Key_F12: { // 情报板内容显示选择
    } break;
    case Qt::Key_X:
    case Qt::Key_Escape: { // 系统退出
        GM_INSTANCE->m_gate->send(API::SYSTEM_QUIT::REQUEST, QJsonObject());
    } break;
    case Qt::Key_I: { // 上班
    } break;
    case Qt::Key_G:
    case Qt::Key_U: { // 下班
    } break;
    case Qt::Key_C: { // 线圈1模拟
    } break;
    case Qt::Key_V: { // 线圈2模拟
    } break;
    case Qt::Key_B: { // 线圈3模拟
    } break;
    case Qt::Key_N: { // 线圈4模拟
    } break;
    case Qt::Key_H: { // 线圈5模拟
    } break;
    case Qt::Key_J: { // 线圈6模拟
    } break;
    case Qt::Key_M: { // 抓拍线圈模拟
    } break;
    case Qt::Key_L: { // 落杆线圈模拟
    } break;
    default:
        break;
    }
}

void ETC::onShowFormResp(int api, const QJsonValue &values)
{
    GM_INSTANCE->m_gate->send(api, values);
}
