#include "lanesystemmaster.h"

#include <QApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include "signalmanager.h"

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

void LaneSystemMaster::createEtc()
{
    m_etcPageCtrl = m_gui.createEtcWindow(false, this);
    m_etcBizCtrl = new ETC(this);
    if (m_etcBizCtrl->init(m_etcPageCtrl) < 0)
        return exit(-1);

    // 前端 => 后端
    connect(m_gui.uiSignalMan(), &SignalManager::sigKeyPress, m_etcBizCtrl, &ETC::onKeyPress);
    connect(m_gui.uiSignalMan(), &SignalManager::sigDialogResp, m_etcBizCtrl, &ETC::onDialogResp);
}
