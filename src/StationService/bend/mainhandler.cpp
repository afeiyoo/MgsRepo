#include "mainhandler.h"

#include <QJsonDocument>
#include <QVariantMap>

#include "Logger.h"
#include "core/baseexception.h"
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

    if (queryType.contains("queryRepeat")) {
    } else if (queryType.contains("saveData")) {
    } else if (queryType.contains("queryXZPass")) {
    } else if (queryType.contains("queryShift")) {
    } else if (queryType.contains("queryData")) {
    } else {
    }
}

QString MainHandler::dealQueryRepeat(const QVariantMap &aMap)
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
}
