#include "dataservice.h"

#include "EasyQtSql.h"
#include "Logger.h"

DataService::DataService() {}

DataService::~DataService() {}

bool DataService::testConnection()
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
