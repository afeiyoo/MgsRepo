#include <QApplication>
#include <QGuiApplication>

#include "lanesystemmaster.h"
#include <QDebug>

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

    LaneSystemMaster master;
    master.init(a);

    master.createEtc(argc, argv);

    return a.exec();
}
