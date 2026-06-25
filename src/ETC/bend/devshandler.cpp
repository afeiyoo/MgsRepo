#include "devshandler.h"

#include "Logger.h"
#include "bend/environment.h"
#include "dao/configs/config.h"
#include "global/etcdefs.h"
#include "global/globalmanager.h"
#include "utils/bizutils.h"
#include "utils/datadealutils.h"
#include "utils/defs.h"

using namespace Utils;

DevsHandler::DevsHandler(QObject *parent)
    : QObject{parent}
{}

DevsHandler::~DevsHandler() {}

void DevsHandler::onCaptureInfo(const ST_CapVehInfo &info)
{
    if (info.nFullLen <= 0 && info.nPlateLen <= 0 && info.nSpecialLen <= 0 && info.chVehicleClass == 0)
        return;

    QString plate = BizUtils::getPlateWithColor(info.nColor, QString::fromUtf8(info.chPlate));
    LOG_INFO().noquote() << "抓拍返回: 颜色" << BizUtils::getColorFormColorCode(info.nColor) << "，车牌" << QString::fromUtf8(info.chPlate)
                         << "，图片大小" << info.nFullLen;

    // TODO 获取车牌对应流水
    QString imgFilePath;
    if (!GM_INSTANCE->m_conf->m_isEnableCompareVehplate) {
        ST_TradeInfo info = getTradeInfo(plate);
        if (!info.tradeID.isEmpty()) {
            imgFilePath += QString("ImageQ_%1.jpg").arg(info.tradeID);
        } else {
            QString tempID = QString("%1%2%3MG")
                                 .arg(GM_INSTANCE->m_conf->m_stationID)
                                 .arg(GM_INSTANCE->m_conf->m_laneID, 2, 10, QChar('0'))
                                 .arg(DataDealUtils::curDateTimeStr("yyMMddhhmmsszzz"));
            imgFilePath += QString("ImageQ_%1.jpg").arg(tempID);
        }
    } else {
    }

    GM_INSTANCE->m_env->m_curCapVehPlate = plate;
    GM_INSTANCE->m_env->m_curCapImageFilePath = imgFilePath;

    if (GM_INSTANCE->m_env->m_isShiftStarted && !GM_INSTANCE->m_conf->m_isEnableCompareVehplate) {
        // 保存车道抓拍流水
    }
    if (GM_INSTANCE->m_env->m_isShiftStarted && !GM_INSTANCE->m_conf->m_psdFlagID.isEmpty()) {
        // 保存承载门架抓拍流水
    }
    if (!GM_INSTANCE->m_conf->m_isEnableCompareVehplate) {
        // 保存抓拍图片
    }

    // 界面刷新
}

void DevsHandler::onVDInfo()
{

}

ST_TradeInfo DevsHandler::getTradeInfo(const QString &plate, bool useFirst)
{
    ST_TradeInfo info;
    for (auto it = GM_INSTANCE->m_env->m_tradeList.begin(); it != GM_INSTANCE->m_env->m_tradeList.end(); ++it) {
        int score = DataDealUtils::calcStrMatchScore(it->vehPlate.trimmed(), plate.trimmed());
        if (score >= 87) {
            info = *it;
            GM_INSTANCE->m_env->m_tradeList.erase(it);
            return info;
        }
    }

    if (!useFirst || GM_INSTANCE->m_env->m_tradeList.isEmpty())
        return info;

    if (useFirst) { // 兜底使用第一个交易记录
        if (!GM_INSTANCE->m_env->m_tradeList.isEmpty()) {
            auto it = GM_INSTANCE->m_env->m_tradeList.begin();
            info = *it;
            GM_INSTANCE->m_env->m_tradeList.erase(it);
        }
    }

    auto it = GM_INSTANCE->m_env->m_tradeList.begin();
    info = *it;
    GM_INSTANCE->m_env->m_tradeList.erase(it);

    return info;
}
