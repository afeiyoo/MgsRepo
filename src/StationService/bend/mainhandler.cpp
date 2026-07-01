#include "mainhandler.h"

#include <QJsonDocument>
#include <QVariantMap>

#include "Logger.h"
#include "config/config.h"
#include "core/baseexception.h"
#include "core/globalmanager.h"
#include "dbs/dataservice.h"
#include "utils/configutils.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"

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
        dealtData = dealQueryShift(aMap);
    } else if (queryType.contains("queryData")) {
        dealtData = dealQueryData(aMap);
    } else if (queryType.contains("queryETCBlack")) {
        dealtData = dealQueryETCBlack(aMap);
    } else {
        throw BaseException(1, QString("未知查询类型%1").arg(queryType));
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

QString MainHandler::dealQueryShift(const QVariantMap &aMap) const
{
    QString shiftDate;
    int shiftID = 0;
    int laneID = 0;
    int flag = 0;

    if (aMap.contains("shiftDate"))
        shiftDate = aMap["shiftDate"].toString();
    if (aMap.contains("shiftId"))
        shiftID = aMap["shiftId"].toInt();
    else if (aMap.contains("shiftID"))
        shiftID = aMap["shiftID"].toInt();
    if (aMap.contains("laneId"))
        laneID = aMap["laneId"].toInt();
    else if (aMap.contains("laneID"))
        laneID = aMap["laneID"].toInt();
    if (aMap.contains("flag"))
        flag = aMap["flag"].toInt();

    if (shiftDate.isEmpty())
        throw BaseException(1, "shiftDate值异常，dealQueryShift执行失败");
    if (shiftID == 0)
        throw BaseException(1, "shiftId值异常，dealQueryShift执行失败");
    if (flag != 1 && flag != 2)
        throw BaseException(1, "flag值异常，dealQueryShift执行失败");
    if (laneID == 0)
        throw BaseException(1, "laneId值异常，dealQueryShift执行失败");

    int cnt = GM_INS->m_ds->fetchShiftCnt(shiftDate, shiftID, laneID, flag);

    QVariantMap resMap;
    if (cnt < 0) {
        resMap["errCode"] = 1;
        resMap["Count"] = 0;
        resMap["errorMessage"] = "dealQueryShift执行异常";
    } else {
        resMap["errCode"] = 0;
        resMap["Count"] = cnt;
        resMap["errorMessage"] = "";
    }

    return DataDealUtils::mapToJson(resMap);
}

QString MainHandler::dealQueryData(const QVariantMap &aMap) const
{
    if (GM_INS->m_conf->m_queryAuthType == 1) {
        QString authStr = aMap["queryAuth"].toString();
        if (authStr.toUpper() != DataDealUtils::cryptoMD5("fjeit" + DataDealUtils::curDateStr("yyyyMMdd")))
            throw BaseException(1, "MD5授权校验未通过");
    } else if (GM_INS->m_conf->m_queryAuthType == 2) {
        if (!GM_INS->m_conf->m_queryAuthIP.contains(m_peerIP))
            throw BaseException(1, "IP授权校验未通过");
    }

    int dataType = 0;
    QString sql;
    if (aMap.contains("querySql"))
        sql = aMap["querySql"].toString();
    if (aMap.contains("dataType"))
        dataType = aMap["dataType"].toInt();

    if (sql.isEmpty())
        throw BaseException(1, "querySql为空，dealQueryData执行失败");
    if (dataType < 1 || dataType > 4)
        throw BaseException(1, "未知dataType类型，dealQueryData执行失败");

    int errCode = 0;
    QString errorMessage;
    QVariant data;
    if (dataType == 1) {
        QVariantMap ans = GM_INS->m_ds->queryMap(sql);
        data = ans;
        if (ans.isEmpty()) {
            errCode = 1;
            errorMessage = "queryMap查询为空";
        }
    } else if (dataType == 2) {
        int ans = GM_INS->m_ds->queryInt(sql);
        data = ans;
        if (ans < 0) {
            errCode = 1;
            errorMessage = "queryInt查询为空";
        }
    } else if (dataType == 3) {
        QString ans = GM_INS->m_ds->queryString(sql);
        data = ans;
        if (ans.isEmpty()) {
            errCode = 1;
            errorMessage = "queryString查询为空";
        }
    } else if (dataType == 4) {
        QVariantList ans = GM_INS->m_ds->queryList(sql);
        data = ans;
        if (ans.isEmpty()) {
            errCode = 1;
            errorMessage = "queryList查询为空";
        }
    }

    QVariantMap resMap;
    resMap["errCode"] = errCode;
    resMap["dataType"] = dataType;
    resMap["errorMessage"] = errorMessage;
    resMap["data"] = data;

    return DataDealUtils::mapToJson(resMap);
}

QString MainHandler::dealQueryETCBlack(const QVariantMap &aMap) const
{
    QString version;
    if (aMap.contains("version"))
        version = aMap["version"].toString();

    if (version.isEmpty())
        throw BaseException(1, "version值为空，dealQueryETCBlack执行失败");

    QString dealtData;
    QString res = checkETCBlackInfo(version);
    return res;
}

QString MainHandler::checkETCBlackInfo(const QString &version) const
{
    QString filePath = QString("%1/download/%2/%3_queryETCBlack.json").arg(FileUtils::curApplicationDirPath(), version.left(8), version);

    int queryRes = 0;
    QString result;
    FileName file = FileName::fromString(filePath);
    if (!file.exists()) {
        QString curVersion = GM_INS->getCurBlackVersion();
        if (curVersion < version) {
            queryRes = 2;
        } else {
            queryRes = 0;
        }
    } else {
        FileReader reader;
        QString errStr;
        if (!reader.fetch(file.toString(), &errStr)) {
            throw BaseException(1, QString("读取文件%1失败: %2").arg(file.fileName(), errStr));
        }

        QString jsonData = reader.data();
        if (jsonData.isEmpty()) {
            queryRes = 4;
        } else { // 文件内容存在，则直接读取文件内容返回
            return jsonData;
        }
    }

    QVariantMap resMap;
    resMap["queryResult"] = queryRes;
    resMap["version"] = version;
    resMap["operateTable"] = 0;
    resMap["amount"] = 0;

    return DataDealUtils::mapToJson(resMap);
}
