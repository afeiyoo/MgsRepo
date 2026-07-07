#pragma once

#include <QObject>

namespace EasyQtSql {
class SqlFactory;
}
class SqlDealer;

class DataService
{
public:
    DataService();
    virtual ~DataService();

    // 数据库连接初始化
    bool init(uint type, const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName);

    // 数据库连接测试
    bool testConnection();

    // 获取数据库连接测试SQL
    QString getTestSql() const;

protected:
    // 数据库连接池
    EasyQtSql::SqlFactory *m_dbFactory = nullptr;
    // Sql语句管理对象
    SqlDealer *m_sqlDealer = nullptr;
};
