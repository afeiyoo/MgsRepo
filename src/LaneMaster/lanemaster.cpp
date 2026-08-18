#include "lanemaster.h"

#include <QApplication>
#include <QCoreApplication>
#include <QFont>
#include <QFontDatabase>
#include <QImage>
#include <QWidget>

#include "ElaApplication.h"
#include "global/globalmanager.h"
#include "global/laneui.h"
#include "global/uiconst.h"
#include "ilanedataservice.h"
#include "iservicehub.h"
#include "pages/mainwindow.h"
#include "utils/stdafx.h"

#include <QDebug>

namespace {
QFont resolveAppFont()
{
    static const QString kFont = Path::DISPLAY_FONT;

    int fontId = QFontDatabase::addApplicationFont(kFont);

    if (fontId >= 0) {
        QStringList families = QFontDatabase::applicationFontFamilies(fontId);

        if (!families.isEmpty())
            return QFont(families.first());
    }

    return QFont(QStringLiteral("Sans Serif"));
}
} // namespace

LaneMaster::LaneMaster(QObject *parent)
    : QObject{parent}
{}

LaneMaster::~LaneMaster()
{
    m_hub->unregisterService("LaneUI");
    m_hub->unregisterService("LaneDataService");
    SAFE_DELETE(m_hub);

    SAFE_DELETE(m_mainWindow);
    SAFE_DELETE(m_ui);
}

void LaneMaster::initLane(QApplication &app)
{
    eApp->init();
    GM_INSTANCE->init();
    app.setFont(resolveAppFont());

    // TODO 根据车道类型，创建车道
    m_hub = createServiceHub();

    ILaneDataService *ds = createLaneDataService(m_hub);
    ds->init();

    qDebug().noquote() << m_hub->serviceNames() << m_hub->methodNames("LaneDataService");
    CallResult res1 = m_hub->call("LaneDataService.queryInt", R"({})");
    if (res1.isSuccess()) {
        qDebug().noquote() << res1.data;
    } else {
        qDebug().noquote() << res1.code << res1.message;
    }
}

LaneUI *LaneMaster::createMtcInUI(bool isMaxShow, QObject *parent)
{
    if (m_mainWindow)
        delete m_mainWindow;

    m_mainWindow = new MainWindow();
    m_mainWindow->initMtcIn();
    if (isMaxShow) {
        m_mainWindow->showMaximized();
    } else {
        m_mainWindow->showNormal();
    }

    return new LaneUI(m_mainWindow->mtcInPage(), parent);
}

LaneUI *LaneMaster::createMtcOutUI(bool isMaxShow, bool isSptShow, QObject *parent)
{
    if (m_mainWindow)
        delete m_mainWindow;

    m_mainWindow = new MainWindow();
    m_mainWindow->initMtcOut(isSptShow);
    if (isMaxShow) {
        m_mainWindow->showMaximized();
    } else {
        m_mainWindow->showNormal();
    }

    return new LaneUI(m_mainWindow->mtcOutPage(), parent);
}

LaneUI *LaneMaster::createEtcUI(bool isMaxShow, QObject *parent)
{
    if (m_mainWindow)
        delete m_mainWindow;

    m_mainWindow = new MainWindow();
    m_mainWindow->initEtc();
    if (isMaxShow) {
        m_mainWindow->showMaximized();
    } else {
        m_mainWindow->showNormal();
    }

    return new LaneUI(m_mainWindow->etcPage(), parent);
}
