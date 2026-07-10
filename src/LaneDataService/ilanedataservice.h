#pragma once

#include "lanedataservice_global.h"

class IServiceHub;
class LANEDATASERVICE_EXPORT ILaneDataService
{
public:
    ILaneDataService() = default;
    virtual ~ILaneDataService() = default;

    virtual int init() = 0;
};

extern "C" LANEDATASERVICE_EXPORT ILaneDataService *createLaneDataService(IServiceHub *hub);
