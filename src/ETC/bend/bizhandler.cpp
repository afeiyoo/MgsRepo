#include "bizhandler.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>

#include "Logger.h"
#include "dao/configs/config.h"
#include "dao/dbs/dataservice.h"
#include "environment.h"
#include "global/apis.h"
#include "global/etcdefs.h"
#include "global/globalmanager.h"
#include "middle/signalctrl.h"
#include "utils/bizutils.h"
#include "utils/datadealutils.h"
#include "utils/defs.h"

using namespace Utils;

BizHandler::BizHandler(QObject *parent)
    : QObject{parent}
{
    m_env = new Environment(this);
}

BizHandler::~BizHandler() {}

bool BizHandler::isInShifted()
{
    return m_env->m_isInShifted;
}

void BizHandler::quitSystemRequest()
{
    if (m_env->m_isInShifted) {
        emit GM_INSTANCE->m_sigCtrl->sigUpdateTradeHint("当前正在上班，无法退出系统");
        emit GM_INSTANCE->m_sigCtrl->sigUpdateHelpHint("请按【F7】操作下班");
        return;
    }

    emit GM_INSTANCE->m_sigCtrl->sigShowInfoDialog(API::SYSTEM_QUIT::QUERY, "请确认是否退出系统?", {"按【确认】键退出，按【返回】键取消"});
}

void BizHandler::quitSystemQuery()
{
    LOG_INFO().noquote() << "系统退出: " << DataDealUtils::curDateTimeStr();
    qApp->exit(0);
}

void BizHandler::onCaptureInfo(const ST_CapVehInfo &info)
{
    if (info.nFullLen <= 0 && info.nPlateLen <= 0 && info.nSpecialLen <= 0 && info.chVehicleClass == 0)
        return;

    QString plate = BizUtils::getPlateWithColor(info.nColor, QString::fromUtf8(info.chPlate));
    LOG_INFO().noquote() << "抓拍返回: 颜色" << BizUtils::getColorFormColorCode(info.nColor) << "，车牌" << QString::fromUtf8(info.chPlate)
                         << "，图片大小" << info.nFullLen;

    // TODO 修改流水抓拍车牌
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

    m_env->m_curCapVehPlate = plate;
    m_env->m_curCapImageFilePath = imgFilePath;

    if (m_env->m_isInShifted && !GM_INSTANCE->m_conf->m_isEnableCompareVehplate) {
        // 保存车道抓拍流水
    }
    if (m_env->m_isInShifted && !GM_INSTANCE->m_conf->m_psdFlagID.isEmpty()) {
        // 保存承载门架抓拍流水
    }
    if (!GM_INSTANCE->m_conf->m_isEnableCompareVehplate) {
        // 保存抓拍图片
    }

    // 界面刷新
}

ST_TradeInfo BizHandler::getTradeInfo(const QString &plate, bool useFirst)
{
    ST_TradeInfo info;
    for (auto it = m_env->m_tradeList.begin(); it != m_env->m_tradeList.end(); ++it) {
        int score = DataDealUtils::calcStrMatchScore(it->vehPlate.trimmed(), plate.trimmed());
        if (score >= 87) {
            info = *it;
            m_env->m_tradeList.erase(it);
            return info;
        }
    }

    if (!useFirst || m_env->m_tradeList.isEmpty())
        return info;

    if (useFirst) { // 兜底使用第一个交易记录
        if (!m_env->m_tradeList.isEmpty()) {
            auto it = m_env->m_tradeList.begin();
            info = *it;
            m_env->m_tradeList.erase(it);
        }
    }

    auto it = m_env->m_tradeList.begin();
    info = *it;
    m_env->m_tradeList.erase(it);

    return info;
}
