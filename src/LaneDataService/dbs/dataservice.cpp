#include "dataservice.h"

#include "EasyQtSql.h"
#include "Logger.h"
#include "core/globalmanager.h"
#include "utils/datadealutils.h"

using namespace Utils;
using namespace EasyQtSql;

DataService::DataService() {}

DataService::~DataService() {}

bool DataService::init(uint type, const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName)
{
    // 建立连接
    SqlFactory::DBSetting setting;
    QString testSql;
    if (type == 1) {
        setting = SqlFactory::DBSetting("QMYSQL", host, port, userName, passWord, dbName);
        testSql = "SELECT 1;";
    } else {
        setting = SqlFactory::DBSetting("QODBC", host, port, userName, passWord, dbName);
        testSql = "SELECT 1 FROM DUAL;";
    }
    m_dbFactory = SqlFactory::getInstance()->config(setting);
    return testConnection(testSql);
}

bool DataService::testConnection(const QString &sql)
{
    if (!m_dbFactory) {
        LOG_ERROR().noquote() << "数据库连接初始化失败: SqlFactory为空";
        return false;
    }

    QSqlDatabase sdb = m_dbFactory->getDatabase();
    if (!sdb.isValid()) {
        LOG_ERROR().noquote() << "数据库连接初始化失败: 无效的数据库连接";
        return false;
    }

    if (!sdb.isOpen()) {
        LOG_ERROR().noquote() << "数据库连接初始化失败:" << sdb.lastError().text();
        return false;
    }

    QSqlQuery query(sdb);
    if (!query.exec(sql)) {
        LOG_ERROR().noquote() << "数据库连接初始化失败:" << query.lastError().text() << "\t" << sql;
        return false;
    }

    LOG_INFO().noquote() << "数据库连接初始化成功";
    return true;
}

QString DataService::fetchString(const QString &sql, const QVariantMap &params, const QString &def)
{
    if (sql.isEmpty())
        return def;

    QSqlDatabase sdb = m_dbFactory->getDatabase();
    Transaction t(sdb);
    try {
        PreparedQuery query = t.prepare(sql);
        QueryResult res = query.exec(params);
        LOG_INFO().noquote() << "执行SQL:" << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return def;

        QString ans = res.scalar<QString>();
        return ans;
    } catch (const EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery;
        return def;
    }
}

int DataService::fetchInt(const QString &sql, const QVariantMap &params, const int def)
{
    if (sql.isEmpty())
        return def;

    QSqlDatabase sdb = m_dbFactory->getDatabase();
    Transaction t(sdb);
    try {
        PreparedQuery query = t.prepare(sql);
        QueryResult res = query.exec(params);
        LOG_INFO().noquote() << "执行SQL:" << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return def;

        int ans = res.scalar<int>();
        return ans;
    } catch (const EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery;
        return def;
    }
}

int DataService::updateRecords(const QString &table, const QVariantMap &updateParams, const QString &whereClause)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase();
    Transaction t(sdb);
    try {
        NonQueryResult res = t.update(table).set(updateParams).where(whereClause);
        LOG_INFO().noquote() << "执行SQL:" << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        int affected = res.numRowsAffected();
        if (!t.commit())
            return -1;

        return affected;
    } catch (const EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery;
        t.rollback();
        return -1;
    }
}

int DataService::insertRecords(const QString &table, const QVariantMap &insertParams)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase();
    Transaction t(sdb);
    try {
        NonQueryResult res = t.insertInto(table).values(insertParams).exec();
        LOG_INFO().noquote() << "执行SQL:" << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        int affected = res.numRowsAffected();
        if (!t.commit())
            return -1;

        return affected;
    } catch (const EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery;
        t.rollback();
        return -1;
    }
}

int DataService::deleteRecords(const QString &table, const QString &whereClause)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase();
    Transaction t(sdb);
    try {
        NonQueryResult res = t.deleteFrom(table).where(whereClause);
        LOG_INFO().noquote() << "执行SQL:" << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        int affected = res.numRowsAffected();
        if (!t.commit())
            return -1;

        return affected;
    } catch (const EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery;
        t.rollback();
        return -1;
    }
}

int DataService::truncateTable(const QString &table)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase();
    Transaction t(sdb);
    try {
        NonQueryResult res = t.deleteFrom(table).where("1=1");
        LOG_INFO().noquote() << "执行SQL:" << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        int affected = res.numRowsAffected();
        if (!t.commit())
            return -1;

        return affected;
    } catch (const EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery;
        t.rollback();
        return -1;
    }
}
