#include "dataservicedameng.h"

#include "EasyQtSql.h"

using namespace EasyQtSql;

DataServiceDameng::DataServiceDameng(QObject *parent)
    : DataService{parent}
{}

DataServiceDameng::~DataServiceDameng() {}

void DataServiceDameng::init(const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName)
{
    SqlFactory::DBSetting setting("QODBC", host, port, userName, passWord, dbName);
    m_dbFactory = SqlFactory::getInstance()->config(setting, "biz");
}
