#pragma once

#include <QObject>

#include "dataservice.h"

class DataServiceSqlServer : public DataService
{
    Q_OBJECT
public:
    explicit DataServiceSqlServer(QObject *parent = nullptr);
    ~DataServiceSqlServer() override;

public:
    bool init(const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName) override;

protected:
    QString getTestSql() const override;

    QString getSuccessedTradesSql(int vehicleIdentifyType, QString vehPlate, QString cardID, int dataType, int judgeTime) const override;
};
