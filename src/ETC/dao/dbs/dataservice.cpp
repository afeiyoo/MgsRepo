#include "dataservice.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "EasyQtSql.h"
#include "Logger.h"
#include "utils/datadealutils.h"

using namespace EasyQtSql;
using namespace Utils;

DataService::DataService(QObject *parent)
    : QObject{parent}
{}

DataService::~DataService() {}

bool DataService::testConnection(const QString &connectionName, const QString &testSql) const
{
    if (!m_dbFactory) {
        LOG_CERROR("db").noquote() << "数据库连接初始化失败: SqlFactory为空";
        return false;
    }

    QSqlDatabase sdb = m_dbFactory->getDatabase(connectionName);
    if (!sdb.isValid()) {
        LOG_CERROR("db").noquote() << "数据库连接初始化失败: 无效的数据库连接";
        return false;
    }

    if (!sdb.isOpen()) {
        LOG_CERROR("db").noquote() << "数据库连接初始化失败:" << sdb.lastError().text();
        return false;
    }

    QSqlQuery query(sdb);
    if (!query.exec(testSql)) {
        LOG_CERROR("db").noquote() << "数据库连接探测失败:" << query.lastError().text() << "\t" << testSql;
        return false;
    }

    LOG_CINFO("db").noquote() << "数据库连接初始化成功:" << testSql;
    return true;
}

QString DataService::getStationIP(const QString &stationID) const
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("biz");

    QString sql = QString(R"(SELECT IPADDR FROM T_STATION WHERE STATIONID = ? AND IPADDR NOT IN ('0000', '0.0.0.0');)");

    Transaction t(sdb);
    try {
        PreparedQuery query = t.prepare(sql);
        QueryResult res = query.exec(stationID);

        LOG_CINFO("db").noquote() << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return QStringLiteral("");

        QString data = res.value("IPADDR").toString();
        return data;
    } catch (const DBException &e) {
        LOG_CERROR("db").noquote() << e.lastError.text() << "\t" << e.lastQuery;
        return QStringLiteral("");
    }
}

bool DataService::isHolidayFreeVehClass(uint vehClass, bool checkVehClass) const
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("biz");

    QString sql = QString(R"(SELECT COUNT(*) AS CNT FROM T_HOLIDAYFREE WHERE NOW() BETWEEN STARTTIME AND STOPTIME;)");
    if (checkVehClass) {
        sql += QString(" AND VEHCLASS = %1").arg(vehClass);
    }

    Transaction t(sdb);
    try {
        PreparedQuery query = t.prepare(sql);
        QueryResult res = query.exec();

        LOG_CINFO("db").noquote() << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return false;

        int data = res.value("CNT").toInt();
        return data > 0;
    } catch (const DBException &e) {
        LOG_CERROR("db").noquote() << e.lastError.text() << "\t" << e.lastQuery;
        return false;
    }
}

void DataService::saveLaneCapRecord(const QString &vehPlate, const QString &stationdID, int vehClass) {}

void DataService::saveFFCapRecord(const QString &captureID, const QString &vehPlate)
{

}

void DataService::saveFFShiftStat()
{

}
