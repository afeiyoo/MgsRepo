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

    Q_INVOKABLE QByteArray queryInt(const QByteArray &json);

private:
    IServiceHub *m_hub = nullptr;
};
