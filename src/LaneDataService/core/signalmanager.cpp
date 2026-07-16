#include "signalmanager.h"

SignalManager::SignalManager(QObject *parent)
    : QObject{parent}
{
    qRegisterMetaType<ST_FullBlackStatus>("ST_FullBlackStatus");
}
