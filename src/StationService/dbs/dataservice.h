#pragma once

#include <QObject>
#include <QVariant>

namespace EasyQtSql {
class SqlFactory;
}

class DataService : public QObject
{
    Q_OBJECT
public:
    explicit DataService(QObject *parent = nullptr);
    ~DataService() override;

    // 数据库连接初始化
    virtual bool init(const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName) = 0;

    // 测试数据库连接
    bool testConnection() const;

    // 检查是否已交易
    QVariantList fetchSuccessedTrades(int vehicleIdentifyType, QString vehPlate, QString cardID, int dataType, int judgeTime) const;

    // 获取符合条件的记录数
    int fetchRecordCnt(const QVariantMap &kvs, const QString &tableName, const QString &tableKey) const;

    // 更新记录
    bool updateRecord(const QVariantMap &kvs, const QString &tableName, const QString &tableKey) const;

    // 插入记录
    bool insertRecord(const QVariantMap &kvs, const QString &tableName) const;

    // 获取厦漳大桥交易趟次
    int fetchXZPassTimes(const QString &sql) const;

    // 获取班次流水数量
    int fetchShiftCnt(const QString &shiftDate, int shiftID, int laneID, int flag) const;

    // 查询数量
    int queryInt(const QString &sql, bool *ok) const;

    // 查询字符串
    QString queryString(const QString &sql, bool *ok) const;

    // 查询键值对
    QVariantMap queryMap(const QString &sql, bool *ok) const;

    // 查询列表
    QVariantList queryList(const QString &sql, bool *ok) const;

protected:
    // 获取数据库连接测试SQL
    virtual QString getTestSql() const = 0;

    // 获取成功交易的SQL
    virtual QString getSuccessedTradesSql(int vehicleIdentifyType, QString vehPlate, QString cardID, int dataType, int judgeTime) const = 0;

    // 获取班次流水数量的SQL
    virtual QString getShiftCntSql(const QString &shiftDate, int shiftID, int laneID, int flag) const = 0;

private:
    QString findMapKeyCaseInsensitive(const QVariantMap &map, const QString &key) const;
    QString valueToSqlLiteral(const QVariant &value) const;

protected:
    // 数据库连接池
    EasyQtSql::SqlFactory *m_dbFactory = nullptr;
};
