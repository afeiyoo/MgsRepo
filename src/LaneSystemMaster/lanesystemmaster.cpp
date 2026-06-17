#include "lanesystemmaster.h"

#include <QApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

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

void LaneSystemMaster::createEtc(int argc, char *argv[])
{
    m_etcPageCtrl = m_gui.createEtcWindow(false, this);
    m_etcBizCtrl = new ETC(this);
    if (m_etcBizCtrl->init(argc, argv) < 0)
        return exit(-1);
    m_etcBizCtrl->bindUi(m_etcPageCtrl);

    // 前端 => 后端
    connect(m_etcPageCtrl, &EtcPageController::sigKeyPress, m_etcBizCtrl, &ETC::onKeyPress);
}
