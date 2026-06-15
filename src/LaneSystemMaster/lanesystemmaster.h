#pragma once

#include <QObject>

#include "lanesystemgui.h"

class QApplication;

class LaneSystemMaster : public QObject
{
    Q_OBJECT
public:
    explicit LaneSystemMaster(QObject *parent = nullptr);
    ~LaneSystemMaster() override;

    void init(QApplication &app);

    void createMtcIn();
    void createMtcOut();
    void createEtc();

private:
    LaneSystemGUI m_gui;
    MtcInPageController *m_mtcinPageCtrl = nullptr;
    MtcOutPageController *m_mtcoutPageCtrl = nullptr;
    EtcPageController *m_etcPageCtrl = nullptr;
};
