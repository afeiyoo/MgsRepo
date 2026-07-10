#pragma once

#include <QObject>

#include "ilanedataservice.h"

class IServiceHub;
class LaneDataService : public QObject, public ILaneDataService
{
    Q_OBJECT
public:
    LaneDataService(IServiceHub *hub);
    ~LaneDataService() override;

    int init() override;

    Q_INVOKABLE QByteArray fetchString(const QByteArray &json);
    Q_INVOKABLE QByteArray fetchInt(const QByteArray &json);
    Q_INVOKABLE QByteArray fetchReal(const QByteArray &json);
    Q_INVOKABLE QByteArray fetchList(const QByteArray &json);
    Q_INVOKABLE QByteArray fetchMap(const QByteArray &json);
    Q_INVOKABLE QByteArray fetchDate(const QByteArray &json);
    // NOTE：不支持整表更新
    Q_INVOKABLE QByteArray updateRecord(const QByteArray &json);
    Q_INVOKABLE QByteArray insertRecord(const QByteArray &json);
    // NOTE：不支持整表删除
    Q_INVOKABLE QByteArray deleteRecord(const QByteArray &json);
    // 整表删除
    Q_INVOKABLE QByteArray truncateTable(const QByteArray &json);

private:
    IServiceHub *m_hub = nullptr;
};
