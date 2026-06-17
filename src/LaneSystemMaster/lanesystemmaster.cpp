#include "lanesystemmaster.h"

#include <QApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include "json/dialogparams.h"
#include "json/infodialogparams.h"

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

    // 前端 => 后端
    connect(m_etcPageCtrl, &EtcPageController::sigKeyPress, m_etcBizCtrl, &ETC::onKeyPress);
    connect(m_etcPageCtrl, &EtcPageController::sigShowDialogResp, m_etcBizCtrl, &ETC::onShowDialogResp);

    // 前端 <= 后端
    connect(m_etcBizCtrl, &ETC::sigShowDialogRequest, m_etcPageCtrl, [=](const QString &dialog, const QJsonValue &values) {
        if (dialog == "infoDialog") {
            auto resp = DialogParams<InfoDialogRequest>::fromJson(values);

            m_etcPageCtrl->setApi(resp.api);

            QString title = resp.data.title;
            QStringList strs = resp.data.strs;
            bool switchLine = resp.data.switchLine;
            m_etcPageCtrl->showInfoDialog(title, strs, switchLine);
        }
    });
}
