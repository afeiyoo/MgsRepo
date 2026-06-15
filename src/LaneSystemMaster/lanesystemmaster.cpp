#include "lanesystemmaster.h"

#include <QApplication>

LaneSystemMaster::LaneSystemMaster(QObject *parent)
    : QObject{parent}
{}

LaneSystemMaster::~LaneSystemMaster() {}

void LaneSystemMaster::init(QApplication &app)
{
    m_gui.initFront(app);
}

void LaneSystemMaster::createMtcIn() {}

void LaneSystemMaster::createMtcOut() {}

void LaneSystemMaster::createEtc() {}
