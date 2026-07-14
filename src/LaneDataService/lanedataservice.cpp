#include "lanedataservice.h"

#include <QVariantMap>

#include "bend/fullblackchecker.h"
#include "config/config.h"
#include "core/baseexception.h"
#include "core/globalmanager.h"
#include "dbs/dataservice.h"
#include "dbs/sqldealer.h"
#include "iservicehub.h"
#include "utils/datadealutils.h"

using namespace Utils;

LaneDataService::LaneDataService(IServiceHub *hub)
    : m_hub(hub)
{
    // 注册服务
    m_hub->registerService(this);
}

LaneDataService::~LaneDataService() {}

int LaneDataService::init()
{
    return GM_INS->init();
}

QByteArray LaneDataService::fetchString(const QByteArray &json)
{
    QVariantMap resMap;
    try {
        if (!GM_INS->m_ds)
            throw BaseException(-1, "数据服务未初始化");

        bool ok = false;
        QString errDesc;
        QVariantMap oneMap = DataDealUtils::jsonToMap(json, &ok, &errDesc);
        if (!ok)
            throw BaseException(-1, errDesc);

        if (!oneMap.contains("namespace"))
            throw BaseException(-1, "未传入namespace字段");
        if (!oneMap.contains("sqlID"))
            throw BaseException(-1, "未传入sqlID字段");
        if (!oneMap.contains("params"))
            throw BaseException(-1, "未传入params字段");

        QString sqlNamespace = oneMap["namespace"].toString();
        QString sqlID = oneMap["sqlID"].toString();
        QString def = oneMap["def"].toString();
        QVariantMap params = oneMap["params"].toMap();

        QString sql = GM_INS->m_sqlDealer->getSql(sqlNamespace, sqlID);

        QString ans = GM_INS->m_ds->fetchString(sql, params, def);
        resMap["status"] = 0;
        resMap["desc"] = "";
        resMap["data"] = ans;
        return DataDealUtils::mapToJson(resMap);
    } catch (const BaseException &e) {
        resMap["status"] = e.status();
        resMap["desc"] = e.desc();
        return DataDealUtils::mapToJson(resMap);
    }
}

QByteArray LaneDataService::fetchInt(const QByteArray &json)
{
    // TODO 待实现
}

QByteArray LaneDataService::fetchReal(const QByteArray &json)
{
    // TODO 待实现
}

QByteArray LaneDataService::fetchList(const QByteArray &json)
{
    // TODO 待实现
}

QByteArray LaneDataService::fetchMap(const QByteArray &json)
{
    // TODO 待实现
}

QByteArray LaneDataService::fetchDate(const QByteArray &json)
{
    // TODO 待实现
}

QByteArray LaneDataService::updateRecord(const QByteArray &json)
{
    QVariantMap resMap;
    try {
        if (!GM_INS->m_ds)
            throw BaseException(-1, "数据服务未初始化");

        bool ok = false;
        QString errDesc;
        QVariantMap oneMap = DataDealUtils::jsonToMap(json, &ok, &errDesc);
        if (!ok)
            throw BaseException(-1, errDesc);

        QString table;
        QVariantMap updateParams;
        QString whereClause;
        if (oneMap.contains("table"))
            table = oneMap["table"].toString();
        if (oneMap.contains("updateParams"))
            updateParams = oneMap["updateParams"].toMap();
        if (oneMap.contains("whereClause"))
            whereClause = oneMap["whereClause"].toString();

        if (table.isEmpty())
            throw BaseException(-1, "table字段不能为空");
        if (whereClause.trimmed().isEmpty())
            throw BaseException(-1, "whereClause字段字段不能为空");
        if (updateParams.isEmpty())
            throw BaseException(-1, "updateParams字段不能为空");

        int affected = GM_INS->m_ds->updateRecords(table, updateParams, whereClause);
        if (affected < 0)
            throw BaseException(-1, "更新记录失败");

        resMap["status"] = 0;
        resMap["desc"] = "";
        resMap["data"] = affected;
        return DataDealUtils::mapToJson(resMap);
    } catch (const BaseException &e) {
        resMap["status"] = e.status();
        resMap["desc"] = e.desc();
        return DataDealUtils::mapToJson(resMap);
    }
}

QByteArray LaneDataService::insertRecord(const QByteArray &json)
{
    QVariantMap resMap;
    try {
        if (!GM_INS->m_ds)
            throw BaseException(-1, "数据服务未初始化");

        bool ok = false;
        QString errDesc;
        QVariantMap oneMap = DataDealUtils::jsonToMap(json, &ok, &errDesc);
        if (!ok)
            throw BaseException(-1, errDesc);

        QString table;
        QVariantMap insertParams;
        QStringList ignoreFields;
        if (oneMap.contains("table"))
            table = oneMap["table"].toString();
        if (oneMap.contains("insertParams"))
            insertParams = oneMap["insertParams"].toMap();

        ignoreFields = oneMap["ignoreFields"].toStringList();
        // 去除自增列
        for (const auto &field : ignoreFields) {
            insertParams.remove(field);
        }

        if (table.trimmed().isEmpty())
            throw BaseException(-1, "table字段不能为空");
        if (insertParams.isEmpty())
            throw BaseException(-1, "insertParams字段不能为空");

        int affected = GM_INS->m_ds->insertRecords(table, insertParams);
        if (affected < 0)
            throw BaseException(-1, "插入记录失败");

        resMap["status"] = 0;
        resMap["desc"] = "";
        resMap["data"] = affected;
        return DataDealUtils::mapToJson(resMap);
    } catch (const BaseException &e) {
        resMap["status"] = e.status();
        resMap["desc"] = e.desc();
        return DataDealUtils::mapToJson(resMap);
    }
}

QByteArray LaneDataService::deleteRecord(const QByteArray &json)
{
    QVariantMap resMap;
    try {
        if (!GM_INS->m_ds)
            throw BaseException(-1, "数据服务未初始化");

        bool ok = false;
        QString errDesc;
        QVariantMap oneMap = DataDealUtils::jsonToMap(json, &ok, &errDesc);
        if (!ok)
            throw BaseException(-1, errDesc);

        QString table;
        QString whereClause;
        if (oneMap.contains("table"))
            table = oneMap["table"].toString();
        if (oneMap.contains("whereClause"))
            whereClause = oneMap["whereClause"].toString();

        if (table.trimmed().isEmpty())
            throw BaseException(-1, "table字段不能为空");
        if (whereClause.isEmpty())
            throw BaseException(-1, "whereClause字段不能为空");

        int affected = GM_INS->m_ds->deleteRecords(table, whereClause);
        if (affected < 0)
            throw BaseException(-1, "删除记录失败");

        resMap["status"] = 0;
        resMap["desc"] = "";
        resMap["data"] = affected;
        return DataDealUtils::mapToJson(resMap);
    } catch (const BaseException &e) {
        resMap["status"] = e.status();
        resMap["desc"] = e.desc();
        return DataDealUtils::mapToJson(resMap);
    }
}

QByteArray LaneDataService::truncateTable(const QByteArray &json)
{
    QVariantMap resMap;
    try {
        if (!GM_INS->m_ds)
            throw BaseException(-1, "数据服务未初始化");

        bool ok = false;
        QString errDesc;
        QVariantMap oneMap = DataDealUtils::jsonToMap(json, &ok, &errDesc);
        if (!ok)
            throw BaseException(-1, errDesc);

        QString table;
        if (oneMap.contains("table"))
            table = oneMap["table"].toString();

        if (table.trimmed().isEmpty())
            throw BaseException(-1, "table字段不能为空");

        int affected = GM_INS->m_ds->truncateTable(table);
        if (affected < 0)
            throw BaseException(-1, "清空表内容失败");

        resMap["status"] = 0;
        resMap["desc"] = "";
        resMap["data"] = affected;
        return DataDealUtils::mapToJson(resMap);
    } catch (const BaseException &e) {
        resMap["status"] = e.status();
        resMap["desc"] = e.desc();
        return DataDealUtils::mapToJson(resMap);
    }
}

QByteArray LaneDataService::isFullBlackOk(const QByteArray &json)
{
    QVariantMap resMap;
    resMap["status"] = GM_INS->m_fullBlackChecker->m_fullBlackStatus;
    if (GM_INS->m_fullBlackChecker->m_fullBlackStatus == 0) {
        resMap["desc"] = "全量状态正常";
    } else {
        resMap["desc"] = "全量状态异常";
    }
    resMap["data"] = GM_INS->m_conf->m_fullBatchNo;
    return DataDealUtils::mapToJson(resMap);
}

ILaneDataService *createLaneDataService(IServiceHub *hub)
{
    return new LaneDataService(hub);
}
