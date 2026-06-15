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

    virtual void init(const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName) = 0;

    // 检查是否
    QString getStationIP(const QString &stationID) const;

    // 检查是否节假日免费车型
    bool isHolidayFreeVehClass(uint vehClass, bool checkVehClass = false) const;

protected:
    EasyQtSql::SqlFactory *m_dbFactory = nullptr;
};
