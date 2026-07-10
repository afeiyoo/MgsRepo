#include "lanedataservice.h"

#include "core/globalmanager.h"
#include "iservicehub.h"

LaneDataService::LaneDataService(IServiceHub *hub)
    : m_hub(hub)
{
    m_hub->registerService(this);
}

LaneDataService::~LaneDataService() {}

int LaneDataService::init()
{
    return GM_INS->init(m_hub);
}

QByteArray LaneDataService::queryInt(const QByteArray &json)
{
    return R"({"Hello": "world"})";
}

ILaneDataService *createLaneDataService(IServiceHub *hub)
{
    return new LaneDataService(hub);
}
