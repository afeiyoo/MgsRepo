#include "dataservice.h"

#include "EasyQtSql.h"
#include "Logger.h"
#include "config/config.h"
#include "core/globalmanager.h"
#include "core/signalmanager.h"
#include "utils/datadealutils.h"

using namespace Utils;
using namespace EasyQtSql;

DataService::DataService(QObject *parent)
    : QObject(parent)
{}

DataService::~DataService() {}

bool DataService::init(uint type, const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName)
{
    connect(GM_INS->m_sigMan, &SignalManager::sigLoadFullBlack, this, &DataService::onLoadFullBlack);

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

QString DataService::fetchString(const QString &sql, const QVariantMap &params, const QString &def)
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

bool DataService::isBlack(const QString &cardID)
{
    QString activeConnName = getActiveFBConnName();
    QSqlDatabase sdb = m_dbFactory->getDatabase(activeConnName);
    Transaction t(sdb);
    try {
    } catch (const EasyQtSql::DBException &e) {
    }
}

void DataService::onLoadFullBlack(const QString &path, int batchNo)
{
    const QString newConnName = QString("fb_%1_%2").arg(batchNo).arg(++m_fbConnSeq);
    const QString oldConnName = getActiveFBConnName();

    SqlFactory::DBSetting setting("QSQLITE", path);
    m_dbFactory = SqlFactory::getInstance()->config(setting, newConnName);
    if (!testConnection(newConnName, "SELECT 1")) {
        emit GM_INS->m_sigMan->sigLoadFullBlackRes(false, batchNo, "");
        m_dbFactory->removeDataBase(newConnName); // 删除缓存连接
        return;
    }

    // 校验全量是否正常
    QVariantMap resMap = fetchFullBlackInfo(batchNo, newConnName);
    if (resMap.isEmpty()) {
        LOG_ERROR().noquote() << "全量文件异常，获取cleantable,version信息失败!";
        emit GM_INS->m_sigMan->sigLoadFullBlackRes(false, batchNo, "");
        m_dbFactory->removeDataBase(newConnName); // 删除缓存连接
        return;
    }
    QString version;
    QString cleanTable;
    if (resMap.contains("version"))
        version = resMap["version"].toString();
    if (resMap.contains("cleantable"))
        cleanTable = resMap["cleantable"].toString();
    if (version.isEmpty() || cleanTable.isEmpty()) {
        LOG_ERROR().noquote() << "全量文件异常，cleantable或version为空!";
        emit GM_INS->m_sigMan->sigLoadFullBlackRes(false, batchNo, "");
        m_dbFactory->removeDataBase(newConnName); // 删除缓存连接
        return;
    }

    // 更新全量时，需要删除旧全量信息
    if (batchNo > GM_INS->m_conf->m_fullBatchNo) {
        int affected = truncateETCBlackCard(cleanTable, newConnName);
        if (affected < 0) {
            LOG_ERROR().noquote() << "更新全量时，清除" << cleanTable << "失败";
            emit GM_INS->m_sigMan->sigLoadFullBlackRes(false, batchNo, "");
            m_dbFactory->removeDataBase(newConnName); // 删除缓存连接
            return;
        }
    }

    // 删除旧连接
    GM_INS->m_conf->setFullBatchNo(batchNo);
    setActiveFBConnName(newConnName);
    if (!oldConnName.isEmpty())
        m_dbFactory->removeDataBase(oldConnName);

    emit GM_INS->m_sigMan->sigLoadFullBlackRes(true, batchNo, version);
}

QString DataService::getActiveFBConnName()
{
    QReadLocker locker(&m_activeFBConnLock);
    return m_activeFBConnName;
}

void DataService::setActiveFBConnName(const QString &name)
{
    QWriteLocker locker(&m_activeFBConnLock);
    m_activeFBConnName = name;
}

QVariantMap DataService::fetchFullBlackInfo(int batchNo, const QString &connName)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase(connName);
    const QString sql = "SELECT version, cleantable FROM t_operatectrl WHERE paramtype = ? AND batchno = ?";

    Transaction t(sdb);
    try {
        PreparedQuery query = t.prepare(sql);
        QueryResult res = query.exec(515, batchNo);

        LOG_INFO().noquote() << "执行SQL语句:" << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return {};

        return res.toMap();
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery;
        return {};
    }
}

int DataService::truncateETCBlackCard(const QString &table, const QString &connName)
{
    QSqlDatabase sdb = m_dbFactory->getDatabase(connName);
    Transaction t(sdb);
    try {
        NonQueryResult res = t.deleteFrom(table).exec();
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
