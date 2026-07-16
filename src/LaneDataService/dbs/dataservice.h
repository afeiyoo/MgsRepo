#pragma once

#include <QObject>
#include <QReadWriteLock>

namespace EasyQtSql {
class SqlFactory;
}
class DataService : public QObject
{
    Q_OBJECT
public:
    explicit DataService(QObject *parent = nullptr);
    ~DataService() override;

    // 车道数据库连接初始化
    bool init(uint type, const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName);

    // 数据库连接测试
    bool testConnection(const QString &connName, const QString &sql);

    // 查询记录String
    QString fetchString(const QString &sql, const QVariantMap &params, const QString &def);

    // 查询记录int
    int fetchInt(const QString &sql, const QVariantMap &params, const int def);

    // 执行更新 返回值>=0表示影响行数，<0表示执行失败
    int updateRecords(const QString &table, const QVariantMap &updateParams, const QString &whereClause);

    // 执行插入 返回值>=0表示影响行数，<0表示执行失败
    int insertRecords(const QString &table, const QVariantMap &insertParams);

    // 执行删除 返回值>=0表示影响行数，<0表示执行失败
    int deleteRecords(const QString &table, const QString &whereClause);

    // 整表删除（不删除表结构） 返回值>=0表示影响行数，<0表示执行失败
    int truncateTable(const QString &table);

public slots:
    bool cleanETCBlackCard(const QString &table);

private:
    // 数据库连接池
    EasyQtSql::SqlFactory *m_dbFactory = nullptr;
};
