#include <QApplication>

#include "ElaApplication.h"
#include "Logger.h"
#include "global/globalmanager.h"
#include "pages/mainwindow.h"
#include "utils/datadealutils.h"

using namespace Utils;

int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    #else
    // 根据实际屏幕缩放比例更改
    qputenv("QT_SCALE_FACTOR", "1.5");
    #endif
#endif
    QApplication a(argc, argv);

    eApp->init();
    GM_INSTANCE->init();
    QFont font("Microsoft YaHei UI");
    a.setFont(font);

    QObject::connect(&a, &QCoreApplication::aboutToQuit, [=]() { LOG_INFO().noquote() << "MgsToolsKit 退出:" << DataDealUtils::curDateTimeStr(); });

    LOG_INFO().noquote() << "LaneSystemGUI 启动:" << DataDealUtils::curDateTimeStr();

    MainWindow w;
    w.initEtc();
    w.showMaximized();

    int result = a.exec();
    if (result)
        LOG_WARNING().noquote() << "LaneSystemGUI 运行错误，错误码: " << result;
    return result;
}
