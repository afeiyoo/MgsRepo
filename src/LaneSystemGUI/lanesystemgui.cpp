#include "lanesystemgui.h"

#include <QApplication>
#include <QCoreApplication>
#include <QFont>
#include <QWidget>

#include "ElaApplication.h"
#include "Logger.h"
#include "global/globalmanager.h"
#include "pages/mainwindow.h"
#include "utils/datadealutils.h"

using namespace Utils;

namespace LaneSystemGUI
{
void initFront(QApplication &app)
{
    eApp->init();
    GM_INSTANCE->init();

    app.setFont(QFont("Microsoft YaHei UI"));

    QObject::connect(&app, &QCoreApplication::aboutToQuit, []() {
        LOG_INFO().noquote() << "LaneSystemGUI 退出:" << DataDealUtils::curDateTimeStr();
    });

    LOG_INFO().noquote() << "LaneSystemGUI 启动:" << DataDealUtils::curDateTimeStr();
}

QWidget *createMainWindow(PageType page, QWidget *parent)
{
    auto *window = new MainWindow(parent);
    switch (page)
    {
    case PageType::MtcIn:
        window->initMtcIn();
        break;
    case PageType::MtcOut:
        window->initMtcOut();
        break;
    case PageType::Etc:
    default:
        window->initEtc();
        break;
    }
    return window;
}
} // namespace LaneSystemGUI
