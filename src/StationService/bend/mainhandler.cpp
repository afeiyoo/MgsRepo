#include "mainhandler.h"

#include <QJsonDocument>
#include <QVariantMap>

#include "Logger.h"
#include "core/baseexception.h"
#include "core/globalmanager.h"
#include "dbs/dataservice.h"
#include "utils/datadealutils.h"

using namespace Utils;

MainHandler::MainHandler(const QString &peerIP, QObject *parent)
    : QObject{parent}
    , m_peerIP{peerIP}
{}

QString MainHandler::doMainDeal(const QByteArray &reqBody) const
{
    bool ok = false;
    QString errDesc;
    QVariantMap aMap = DataDealUtils::jsonToMap(reqBody, ok, errDesc);

    if (!ok) {
        LOG_ERROR().noquote() << errDesc;
        throw BaseException(1, errDesc);
    }

    QString queryType;
    if (aMap.contains("queryType"))
        queryType = aMap["queryType"].toString();

    QString dealtData;
    if (queryType.contains("queryRepeat")) {
        dealtData = dealQueryRepeat(aMap);
    } else if (queryType.contains("saveData")) {
    } else if (queryType.contains("queryXZPass")) {
    } else if (queryType.contains("queryShift")) {
    } else if (queryType.contains("queryData")) {
    } else {
    }

    return dealtData;
}

QString MainHandler::dealQueryRepeat(const QVariantMap &aMap) const
{
    int dataType = 0;
    int vehicleIdentifyType = 0;
    QString vehPlate;
    QString cardId;
    int judgeTime = 0;
    if (aMap.contains("dataType"))
        dataType = aMap["dataType"].toInt();
    if (aMap.contains("vehicleIdentifyType"))
        vehicleIdentifyType = aMap["vehicleIdentifyType"].toInt();
    if (aMap.contains("vehplate"))
        vehPlate = aMap["vehplate"].toString();
    if (aMap.contains("cardId"))
        cardId = aMap["cardId"].toString();
    if (aMap.contains("judgeTime"))
        judgeTime = aMap["judgeTime"].toInt();

    QVariantList trades = GM_INS->m_ds->fetchSuccessedTrades(vehicleIdentifyType, vehPlate, cardId, dataType, judgeTime);

    QVariantMap resMap;
    if (trades.isEmpty()) {
        resMap.insert("judgeResult", 0);
    } else {
        resMap.insert("judgeResult", 1);

        QVariantList judgeDetails;
        for (const auto &trade : trades) {
            QVariantMap detail;
            QVariantMap aMap = trade.toMap();

            detail["tradeId"] = aMap["TRADEID"].toString();
            detail["laneID"] = aMap["LANEID"].toString();
            detail["tradeTime"] = aMap["TRADETIME"].toString();
            detail["vehplate"] = aMap["VEHPLATE"].toString();
            detail["cardId"] = aMap["CARDID"].toString();
            detail["fee"] = aMap["FEE"].toString();
            judgeDetails.append(detail);
        }
        resMap.insert("judgeDetail", judgeDetails);
    }

    QString dealtData = DataDealUtils::mapToJson(resMap);
    return dealtData;
}
