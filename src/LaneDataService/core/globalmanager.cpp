#include "globalmanager.h"

GlobalManager::GlobalManager(QObject *parent)
    : QObject{parent}
{}

GlobalManager::~GlobalManager() {}

int GlobalManager::init()
{
    return 0;
}
