#pragma once

#include "dataservice.h"

class DataServiceMysql : public DataService
{
    Q_OBJECT
public:
    explicit DataServiceMysql(QObject *parent = nullptr);
    ~DataServiceMysql() override;

    void init(const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName) override;
};
