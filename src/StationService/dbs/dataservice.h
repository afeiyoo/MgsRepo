#pragma once

#include <QObject>

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

protected:
    // 获取数据库连接测试SQL
    virtual QString getTestSql() const = 0;

    // 获取成功交易的SQL
    virtual QString getSuccessedTradesSql(int vehicleIdentifyType, QString vehPlate, QString cardID, int dataType, int judgeTime) const = 0;

protected:
    // 数据库连接池
    EasyQtSql::SqlFactory *m_dbFactory = nullptr;
};
