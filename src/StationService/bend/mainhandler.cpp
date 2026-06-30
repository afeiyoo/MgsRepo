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

MainHandler::~MainHandler() {}

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
        dealtData = dealSaveData(aMap);
    } else if (queryType.contains("queryXZPass")) {
        dealtData = dealQueryXZPass(aMap);
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
            QVariantMap oneMap = trade.toMap();
            detail["tradeId"] = oneMap["TRADEID"].toString();
            detail["laneID"] = oneMap["LANEID"].toString();
            detail["tradeTime"] = oneMap["TRADETIME"].toString();
            detail["vehplate"] = oneMap["VEHPLATE"].toString();
            detail["cardId"] = oneMap["CARDID"].toString();
            detail["fee"] = oneMap["FEE"].toString();

            judgeDetails.append(detail);
        }
        resMap.insert("judgeDetail", judgeDetails);
    }

    return DataDealUtils::mapToJson(resMap);
}

QString MainHandler::dealSaveData(const QVariantMap &aMap) const
{
    QString tableName;
    QString tableKey;
    QVariantMap kvs;
    if (aMap.contains("saveTable"))
        tableName = aMap["saveTable"].toString();
    if (aMap.contains("tableKey"))
        tableKey = aMap["tableKey"].toString();
    if (aMap.contains("saveData"))
        kvs = aMap["saveData"].toMap();

    if (tableName.isEmpty())
        throw BaseException(1, "saveTable为空，dealSaveData执行失败");
    if (tableKey.isEmpty())
        throw BaseException(1, "tableKey为空，dealSaveData执行失败");
    if (kvs.isEmpty())
        throw BaseException(1, "saveData为空，dealSaveData执行失败");

    int cnt = GM_INS->m_ds->fetchRecordCnt(kvs, tableName, tableKey);
    if (cnt < 0)
        throw BaseException(1, "查询记录是否已存在时，发生异常");

    QString successMessage;
    if (cnt > 0) { // 记录已存在，执行更新
        LOG_INFO().noquote() << "记录已存在，执行更新";
        bool res = GM_INS->m_ds->updateRecord(kvs, tableName, tableKey);
        if (!res)
            throw BaseException(1, "更新记录失败");

        successMessage = "更新记录成功";
    } else { // 记录不存在，执行插入
        LOG_INFO().noquote() << "记录不存在，执行插入";
        bool res = GM_INS->m_ds->insertRecord(kvs, tableName);
        if (!res)
            throw BaseException(1, "插入记录失败");

        successMessage = "插入记录成功";
    }

    QVariantMap resMap;
    resMap["errCode"] = 0;
    resMap["errorMessage"] = successMessage;
    return DataDealUtils::mapToJson(resMap);
}

QString MainHandler::dealQueryXZPass(const QVariantMap &aMap) const
{
    QString querySql;
    if (aMap.contains("querySql"))
        querySql = aMap["querySql"].toString();

    if (querySql.isEmpty())
        throw BaseException(1, "querySql为空，dealQueryXZPass执行失败");

    int cnt = GM_INS->m_ds->fetchXZPassTimes(querySql);
    if (cnt < 0)
        throw BaseException(1, "查询厦漳大桥通行趟次时，发生异常");

    QVariantMap resMap;
    if (cnt > 0) {
        resMap["errCode"] = 0;
        resMap["errorMessage"] = QString::number(cnt);
    } else {
        resMap["errCode"] = 1;
        resMap["errorMessage"] = "未查询到相关记录";
    }
    return DataDealUtils::mapToJson(resMap);
}
