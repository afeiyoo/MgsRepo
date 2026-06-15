#include "etc.h"

#include "global/globalmanager.h"

ETC::ETC(QObject *parent)
    : QObject{parent}
{}

ETC::~ETC() {}

int ETC::init(int argc, char *argv[])
{
    return GM_INSTANCE->init(argc, argv);
}
