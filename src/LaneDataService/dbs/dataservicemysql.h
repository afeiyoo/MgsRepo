#pragma once

#include "dataservice.h"

class DataServiceMysql : public DataService
{
public:
    DataServiceMysql();
    ~DataServiceMysql() override;

public:
    bool init(const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName) override;

    QString getTestSql() const override;
};
