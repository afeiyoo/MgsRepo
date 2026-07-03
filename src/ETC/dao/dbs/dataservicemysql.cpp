#include "dataservicemysql.h"

#include "EasyQtSql.h"

using namespace EasyQtSql;

DataServiceMysql::DataServiceMysql(QObject *parent)
    : DataService{parent}
{}

DataServiceMysql::~DataServiceMysql() {}

bool DataServiceMysql::init(const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName)
{
    SqlFactory::DBSetting setting("QMYSQL", host, port, userName, passWord, dbName);
    m_dbFactory = SqlFactory::getInstance()->config(setting, "biz");
    return testConnection("biz", "SELECT 1");
}
