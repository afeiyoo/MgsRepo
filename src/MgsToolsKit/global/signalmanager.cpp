#include "signalmanager.h"

SignalManager::SignalManager(QObject *parent)
    : QObject{parent}
{
    qRegisterMetaType<ST_CardRobotStatusInfo>("ST_CardRobotStatusInfo");
}

SignalManager::~SignalManager() {}
