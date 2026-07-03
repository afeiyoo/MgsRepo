#include "core/stationserviceapp.h"

int main(int argc, char *argv[])
{
    StationServiceApp service(argc, argv);
    return service.exec();
}
