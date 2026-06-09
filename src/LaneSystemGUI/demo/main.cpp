#include <QApplication>
#include <QWidget>

#include "Logger.h"
#include "lanesystemgui.h"

int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    #else
    qputenv("QT_SCALE_FACTOR", "1.5");
    #endif
#endif
    QApplication a(argc, argv);
    LaneSystemGUI::initFront(a);

    QWidget *window = LaneSystemGUI::createMainWindow(PageType::Etc);
    window->show();

    int result = a.exec();
    if (result)
        LOG_WARNING().noquote() << "LaneSystemGUI 运行错误，错误码: " << result;
    return result;
}
