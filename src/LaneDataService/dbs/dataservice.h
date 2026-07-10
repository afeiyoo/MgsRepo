#pragma once

#include <QObject>

namespace EasyQtSql {
class SqlFactory;
}
class DataService
{
public:
    DataService();
    virtual ~DataService();

    // 数据库连接初始化
    bool init(uint type, const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName);

    // 数据库连接测试
    bool testConnection(const QString &sql);

    // 查询String
    QString fetchString(const QString &sql, const QString &def);

    // 查询int
    int fetchInt(const QString &sql, const int def);

protected:
    // 数据库连接池
    EasyQtSql::SqlFactory *m_dbFactory = nullptr;
};
