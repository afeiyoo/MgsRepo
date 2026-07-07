#include "lanedataservice.h"

#include "core/globalmanager.h"

LaneDataService::LaneDataService() {}

LaneDataService::~LaneDataService() {}

int LaneDataService::init()
{
    GM_INS->init();
}
