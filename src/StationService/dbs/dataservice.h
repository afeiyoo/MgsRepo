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

    virtual bool init(const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName) = 0;

protected:
    // 数据库连接池
    EasyQtSql::SqlFactory *m_dbFactory = nullptr;
};
