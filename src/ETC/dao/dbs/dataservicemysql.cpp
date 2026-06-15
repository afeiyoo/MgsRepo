#include "dataservicemysql.h"

#include "EasyQtSql.h"

using namespace EasyQtSql;

DataServiceMysql::DataServiceMysql(QObject *parent)
    : DataService{parent}
{}

DataServiceMysql::~DataServiceMysql() {}

void DataServiceMysql::init(const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName)
{
    SqlFactory::DBSetting setting("MYSQL", host, port, userName, passWord, dbName);
    m_dbFactory = SqlFactory::getInstance()->config(setting, "biz");
}
