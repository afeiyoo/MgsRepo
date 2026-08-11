#include "dataservice.h"

#include "EasyQtSql.h"
#include "Logger.h"
#include "core/globalmanager.h"
#include "core/signalmanager.h"
#include "sqldealer.h"
#include "utils/datadealutils.h"

#include <QDir>
#include <QFileInfo>
#include <QSqlQuery>
#include <QThread>

#include <utility>

using namespace Utils;
using namespace EasyQtSql;

DataService::DataService(QObject *parent)
    : QObject(parent)
{}

DataService::~DataService()
{
    QStringList connectionNames;
    for (int i = 0; i < 2; ++i) {
        if (m_fbDao[i].isValid()) {
            connectionNames.append(m_fbDao[i].connectionName());
            if (m_fbDao[i].isOpen())
                m_fbDao[i].close();
            m_fbDao[i] = QSqlDatabase();
        }
    }

    if (m_dbDao.isValid()) {
        connectionNames.append(m_dbDao.connectionName());
        if (m_dbDao.isOpen())
            m_dbDao.close();
        m_dbDao = QSqlDatabase();
    }

    for (const QString &connectionName : connectionNames) {
        if (!connectionName.isEmpty() && QSqlDatabase::contains(connectionName))
            QSqlDatabase::removeDatabase(connectionName);
    }
}

bool DataService::init(uint type, const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName)
{
    connect(GM_INS->m_sigMan, &SignalManager::sigFullBlackActivated, this, &DataService::onFullBlackActivated);
    connect(GM_INS->m_sigMan, &SignalManager::sigDeltaBlackActivated, this, &DataService::onDeltaBlackActivated);

    // 建立黑名单查询连接
    for (int i = 0; i < 2; ++i) {
        const QString connectionName = QString("fullBlackQuery_%1").arg(i, 2, 10, QChar('0'));
        m_fbDao[i] = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        m_fbDao[i].setConnectOptions("QSQLITE_OPEN_READONLY;QSQLITE_BUSY_TIMEOUT=1000");
    }
    m_dbDao = QSqlDatabase::addDatabase("QSQLITE", "deltaBlackQuery");
    m_dbDao.setConnectOptions("QSQLITE_OPEN_READONLY;QSQLITE_BUSY_TIMEOUT=1000");

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

bool DataService::checkBlackCard(const QString &cardID)
{
    // TODO 待实现
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

void DataService::onFullBlackActivated(QString dbPath)
{
    if (dbPath.isEmpty() || !QFile::exists(dbPath)) {
        LOG_ERROR().noquote() << "切换全量查询连接失败: 数据库路径无效，路径:" << dbPath;
        return;
    }

    const QString currentPath = QDir::cleanPath(m_fbDao[0].databaseName());
    if (m_fbDao[0].isOpen() && currentPath == dbPath) {
        LOG_INFO().noquote() << "全量查询连接已经是目标数据库，无需重复切换，路径:" << dbPath;
        return;
    }

    if (m_fbDao[1].isOpen())
        m_fbDao[1].close();
    m_fbDao[1].setDatabaseName(dbPath);
    if (!m_fbDao[1].open()) {
        LOG_ERROR().noquote() << "建立候选全量查询连接失败，路径:" << dbPath << "错误:" << m_fbDao[1].lastError().text();
        return;
    }

    std::swap(m_fbDao[0], m_fbDao[1]);
    m_fbDao[1].close();

    LOG_INFO().noquote() << "全量查询连接切换成功，路径:" << dbPath;
}

void DataService::onDeltaBlackActivated(QString dbPath)
{
    if (dbPath.isEmpty() || !QFile::exists(dbPath)) {
        LOG_ERROR().noquote() << "打开增量查询连接失败: 数据库路径无效，路径:" << dbPath;
        return;
    }

    if (m_dbDao.isOpen())
        return;

    m_dbDao.setDatabaseName(dbPath);
    if (!m_dbDao.open()) {
        LOG_ERROR().noquote() << "建立增量查询连接失败，路径:" << dbPath << "错误:" << m_dbDao.lastError().text();
        return;
    }

    LOG_INFO().noquote() << "增量查询连接打开成功，路径:" << dbPath;
}
