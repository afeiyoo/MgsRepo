#include "dataservice.h"

#include <QSqlDatabase>

#include "EasyQtSql.h"
#include "Logger.h"
#include "utils/datadealutils.h"

using namespace EasyQtSql;
using namespace Utils;

DataService::DataService(QObject *parent)
    : QObject{parent}
{}

DataService::~DataService() {}

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
