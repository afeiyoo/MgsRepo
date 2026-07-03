#include <QCoreApplication>

#include <QSettings>

#include "HttpServer/httplistener.h"
#include "Logger.h"
#include "bend/cron.h"
#include "core/globalmanager.h"
#include "core/requestmapper.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int res = GM_INS->init();
    if (res < 0) {
        return res;
    }

    LOG_INFO().noquote() << "<<< Starting the application: StationService >>>";

    // 定时任务执行
    Cron checker;
    checker.start();

    // Http 服务
    QSettings *listenerSettings = new QSettings(GM_INS->m_confPath, QSettings::IniFormat, &a);
    listenerSettings->beginGroup("Listener");
    new stefanfrings::HttpListener(listenerSettings, new RequestMapper(&a), &a);

    int result = a.exec();
    if (result)
        LOG_WARNING().noquote() << "Something went wrong. Result code is" << result;
    return result;
}
