#include "dataservice.h"

#include "EasyQtSql.h"
#include "Logger.h"
#include "core/globalmanager.h"
#include "sqldealer.h"
#include "utils/datadealutils.h"

using namespace Utils;
using namespace EasyQtSql;

DataService::DataService(QObject *parent)
    : QObject(parent)
{}

DataService::~DataService() {}

bool DataService::init(uint type, const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName)
{
    // 建立连接
    SqlFactory::DBSetting setting;
    QString testSql;
    if (type == 1) {
        setting = SqlFactory::DBSetting("QMYSQL", host, port, userName, passWord, dbName);
        testSql = "select 1";
    } else {
        setting = SqlFactory::DBSetting("QODBC", host, port, userName, passWord, dbName);
        testSql = "select 1 from dual";
    }
    m_dbFactory = SqlFactory::getInstance()->config(setting, "tolllanedb");
    return testConnection("tolllanedb", testSql);
}

bool DataService::testConnection(const QString &connName, const QString &sql)
{
    if (!m_dbFactory) {
        LOG_ERROR().noquote() << QString("数据库连接 %1 初始化失败: SqlFactory为空").arg(connName);
        return false;
    }

    QSqlDatabase sdb = m_dbFactory->getDatabase(connName);
    if (!sdb.isValid()) {
        LOG_ERROR().noquote() << QString("数据库连接 %1 初始化失败: 无效的数据库连接").arg(connName);
        return false;
    }

    if (!sdb.isOpen()) {
        LOG_ERROR().noquote() << QString("数据库连接 %1 初始化失败:").arg(connName) << sdb.lastError().text();
        return false;
    }

    QSqlQuery query(sdb);
    if (!query.exec(sql)) {
        LOG_ERROR().noquote() << QString("数据库连接 %1 初始化失败:").arg(connName) << query.lastError().text() << "\t" << sql;
        return false;
    }

    LOG_INFO().noquote() << QString("数据库连接 %1 初始化成功").arg(connName);
    return true;
}

QString DataService::fetchString(const QString &sqlNamespace, const QString &sqlID, const QVariantMap &params, const QString &def)
{
    QString sql = GM_INS->m_sql->getSql(sqlNamespace, sqlID);
    if (sql.isEmpty())
        return def;

    QSqlDatabase sdb = m_dbFactory->getDatabase("tolllanedb");
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

    QSqlDatabase sdb = m_dbFactory->getDatabase("tolllanedb");
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
    QSqlDatabase sdb = m_dbFactory->getDatabase("tolllanedb");

    try {
        Database db(sdb);
        return updateRecordsImpl(db, table, updateParams, whereClause);
    } catch (const EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery;
        return -1;
    }
}

int DataService::insertRecords(const QString &table, const QVariantMap &insertParams)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("tolllanedb");

    try {
        Database db(sdb);
        return insertRecordsImpl(db, table, insertParams);
    } catch (const EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery;
        return -1;
    }
}

int DataService::deleteRecords(const QString &table, const QString &whereClause)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("tolllanedb");

    try {
        Database db(sdb);
        return deleteRecordsImpl(db, table, whereClause);
    } catch (const EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery;
        return -1;
    }
}

int DataService::truncateTable(const QString &table)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("tolllanedb");

    try {
        Database db;
        return deleteRecordsImpl(db, table, "1=1");
    } catch (const EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery;
        return -1;
    }
}

bool DataService::cleanETCBlackCard(const QString &table)
{
    int affected = truncateTable(table);
    return affected >= 0;
}

int DataService::updateRecordsImpl(EasyQtSql::Database &db, const QString &table, const QVariantMap &updateParams, const QString &whereClause)
{
    NonQueryResult res = db.update(table).set(updateParams).where(whereClause);
    LOG_INFO().noquote() << "执行SQL:" << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());
    return res.numRowsAffected();
}

int DataService::insertRecordsImpl(EasyQtSql::Database &db, const QString &table, const QVariantMap &insertParams)
{
    NonQueryResult res = db.insertInto(table).values(insertParams).exec();
    LOG_INFO().noquote() << "执行SQL:" << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());
    return res.numRowsAffected();
}

int DataService::deleteRecordsImpl(EasyQtSql::Database &db, const QString &table, const QString &whereClause)
{
    NonQueryResult res = db.deleteFrom(table).where(whereClause);
    LOG_INFO().noquote() << "执行SQL:" << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());
    return res.numRowsAffected();
}
