#include "dataservice.h"

#include "EasyQtSql.h"
#include "Logger.h"
#include "core/baseexception.h"
#include "core/globalmanager.h"
#include "utils/datadealutils.h"

using namespace Utils;

DataService::DataService(QObject *parent)
    : QObject{parent}
{}

DataService::~DataService() {}

bool DataService::testConnection() const
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
    QString testSql = getTestSql();
    if (!query.exec(testSql)) {
        LOG_ERROR().noquote() << "数据库连接初始化失败:" << query.lastError().text() << "\t" << testSql;
        return false;
    }

    LOG_INFO().noquote() << "数据库连接初始化成功";
    return true;
}

QVariantList DataService::fetchSuccessedTrades(int vehicleIdentifyType, QString vehPlate, QString cardID, int dataType, int judgeTime) const
{
    QString sql = getSuccessedTradesSql(vehicleIdentifyType, vehPlate, cardID, dataType, judgeTime);
    QSqlDatabase sdb = m_dbFactory->getDatabase();

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::QueryResult res = t.execQuery(sql);
        LOG_INFO().noquote() << "执行SQL:" << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        QVariantList trades;
        while (res.next()) {
            trades.append(res.toMap());
        }

        return trades;
    } catch (const EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return {};
    }
}

int DataService::fetchRecordCnt(const QVariantMap &kvs, const QString &tableName, const QString &tableKey) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QStringList keys = tableKey.split(",", Qt::SkipEmptyParts);
#else
    QStringList keys = tableKey.split(",", QString::SkipEmptyParts);
#endif

    QString whereClause;
    for (const QString &key : keys) {
        QString k = key.trimmed();
        QString mapKey = findMapKeyCaseInsensitive(kvs, k);
        if (mapKey.isEmpty())
            continue;

        QString value = valueToSqlLiteral(kvs.value(mapKey));
        if (value.isEmpty())
            continue;

        if (!whereClause.isEmpty())
            whereClause += " AND ";
        whereClause += (k + "=" + value);
    }

    if (whereClause.isEmpty()) {
        LOG_ERROR().noquote() << "组装SQL失败: where子句为空";
        return -1;
    }

    QString sql = "SELECT count(*) FROM " + tableName + " WHERE " + whereClause;
    QSqlDatabase sdb = m_dbFactory->getDatabase();

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::QueryResult res = t.execQuery(sql);
        LOG_INFO().noquote() << "执行SQL: " << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return 0;

        return res.scalar<int>();
    } catch (const EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery.left(1024);
        return -1;
    }
}

bool DataService::updateRecord(const QVariantMap &kvs, const QString &tableName, const QString &tableKey) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QStringList keys = tableKey.split(",", Qt::SkipEmptyParts);
#else
    QStringList keys = tableKey.split(",", QString::SkipEmptyParts);
#endif

    QVariantMap newKvs = kvs;

    QString whereClause;
    for (const QString &key : keys) {
        QString k = key.trimmed();
        QString mapKey = findMapKeyCaseInsensitive(newKvs, k);
        if (mapKey.isEmpty())
            continue;
        QVariant v = newKvs.take(mapKey); // 挪出主键对应的键值对

        QString value = valueToSqlLiteral(v);
        if (value.isEmpty())
            continue;

        if (!whereClause.isEmpty())
            whereClause += " AND ";
        whereClause += (k + "=" + value);
    }

    if (whereClause.isEmpty()) {
        LOG_ERROR().noquote() << "组装SQL失败: where子句为空";
        return false;
    }

    QSqlDatabase sdb = m_dbFactory->getDatabase();
    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::NonQueryResult res = t.update(tableName).set(newKvs).where(whereClause);
        LOG_INFO().noquote() << "执行SQL语句: " << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        return t.commit();
    } catch (const EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery.left(1024);
        return false;
    }
}

bool DataService::insertRecord(const QVariantMap &kvs, const QString &tableName) const
{
    QSqlDatabase sdb = m_dbFactory->getDatabase();
    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::NonQueryResult res = t.insertInto(tableName).values(kvs).exec();
        LOG_INFO().noquote() << "执行sql语句: " << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());
        return t.commit();
    } catch (const EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery.left(1024);
        return false;
    }
}

int DataService::fetchXZPassTimes(const QString &sql) const
{
    QSqlDatabase sdb = m_dbFactory->getDatabase();

    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::QueryResult res = t.execQuery(sql);
        LOG_INFO().noquote() << "执行SQL: " << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return 0;

        return res.scalar<int>();
    } catch (const EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery.left(1024);
        return -1;
    }
}

int DataService::fetchShiftCnt(const QString &shiftDate, int shiftID, int laneID, int flag) const
{
    QString sql = getShiftCntSql(shiftDate, shiftID, laneID, flag);
    QSqlDatabase sdb = m_dbFactory->getDatabase();
    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::QueryResult res = t.execQuery(sql);
        LOG_INFO().noquote() << "执行SQL: " << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return 0;

        int count = res.scalar<int>();
        return count;
    } catch (const EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery.left(1024);
        return -1;
    }
}

int DataService::queryInt(const QString &sql, bool *ok) const
{
    *ok = true;
    QSqlDatabase sdb = m_dbFactory->getDatabase();
    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::QueryResult res = t.execQuery(sql);
        LOG_INFO().noquote() << "执行SQL: " << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return 0;

        int ans = res.scalar<int>();
        return ans;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery.left(1024);
        *ok = false;
        return -1;
    }
}

QString DataService::queryString(const QString &sql, bool *ok) const
{
    *ok = true;
    QSqlDatabase sdb = m_dbFactory->getDatabase();
    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::QueryResult res = t.execQuery(sql);
        LOG_INFO().noquote() << "执行SQL: " << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return "";

        QString ans = res.scalar<QString>();
        return ans;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery.left(1024);
        *ok = false;
        return "";
    }
}

QVariantMap DataService::queryMap(const QString &sql, bool *ok) const
{
    *ok = true;
    QSqlDatabase sdb = m_dbFactory->getDatabase();
    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::QueryResult res = t.execQuery(sql);
        LOG_INFO().noquote() << "执行SQL: " << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return {};

        return res.toMap();
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery.left(1024);
        *ok = false;
        return {};
    }
}

QVariantList DataService::queryList(const QString &sql, bool *ok) const
{
    *ok = true;
    QSqlDatabase sdb = m_dbFactory->getDatabase();
    EasyQtSql::Transaction t(sdb);
    try {
        EasyQtSql::QueryResult res = t.execQuery(sql);
        LOG_INFO().noquote() << "执行SQL: " << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        QVariantList records;
        while (res.next()) {
            QVariantMap record = res.toMap();
            records.append(record);
        }
        return records;
    } catch (EasyQtSql::DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery.left(1024);
        *ok = false;
        return {};
    }
}

QString DataService::findMapKeyCaseInsensitive(const QVariantMap &map, const QString &key) const
{
    for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
        if (it.key().compare(key, Qt::CaseInsensitive) == 0)
            return it.key();
    }

    return QString();
}

QString DataService::valueToSqlLiteral(const QVariant &value) const
{
    if (!value.isValid() || value.isNull())
        return "NULL";

    switch (value.type()) {
    case QVariant::Bool:
        return value.toBool() ? "1" : "0";
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::LongLong:
    case QVariant::ULongLong:
        return value.toString();
    case QVariant::Double:
        return QString::number(value.toDouble(), 'g', 15);
    default:
        return "'" + value.toString().replace("'", "''") + "'";
    }
}
