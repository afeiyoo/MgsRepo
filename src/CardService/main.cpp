#include "core/cardserviceapp.h"

int main(int argc, char *argv[])
{
    CardServiceApp service(argc, argv);
    return service.exec();
}
