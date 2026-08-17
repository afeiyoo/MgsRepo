#include <QApplication>

#include "ElaApplication.h"
#include "Logger.h"
#include "global/globalmanager.h"
#include "mainwindow.h"
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

    QObject::connect(&a, &QCoreApplication::aboutToQuit, [=]() { LOG_INFO().noquote() << "MgsToolsKit 退出:" << DataDealUtils::curDateTimeStr(); });

    MainWindow w;
    w.show();

    LOG_INFO().noquote() << "MgsToolsKit 启动:" << DataDealUtils::curDateTimeStr();

    int result = a.exec();
    if (result)
        LOG_WARNING().noquote() << "MgsToolsKit 运行错误，错误码: " << result;
    return result;
}
