#include "dataservice.h"

#include "EasyQtSql.h"
#include "Logger.h"
#include "core/globalmanager.h"
#include "sqldealer.h"
#include "utils/datadealutils.h"
#include "utils/stdafx.h"

using namespace Utils;
using namespace EasyQtSql;

DataService::DataService(QObject *parent)
    : QObject(parent)
{}

DataService::~DataService()
{
    SAFE_DELETE(m_sql);
}

bool DataService::init(uint type, const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName)
{
    m_sql = new SqlDealer();
    // 加载sql文件
    bool sqlOk = m_sql->loadSqlFiles();
    if (!sqlOk)
        return false;

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
    m_dbFactory = SqlFactory::getInstance()->config(setting, "main");
    return testConnection("main", testSql);
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
    QString sql = m_sql->getSql(sqlNamespace, sqlID);
    if (sql.isEmpty())
        return def;

    QSqlDatabase sdb = m_dbFactory->getDatabase("main");
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

    QSqlDatabase sdb = m_dbFactory->getDatabase("main");
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
    QSqlDatabase sdb = m_dbFactory->getDatabase("main");
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
    QSqlDatabase sdb = m_dbFactory->getDatabase("main");
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
    QSqlDatabase sdb = m_dbFactory->getDatabase("main");
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
    QSqlDatabase sdb = m_dbFactory->getDatabase("main");
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

bool DataService::cleanETCBlackCard(const QString &table)
{
    int affected = truncateTable(table);
    return affected >= 0;
}

QString DataService::getIncrementBlackVersion()
{
    // TODO 获取增量版本
    return "";
}
