#pragma once

#include <QObject>

#include "dataservice.h"

class DataServiceMysql : public DataService
{
    Q_OBJECT
public:
    explicit DataServiceMysql(QObject *parent = nullptr);
    ~DataServiceMysql() override;

public:
    bool init(const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName) override;

protected:
    QString getTestSql() const override;

    QString getSuccessedTradesSql(int vehicleIdentifyType, QString vehPlate, QString cardID, int dataType, int judgeTime) const override;

    QString getShiftCntSql(const QString &shiftDate, int shiftID, int laneID, int flag) const override;
};
