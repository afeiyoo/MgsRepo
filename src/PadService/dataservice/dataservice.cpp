#include "dataservice.h"

#include "HttpClient/src/http.h"
#include "Logger.h"
#include "NlohmannJson/nlojson.hpp"
#include "bean/t_discardticketreview.h"
#include "bean/t_specialcards.h"
#include "core/baseexception.h"
#include "core/globalmanager.h"
#include "utils/datadealutils.h"

DataService::DataService() {}

DataService::~DataService() {}

int DataService::init(const QString &dbType, const QString &driver, const QString &userName, const QString &passWord, const QString &dbName)
{
    QString connectionString = QString("Driver={%1};DBQ=%2;UID=%3;PWD=%4").arg(driver, dbName, userName, passWord);
    EasyQtSql::SqlFactory::DBSetting setting(dbType, connectionString);

    m_dbFactory = EasyQtSql::SqlFactory::getInstance()->config(setting, "oracle");

    return testConnection();
}

bool DataService::getLatestOutTradeInPlate(const QString &vehPlate, QObject *obj, int type)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(
        R"(SELECT * FROM %1 WHERE exvehplate = ? AND extime = (SELECT max(extime) FROM (SELECT max(extime) AS extime FROM t_etc_out WHERE exvehplate = ? UNION SELECT max(extime) AS extime FROM t_mtc_out WHERE exvehplate =?) t))");

    if (type == 0) {
        sql = sql.arg("t_etc_out");
    } else {
        sql = sql.arg("t_mtc_out");
    }

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(vehPlate, vehPlate, vehPlate);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return false;

        res.fetchObject(*obj);
        return true;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return false;
    }
}

bool DataService::getLatestOutTradeInTradeID(const QString &tradeId, QObject *obj, int type)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(
        R"(SELECT * FROM %1 WHERE tradeid = ? AND extime = (SELECT max(extime) FROM (SELECT max(extime) AS extime FROM t_etc_out WHERE tradeid = ? UNION SELECT max(extime) AS extime FROM t_mtc_out WHERE tradeid =?) t))");

    if (type == 0) {
        sql = sql.arg("t_etc_out");
    } else {
        sql = sql.arg("t_mtc_out");
    }

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(tradeId, tradeId, tradeId);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return false;

        res.fetchObject(*obj);
        return true;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return false;
    }
}

QVariantMap DataService::getEnInfo(const QString &passID)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(
        R"(SELECT tradeid, TO_CHAR(entime, 'yyyy-mm-dd HH24:mi:ss') AS entime, cnlaneid, enstation, stationname FROM t_mtc_in WHERE passid = ? UNION ALL SELECT tradeid, TO_CHAR(entime, 'yyyy-mm-dd HH24:mi:ss') AS entime, cnlaneid, enstation, stationname FROM t_etc_in WHERE passid = ?)");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(passID, passID);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return {};

        QVariantMap resMap = res.toMap();
        return resMap;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return {};
    }
}

QList<QVariantMap> DataService::getGantryInfos(const QString &passId)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(
        R"(SELECT g.gantryid AS gantryid, e.tradetime AS tradetime, e.flagid AS flagid, g.flagname AS flagname, e.errcode FROM t_ff_etcTrade e, t_etcflag g WHERE e.passid = ? AND e.flagid = g.flagid UNION ALL SELECT g.gantryid AS gantryid, e.tradetime AS tradetime, e.flagid AS flagid, g.flagname AS flagname, e.errcode FROM t_ff_cpcrecord e, t_etcflag g WHERE e.passid = ? AND e.flagid = g.flagid ORDER BY tradetime)");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(passId, passId);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        QList<QVariantMap> records;
        while (res.next()) {
            QVariantMap record = res.toMap();
            records.append(record);
        }

        return records;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return {};
    }
}

QString DataService::getGantryNodeID(const QString &nodeHex)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(R"(SELECT nodeid FROM t_cnfeenode WHERE nodetype = 1 AND hexnode = ?)");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(nodeHex);

        if (!res.next())
            return "";

        QString data = res.value("NODEID").toString();
        return data;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return "";
    }
}

QString DataService::getGantryNodeName(const QString &nodeId)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(R"(SELECT nodename FROM t_cnfeenode WHERE nodetype = 1 AND nodeid = ?)");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(nodeId);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return "";

        QString data = res.value("NODENAME").toString();
        return data;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return "";
    }
}

QString DataService::getGantryHexNode(const QString &nodeId)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(R"(SELECT hexnode FROM t_cnfeenode WHERE nodetype = 1 AND nodeid = ?)");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(nodeId);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return "";

        QString data = res.value("HEXNODE").toString();
        return data;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return "";
    }
}

bool DataService::getSplitOut(const QString &tradeId, QObject *obj)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(R"(SELECT * FROM t_split_out WHERE tradeid = ? AND provincenum = 35)");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(tradeId);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return false;

        res.fetchObject(*obj);
        return true;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return false;
    }
}

QString DataService::getUserID(const QString &cardId)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(R"(SELECT userid FROM t_user WHERE isvalid = 1 AND idtcardnum = ?)");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(cardId);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return "";

        QString data = res.value("USERID").toString();
        return data;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return "";
    }
}

QString DataService::getUserName(const QString &param, int type)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(R"(SELECT username FROM t_user WHERE isvalid = 1 AND %1 = ?)");

    if (type == 0) {
        sql = sql.arg("idtcardnum");
    } else {
        sql = sql.arg("userid");
    }

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(param);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return "";

        QString data = res.value("USERNAME").toString();
        return data;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return "";
    }
}

QString DataService::getStationIP(const QString &stationId)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(R"(SELECT ipaddr FROM t_station WHERE stationid = ? AND ipaddr NOT IN ('0000', '0.0.0.0'))");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(stationId);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return "";

        QString data = res.value("IPADDR").toString();
        return data;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return "";
    }
}

QString DataService::getStationName(const QString &nodeId)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(R"(SELECT nodename FROM t_cnfeenode WHERE hexnode = ?)");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(nodeId);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return "";

        QString data = res.value("NODENAME").toString();
        return data;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return "";
    }
}

QString DataService::getLaneIP(const QString &stationId, int laneId)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(R"(SELECT ipaddr FROM t_lane WHERE stationid = ? AND laneid = ? AND ipaddr NOT IN ('0000', '0.0.0.0'))");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(stationId, laneId);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return "";

        QString data = res.value("IPADDR").toString();
        return data;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return "";
    }
}

QString DataService::getGrayCardRemark(const QString &cardId)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(R"(SELECT remark FROM t_graycard  WHERE cardid = ? AND isvalid = 1)");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(cardId);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return "";

        QString data = res.value("REMARK").toString();
        return data;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return "";
    }
}

QString DataService::getGrayVehicleRemark(const QString &plate)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(R"(SELECT remark FROM t_grayvehicle  WHERE vehplate = ? AND isvalid = 1)");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(plate);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return "";

        QString data = res.value("REMARK").toString();
        return data;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return "";
    }
}

int DataService::getGreenPassBanType(const QString &plate)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(R"(SELECT bantype FROM t_greenpassban WHERE vehplate = ? AND isvalid = 1 AND CURRENT_TIMESTAMP BETWEEN starttime AND endtime)");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(plate);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return -1;

        int data = res.value("BANTYPE").toInt();
        return data;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return -1;
    }
}

bool DataService::getGreenPassAppointment(const QString &plate)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(R"(SELECT count(*) as count FROM t_appointment WHERE vehicleid = ?)");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(plate);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return false;

        int data = res.value("COUNT").toInt();
        return data > 0;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return false;
    }
}

bool DataService::getSpecialCardExist(const T_SpecialCards &specialCard, QUrl url)
{
    QVariantMap sendMap;
    sendMap["queryType"] = "queryData";
    sendMap["queryAuth"] = "1";
    sendMap["querySql"] = QString(
                              "SELECT COUNT(*) AS cnt FROM t_specialcards WHERE shiftdate = '%1' AND laneid = %2 AND shiftid = %3 AND cardid = '%4'")
                              .arg(specialCard.ShiftDate.toString("yyyy-MM-dd hh:mm:ss"))
                              .arg(specialCard.LaneID)
                              .arg(specialCard.ShiftID)
                              .arg(specialCard.CardID);
    sendMap["dataType"] = 4;

    NloJson nloJson;

    QString sendData = nloJson.serialize(sendMap);
    LOG_INFO().noquote() << QString("查询特情卡 %1 记录是否存在: ").arg(specialCard.CardID) << sendData.left(1024);

    QVariantList records;
    QByteArray result;
    Http client;
    bool ok = client.postSync(result, url, sendData.toUtf8(), "application/json");
    if (ok) {
        LOG_INFO().noquote() << "返回特情卡 %1 记录存在查询结果: " << result.left(1024);
        QVariant parsed = nloJson.parse(result);
        if (!parsed.isValid() || !parsed.canConvert<QVariantMap>()) {
            throw BaseException(1, QString("响应失败: 站级服务响应数据异常!"));
        }

        QVariantMap resMap = parsed.toMap();
        if (resMap["errCode"].toInt() == 1) {
            QString errorMessage = resMap["errorMessage"].toString();
            throw BaseException(1, QString("响应失败: 站级服务返回失败 %1").arg(errorMessage));
        } else {
            records = resMap["data"].toList();
        }
    } else {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    if (records.isEmpty()) {
        throw BaseException(1, QString("响应失败: 特请卡记录查询为空"));
    }

    QVariantMap resMap = records.first().toMap();
    return resMap["cnt"].toInt() > 0;
}

bool DataService::insertSpecialCard(const T_SpecialCards &specialCard, QUrl url)
{
    QVariantMap map;
    map["dataid"] = specialCard.DataID;
    map["shiftdate"] = specialCard.ShiftDate.toString("yyyy-MM-dd hh:mm:ss");
    map["shiftid"] = specialCard.ShiftID;
    map["laneid"] = specialCard.LaneID;
    map["operator"] = specialCard.Operator;
    map["operatorname"] = specialCard.OperatorName;
    map["inputoperator"] = specialCard.InputOperator;
    map["inputoperatorname"] = specialCard.InputOperatorName;
    map["cardid"] = specialCard.CardID;
    map["specialtype"] = specialCard.SpecialType;
    map["dealdesc"] = specialCard.DealDesc;
    map["remark"] = specialCard.Remark;
    map["updatetime"] = specialCard.UpdateTime.toString("yyyy-MM-dd hh:mm:ss");

    QVariantMap sendMap;
    sendMap["queryType"] = "saveData";
    sendMap["saveTable"] = "t_specialcards";
    sendMap["tableKey"] = "dataid";
    sendMap["saveData"] = map;

    NloJson nloJson;

    QString sendData = nloJson.serialize(sendMap);
    LOG_INFO().noquote() << QString("请求插入特情卡 %1 信息: ").arg(specialCard.CardID) << sendData.left(1024);

    bool isSuccessful = false;
    QByteArray result;
    Http client;
    bool ok = client.postSync(result, url, sendData.toUtf8(), "application/json");
    if (ok) {
        LOG_INFO().noquote() << QString("插入特情卡 %1 信息结果返回: ").arg(specialCard.CardID) << result.left(1024);
        QVariant parsed = nloJson.parse(result);
        if (!parsed.isValid() || !parsed.canConvert<QVariantMap>()) {
            throw BaseException(1, QString("响应失败: 站级服务响应数据异常!"));
        }

        QVariantMap resMap = parsed.toMap();
        if (resMap["errCode"].toInt() == 1) {
            QString errorMessage = resMap["errorMessage"].toString();
            throw BaseException(1, QString("响应失败: 站级服务返回失败 %1").arg(errorMessage));
        } else {
            isSuccessful = true;
        }
    } else {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    return isSuccessful;
}

QVariantList DataService::getFreeTempVehicles(const QString &plate)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(R"(SELECT * FROM t_freetempvehicle WHERE vehplate = ? AND isvalid = 1)");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(plate);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        QVariantList records;
        while (res.next()) {
            QVariantMap record = res.toMap();
            records.append(record);
        }
        return records;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return {};
    }
}

QString DataService::getEmgcSeqNum(const QString &stationId)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(R"(SELECT kitem FROM t_emgcdict WHERE stationid = ? AND laneid = 0 AND ktype = 1)");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(stationId);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return "0";

        QString data = res.value("KITEM").toString();
        return data;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return "";
    }
}

bool DataService::updateEmgcSeqNum(const QString &stationId)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql = QString("UPDATE t_emgcdict SET kitem = kitem+1 WHERE stationid = %1 AND laneid = 0 AND ktype = 1").arg(stationId);

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::NonQueryResult res = t.execNonQuery(sql);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        return t.commit();
    } catch (EasyQtSql::DBException &e) {
        t.rollback();
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return false;
    }
}

bool DataService::insertEmgcSeqNum(const QString &stationId)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::NonQueryResult res
            = t.insertInto("t_emgcdict (stationid, laneid, ktype, kitem, kvalueint, kvaluestring)").values(stationId, 0, 1, "1", 0, "").exec();

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        return t.commit();
    } catch (EasyQtSql::DBException &e) {
        t.rollback();
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return false;
    }
}

QVariantMap DataService::getTicketUseInfo(int laneId, const QString &ticketNum, QUrl url)
{
    QVariantMap sendMap;
    sendMap["queryType"] = "queryData";
    sendMap["queryAuth"] = "1";
    sendMap["querySql"] = QString(
                              "SELECT TOP 1 * FROM t_ticketusemanage WHERE laneid = %1 AND isused = 1 AND lastnum != stopnum AND startnum <= '%2' "
                              "AND stopnum >= '%3' AND remark LIKE '%SPT-POS:30%' ORDER BY dataid")
                              .arg(laneId)
                              .arg(ticketNum, ticketNum);
    sendMap["dataType"] = 4;

    NloJson nloJson;

    QString sendData = nloJson.serialize(sendMap);
    LOG_INFO().noquote() << "查询相关票据信息: " << sendData.left(1024);

    QVariantList records;
    QByteArray result;
    Http client;
    bool ok = client.postSync(result, url, sendData.toUtf8(), "application/json");
    if (ok) {
        LOG_INFO().noquote() << "相关票据信息查询结果: " << result.left(1024);
        QVariant parsed = nloJson.parse(result);
        if (!parsed.isValid() || !parsed.canConvert<QVariantMap>()) {
            throw BaseException(1, QString("响应失败: 站级服务响应数据异常!"));
        }

        QVariantMap resMap = parsed.toMap();
        if (resMap["errCode"].toInt() == 1) {
            QString errorMessage = resMap["errorMessage"].toString();
            throw BaseException(1, QString("响应失败: 站级服务返回失败 %1").arg(errorMessage));
        } else {
            records = resMap["data"].toList();
        }
    } else {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    if (records.isEmpty()) {
        throw BaseException(1, QString("响应失败: 未查询到相关票据信息"));
    }

    QVariantMap resMap = records.first().toMap();
    return resMap;
}

bool DataService::updateTicketUseInfo(int dataId, int newSeqNum, QUrl url)
{
    QVariantMap map;
    map["dataid"] = dataId;
    map["lastnum"] = newSeqNum;

    QVariantMap sendMap;
    sendMap["queryType"] = "saveData";
    sendMap["saveTable"] = "t_ticketusemanage";
    sendMap["tableKey"] = "dataid";
    sendMap["saveData"] = map;

    NloJson nloJson;

    QString sendData = nloJson.serialize(sendMap);
    LOG_INFO().noquote() << QString("请求更新 DataID %1 的票据段信息: ").arg(newSeqNum) << sendData.left(1024);

    bool isSuccessful = false;
    QByteArray result;
    Http client;
    bool ok = client.postSync(result, url, sendData.toUtf8(), "application/json");
    if (ok) {
        LOG_INFO().noquote() << QString("更新 DataID %1 的票据段信息返回结果: ").arg(newSeqNum) << result.left(1024);
        QVariant parsed = nloJson.parse(result);
        if (!parsed.isValid() || !parsed.canConvert<QVariantMap>()) {
            throw BaseException(1, QString("响应失败: 站级服务响应数据异常!"));
        }

        QVariantMap resMap = parsed.toMap();
        if (resMap["errCode"].toInt() == 1) {
            QString errorMessage = resMap["errorMessage"].toString();
            throw BaseException(1, QString("响应失败: 站级服务返回失败 %1").arg(errorMessage));
        } else {
            isSuccessful = true;
        }
    } else {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    return isSuccessful;
}

QVariantList DataService::getDurationOutTrades(const QString &vehPlate, const QString &cardId, const QString &startTime, const QString &stopTime,
                                               QUrl url)
{
    QString startShiftDate = QDateTime::fromString(startTime, "yyyy-MM-dd HH:mm:ss").date().toString("yyyy-MM-dd");
    QString stopShiftDate = QDateTime::fromString(stopTime, "yyyy-MM-dd HH:mm:ss").date().toString("yyyy-MM-dd");

    QVariantMap sendMap;
    sendMap["queryType"] = "queryData";
    sendMap["queryAuth"] = "1";
    sendMap["querySql"] = QString("SELECT tradeid, passid, exvehplate, cardid, enstationname, exstationname, factpay, extime, ticketnum, "
                                  "exvehclass, paytype, mershouldpay, cardtype FROM t_mtc_out WHERE paytype = 0 AND isvalid = 1 AND (exvehplate = "
                                  "'%1' OR cardid = '%2') AND exshiftdate >= '%3' AND exshiftdate <= '%4' AND extime >= '%5' AND extime <= '%6'"
                                  " UNION ALL SELECT tradeid, passid, exvehplate, cardid, enstationname, exstationname, factpay, extime, ticketnum, "
                                  "exvehclass, paytype, mershouldpay, cardtype FROM t_etc_out WHERE (paytype = 2 OR paytype = 0)  AND isvalid = 1 "
                                  "AND (exvehplate = '%7' OR cardid = '%8') AND exshiftdate >= '%9' AND exshiftdate <= '%10' AND extime >= '%11' AND "
                                  "extime <= '%12'")
                              .arg(vehPlate, cardId, startShiftDate, stopShiftDate, startTime, stopTime)
                              .arg(vehPlate, cardId, startShiftDate, stopShiftDate, startTime, stopTime);
    sendMap["dataType"] = 4;

    NloJson nloJson;

    QString sendData = nloJson.serialize(sendMap);
    LOG_INFO().noquote() << QString("%1 在 %2~%3 内的第三方支付与现金支付流水查询请求: ").arg(vehPlate, startTime, stopTime) << sendData;

    QVariantList records;
    QByteArray result;
    Http client;
    bool ok = client.postSync(result, url, sendData.toUtf8(), "application/json");
    if (ok) {
        LOG_INFO().noquote() << "返回相关流水 TradeId 查询结果: " << result.left(1024);
        QVariant parsed = nloJson.parse(result);
        if (!parsed.isValid() || !parsed.canConvert<QVariantMap>()) {
            throw BaseException(1, QString("响应失败: 站级服务响应数据异常!"));
        }

        QVariantMap resMap = parsed.toMap();
        if (resMap["errCode"].toInt() == 1) {
            QString errorMessage = resMap["errorMessage"].toString();
            throw BaseException(1, QString("响应失败: 站级服务返回失败 %1").arg(errorMessage));
        } else {
            records = resMap["data"].toList();
        }
    } else {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    return records;
}

QVariantList DataService::getTicketUseScrapInfos(const QStringList &tradeIds, QUrl url)
{
    if (tradeIds.isEmpty())
        return {};

    QStringList quotedIds;
    for (const QString &tid : tradeIds)
        quotedIds << QString("'%1'").arg(tid);
    QString inClause = quotedIds.join(", ");

    QVariantMap sendMap;
    sendMap["queryType"] = "queryData";
    sendMap["queryAuth"] = "1";
    sendMap["querySql"] = QString("SELECT a.dataid, a.tradeid, a.startnum, CASE WHEN b.tradeid IS NOT NULL THEN 1 ELSE 0 END AS "
                                  "isscrapticket FROM t_mticketuse AS a LEFT JOIN t_scrapticket AS b ON a.tradeid = b.tradeid AND "
                                  "a.startnum = b.ticketnum WHERE a.tradeid in (%1) AND a.isvalid = 1 AND a.startnum = a.endnum")
                              .arg(inClause);
    sendMap["dataType"] = 4;

    NloJson nloJson;

    QString sendData = nloJson.serialize(sendMap);
    LOG_INFO().noquote() << "查询相关废票 scrapTickets 信息: " << sendData.left(1024);

    QVariantList records;
    QByteArray result;
    Http client;
    bool ok = client.postSync(result, url, sendData.toUtf8(), "application/json");
    if (ok) {
        LOG_INFO().noquote() << "返回相关废票 scrapTickets 信息查询结果: " << result.left(1024);
        QVariant parsed = nloJson.parse(result);
        if (!parsed.isValid() || !parsed.canConvert<QVariantMap>()) {
            throw BaseException(1, QString("响应失败: 站级服务响应数据异常!"));
        }

        QVariantMap resMap = parsed.toMap();
        if (resMap["errCode"].toInt() == 1) {
            QString errorMessage = resMap["errorMessage"].toString();
            throw BaseException(1, QString("响应失败: 站级服务返回失败 %1").arg(errorMessage));
        } else {
            records = resMap["data"].toList();
        }
    } else {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }
    return records;
}

QVariantList DataService::getDiscardTickets(const QStringList &tradeIds, QUrl url)
{
    if (tradeIds.isEmpty())
        return {};

    QStringList quotedIds;
    for (const QString &tid : tradeIds)
        quotedIds << QString("'%1'").arg(tid);
    QString inClause = quotedIds.join(", ");

    QVariantMap sendMap;
    sendMap["queryType"] = "queryData";
    sendMap["queryAuth"] = "1";
    sendMap["querySql"] = QString("SELECT tradeid, ticketnum, flag FROM t_discardticket WHERE tradeid IN (%1)").arg(inClause);
    sendMap["dataType"] = 4;

    NloJson nloJson;

    QString sendData = nloJson.serialize(sendMap);
    LOG_INFO().noquote() << "查询相关弃票 discardTickets 信息: " << sendData.left(1024);

    QVariantList records;
    QByteArray result;
    Http client;
    bool ok = client.postSync(result, url, sendData.toUtf8(), "application/json");
    if (ok) {
        LOG_INFO().noquote() << "返回相关弃票 discardTickets 信息查询结果: " << result.left(1024);
        QVariant parsed = nloJson.parse(result);
        if (!parsed.isValid() || !parsed.canConvert<QVariantMap>()) {
            throw BaseException(1, QString("响应失败: 站级服务响应数据异常!"));
        }

        QVariantMap resMap = parsed.toMap();
        if (resMap["errCode"].toInt() == 1) {
            QString errorMessage = resMap["errorMessage"].toString();
            throw BaseException(1, QString("响应失败: 站级服务返回失败 %1").arg(errorMessage));
        } else {
            records = resMap["data"].toList();
        }
    } else {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    return records;
}

int DataService::getOutShiftSettleCount(const QString &stationId, const QString &shiftDate, int shiftId, QUrl url)
{
    QVariantMap sendMap;
    sendMap["queryType"] = "queryData";
    sendMap["queryAuth"] = "1";
    sendMap["querySql"] = QString("SELECT COUNT(*) AS cnt FROM t_outshiftsettle WHERE shiftdate = '%1' AND laneid = 249 AND shiftid = %2 AND isvalid "
                                  "= 1 AND stationid = '%3'")
                              .arg(shiftDate)
                              .arg(shiftId)
                              .arg(stationId);
    sendMap["dataType"] = 4;

    NloJson nloJson;

    QString sendData = nloJson.serialize(sendMap);
    LOG_INFO().noquote() << "查询相关班次信息条数: " << sendData.left(1024);

    QVariantList records;
    QByteArray result;
    Http client;
    bool ok = client.postSync(result, url, sendData.toUtf8(), "application/json");
    if (ok) {
        LOG_INFO().noquote() << "返回相关班次信息条数查询结果: " << result.left(1024);
        QVariant parsed = nloJson.parse(result);
        if (!parsed.isValid() || !parsed.canConvert<QVariantMap>()) {
            throw BaseException(1, QString("响应失败: 站级服务响应数据异常!"));
        }

        QVariantMap resMap = parsed.toMap();
        if (resMap["errCode"].toInt() == 1) {
            QString errorMessage = resMap["errorMessage"].toString();
            throw BaseException(1, QString("响应失败: 站级服务返回失败 %1").arg(errorMessage));
        } else {
            records = resMap["data"].toList();
        }
    } else {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    if (records.isEmpty()) {
        throw BaseException(1, QString("响应失败: 未查询到相关班次信息"));
    }

    QVariantMap resMap = records.first().toMap();
    return resMap["cnt"].toInt();
}

bool DataService::insertOutShiftSettle(const QString &dataId, const QString &shiftDate, int shiftId, const QString &stationId,
                                       const QString &operatorId, const QString &operatorName, QUrl url)
{
    QVariantMap map;
    map["dataid"] = dataId;
    map["stationid"] = stationId;
    map["laneid"] = 249;
    map["operator"] = operatorId;
    map["operatorname"] = operatorName;
    map["shiftid"] = shiftId;
    map["shiftdate"] = shiftDate;
    map["finoperator"] = "0000000";
    map["isvalid"] = 1;
    map["nextoperator"] = "0000000";

    QVariantMap sendMap;
    sendMap["queryType"] = "saveData";
    sendMap["saveTable"] = "t_outshiftsettle";
    sendMap["tableKey"] = "recordid";
    sendMap["saveData"] = map;

    NloJson nloJson;

    QString sendData = nloJson.serialize(sendMap);
    LOG_INFO().noquote() << "请求插入工班信息: " << sendData.left(1024);

    bool isSuccessful = false;
    QByteArray result;
    Http client;
    bool ok = client.postSync(result, url, sendData.toUtf8(), "application/json");
    if (ok) {
        LOG_INFO().noquote() << "插入工班信息返回结果: " << result.left(1024);
        QVariant parsed = nloJson.parse(result);
        if (!parsed.isValid() || !parsed.canConvert<QVariantMap>()) {
            throw BaseException(1, QString("响应失败: 站级服务响应数据异常!"));
        }

        QVariantMap resMap = parsed.toMap();
        if (resMap["errCode"].toInt() == 1) {
            QString errorMessage = resMap["errorMessage"].toString();
            throw BaseException(1, QString("响应失败: 站级服务返回失败 %1").arg(errorMessage));
        } else {
            isSuccessful = true;
        }
    } else {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    return isSuccessful;
}

bool DataService::insertTicketReviewPic(const T_DiscardTicketReview &review, QUrl url)
{
    QVariantMap map;
    map["stationid"] = review.StationID;
    map["tradeid"] = review.TradeID;
    map["flag"] = review.Flag;
    map["picdata"] = review.PicData;
    map["operatetime"] = review.OperateTime.toString("yyyy-MM-dd hh:mm:ss");

    QVariantMap sendMap;
    sendMap["queryType"] = "saveData";
    sendMap["saveTable"] = "t_discardticketreview";
    sendMap["tableKey"] = "tradeid, flag";
    sendMap["saveData"] = map;

    NloJson nloJson;

    QString sendData = nloJson.serialize(sendMap);
    LOG_INFO().noquote() << QString("请求插入交易号 %1 的稽核图片: ").arg(review.TradeID) << sendData.left(1024);

    bool isSuccessful = false;
    QByteArray result;
    Http client;
    bool ok = client.postSync(result, url, sendData.toUtf8(), "application/json");
    if (ok) {
        LOG_INFO().noquote() << QString("插入交易号 %1 的稽核图片返回结果: ").arg(review.TradeID) << result.left(1024);
        QVariant parsed = nloJson.parse(result);
        if (!parsed.isValid() || !parsed.canConvert<QVariantMap>()) {
            throw BaseException(1, QString("响应失败: 站级服务响应数据异常!"));
        }

        QVariantMap resMap = parsed.toMap();
        if (resMap["errCode"].toInt() == 1) {
            QString errorMessage = resMap["errorMessage"].toString();
            throw BaseException(1, QString("响应失败: 站级服务返回失败 %1").arg(errorMessage));
        } else {
            isSuccessful = true;
        }
    } else {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    return isSuccessful;
}

QVariantList DataService::getTicketReviewPic(const QString &stationId, const QString &tradeID, QUrl url)
{
    QVariantMap sendMap;
    sendMap["queryType"] = "queryData";
    sendMap["queryAuth"] = "1";
    sendMap["querySql"] = QString("SELECT picdata FROM t_discardticketreview WHERE stationid = '%1' AND tradeid = '%2' AND flag = 0")
                              .arg(stationId, tradeID);
    sendMap["dataType"] = 4;

    NloJson nloJson;

    QString sendData = nloJson.serialize(sendMap);
    LOG_INFO().noquote() << QString("查询交易号 %1 的稽核审核图片: ").arg(tradeID) << sendData.left(1024);

    QVariantList records;
    QByteArray result;
    Http client;
    bool ok = client.postSync(result, url, sendData.toUtf8(), "application/json");
    if (ok) {
        LOG_INFO().noquote() << QString("返回交易号 %1 的稽核审核图片查询结果: ").arg(tradeID) << result.left(1024);
        QVariant parsed = nloJson.parse(result);
        if (!parsed.isValid() || !parsed.canConvert<QVariantMap>()) {
            throw BaseException(1, QString("响应失败: 站级服务响应数据异常!"));
        }

        QVariantMap resMap = parsed.toMap();
        if (resMap["errCode"].toInt() == 1) {
            QString errorMessage = resMap["errorMessage"].toString();
            throw BaseException(1, QString("响应失败: 站级服务返回失败 %1").arg(errorMessage));
        } else {
            records = resMap["data"].toList();
        }
    } else {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    return records;
}

QVariantList DataService::getExGreenPassTrades(const QString &startTime, const QString &stopTime, QUrl url)
{
    QVariantMap sendMap;
    sendMap["queryType"] = "queryData";
    sendMap["queryAuth"] = "1";
    sendMap["querySql"] = QString("SELECT tradeid, passid, exvehplate, ennetid, enstation, exnetid, exstation, extime, entotalweight, "
                                  "totalweight, cardtype, shouldpay, factpay, provincenum, cardbiztype, reserve, usertype, obuplate "
                                  "FROM T_ETC_OUT WHERE (usertype = 21 OR usertype = 22) AND extime >= '%1' AND extime < '%2' UNION ALL "
                                  "SELECT tradeid, passid, exvehplate, ennetid, enstation, exnetid, exstation, extime, entotalweight, "
                                  "totalweight, cardtype, shouldpay, factpay, '' AS provincenum, cardbiztype, reserve, usertype, "
                                  "'' AS obuplate FROM T_MTC_OUT WHERE (usertype = 21 OR usertype = 22) AND extime >= '%3' AND "
                                  "extime < '%4'")
                              .arg(startTime, stopTime, startTime, stopTime);
    sendMap["dataType"] = 4;

    NloJson nloJson;

    QString sendData = nloJson.serialize(sendMap);
    LOG_INFO().noquote() << QString("本站绿通流水 %1~%2 查询请求: ").arg(startTime, stopTime) << sendData.left(1024);

    QVariantList records;
    QByteArray result;
    Http client;
    bool ok = client.postSync(result, url, sendData.toUtf8(), "application/json");
    if (ok) {
        LOG_INFO().noquote() << QString("返回本站绿通流水 %1~%2 查询结果: ").arg(startTime, stopTime) << result.left(1024);
        QVariant parsed = nloJson.parse(result);
        if (!parsed.isValid() || !parsed.canConvert<QVariantMap>()) {
            throw BaseException(1, QString("响应失败: 站级服务响应数据异常!"));
        }

        QVariantMap resMap = parsed.toMap();
        if (resMap["errCode"].toInt() == 1) {
            QString errorMessage = resMap["errorMessage"].toString();
            throw BaseException(1, QString("响应失败: 站级服务返回失败 %1").arg(errorMessage));
        } else {
            records = resMap["data"].toList();
        }
    } else {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    return records;
}

QVariantList DataService::getStationAuthorization(const QString &stationID)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    QString sql(R"(SELECT * FROM t_emergencyauthfunction WHERE stationid = ?)");

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::PreparedQuery query = t.prepare(sql);
        EasyQtSql::QueryResult res = query.exec(stationID);

        LOG_INFO().noquote() << "执行SQL语句: " << Utils::DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        QVariantList resList;
        while (res.next()) {
            QVariantMap oneMap = res.toMap();
            resList.append(oneMap);
        }

        return resList;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery.left(1024);
        return {};
    }
}

bool DataService::testConnection() const
{
    if (!m_dbFactory) {
        LOG_ERROR().noquote() << "数据库连接初始化失败: SqlFactory为空";
        return false;
    }

    QSqlDatabase sdb = m_dbFactory->getDatabase("oracle");
    if (!sdb.isValid()) {
        LOG_ERROR().noquote() << "数据库连接初始化失败: 无效的数据库连接";
        return false;
    }

    if (!sdb.isOpen()) {
        LOG_ERROR().noquote() << "数据库连接初始化失败:" << sdb.lastError().text();
        return false;
    }

    QSqlQuery query(sdb);
    QString testSql = "SELECT 1 FROM DUAL";
    if (!query.exec(testSql)) {
        LOG_ERROR().noquote() << "数据库连接初始化失败:" << query.lastError().text() << "\t" << testSql;
        return false;
    }

    LOG_INFO().noquote() << "数据库连接初始化成功";
    return true;
}
