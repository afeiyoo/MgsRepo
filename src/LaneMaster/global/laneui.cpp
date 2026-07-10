#include "laneui.h"

#include <QVariantMap>

#include "baseexception.h"
#include "pages/basepage.h"
#include "pages/etcpage.h"
#include "utils/datadealutils.h"

using namespace Utils;

LaneUI::LaneUI(BasePage *page, QObject *parent)
    : QObject{parent}
    , m_page{page}
{}

LaneUI::~LaneUI() {}

QByteArray LaneUI::setStationInfo(const QByteArray &json)
{
    QVariantMap resMap;
    try {
        if (!m_page)
            throw BaseException(-1, "页面未初始化");

        bool ok = false;
        QString errDesc;
        QVariantMap oneMap = DataDealUtils::jsonToMap(json, &ok, &errDesc);
        if (!ok)
            throw BaseException(-1, errDesc);

        if (!oneMap.contains("stationInfo"))
            throw BaseException(-1, "未传入stationInfo字段");

        QString stationInfo = oneMap["stationInfo"].toString();
        m_page->setStationInfo(stationInfo);

        resMap["status"] = 0;
        resMap["desc"] = QString("更新顶栏信息: 收费站 - %1").arg(stationInfo);
        return DataDealUtils::mapToJson(resMap);
    } catch (const BaseException &e) {
        resMap["status"] = e.status();
        resMap["desc"] = e.desc();
        return DataDealUtils::mapToJson(resMap);
    }
}

QByteArray LaneUI::setTotalVehCnt(const QByteArray &json)
{
    QVariantMap resMap;
    try {
        if (!m_page)
            throw BaseException(-1, "页面未初始化");

        bool ok = false;
        QString errDesc;
        QVariantMap oneMap = DataDealUtils::jsonToMap(json, &ok, &errDesc);
        if (!ok)
            throw BaseException(-1, errDesc);

        if (!oneMap.contains("totalVehCnt"))
            throw BaseException(-1, "未传入totalVehCnt字段");

        int totalVehCnt = oneMap["totalVehCnt"].toInt();
        m_page->setTotalVehCnt(totalVehCnt);

        resMap["status"] = 0;
        resMap["desc"] = QString("更新工班信息: 总过车数 - %1").arg(totalVehCnt);
        return DataDealUtils::mapToJson(resMap);
    } catch (const BaseException &e) {
        resMap["status"] = e.status();
        resMap["desc"] = e.desc();
        return DataDealUtils::mapToJson(resMap);
    }
}

QByteArray LaneUI::setLastShiftTotalVehCnt(const QByteArray &json)
{
    QVariantMap resMap;
    try {
        EtcPage *page = qobject_cast<EtcPage *>(m_page);
        if (!page)
            throw BaseException(-1, "页面未初始化");

        bool ok = false;
        QString errDesc;
        QVariantMap oneMap = DataDealUtils::jsonToMap(json, &ok, &errDesc);
        if (!ok)
            throw BaseException(-1, errDesc);

        if (!oneMap.contains("lastShiftTotalVehCnt"))
            throw BaseException(-1, "未传入lastShiftTotalVehCnt字段");

        int lastShiftTotalVehCnt = oneMap["lastShiftTotalVehCnt"].toInt();
        page->setLastShiftTotalVehCnt(lastShiftTotalVehCnt);

        resMap["status"] = 0;
        resMap["desc"] = QString("更新工班信息：上个班次总车次 - %1").arg(lastShiftTotalVehCnt);
        return DataDealUtils::mapToJson(resMap);
    } catch (const BaseException &e) {
        resMap["status"] = e.status();
        resMap["desc"] = e.desc();
        return DataDealUtils::mapToJson(resMap);
    }
}
