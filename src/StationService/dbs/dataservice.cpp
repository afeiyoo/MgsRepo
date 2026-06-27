#include "dataservice.h"
#include "utils/stdafx.h"

#include "EasyQtSql.h"
#include "Logger.h"
#include "utils/datadealutils.h"

using namespace Utils;
using namespace EasyQtSql;

DataService::DataService(QObject *parent)
    : QObject{parent}
{}

DataService::~DataService()
{
    SAFE_DELETE(m_dbFactory);
}

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
    return false;
}

QVariantList DataService::fetchSuccessedTrades(int vehicleIdentifyType, QString vehPlate, QString cardID, int dataType, int judgeTime) const
{
    QString sql = getSuccessedTradesSql(vehicleIdentifyType, vehPlate, cardID, dataType, judgeTime);
    QSqlDatabase sdb = m_dbFactory->getDatabase();

    Transaction t(sdb);
    try {
        QueryResult res = t.execQuery(sql);
        LOG_INFO().noquote() << "执行SQL:" << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        QVariantList trades;
        while (res.next()) {
            trades.append(res.toMap());
        }

        return trades;
    } catch (const DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << '\t' << e.lastQuery.left(1024);
        return {};
    }
}
