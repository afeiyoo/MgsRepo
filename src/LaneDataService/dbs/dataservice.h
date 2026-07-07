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
    virtual bool init(const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName) = 0;

    // 数据库连接测试
    bool testConnection();

    // 获取数据库连接测试SQL
    virtual QString getTestSql() const = 0;

protected:
    // 数据库连接池
    EasyQtSql::SqlFactory *m_dbFactory = nullptr;
};
