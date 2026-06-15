#pragma once

#include "dataservice.h"

class DataServiceDameng : public DataService
{
    Q_OBJECT
public:
    explicit DataServiceDameng(QObject *parent = nullptr);
    ~DataServiceDameng() override;

    bool init(const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName) override;

signals:
};
