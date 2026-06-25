#include "signalctrl.h"

SignalCtrl::SignalCtrl(QObject *parent)
    : QObject{parent}
{
    qRegisterMetaType<QList<uint>>("QList<uint>");
}
