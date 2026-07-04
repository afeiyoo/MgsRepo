#include <QCoreApplication>

#include "HttpServer/httplistener.h"
#include "Logger.h"
#include "core/globalmanager.h"
#include "core/httphandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    int res = GM_INSTANCE->init();
    if (res < 0)
        return res;

    LOG_INFO().noquote() << "<<< Starting the application >>>";
    QSettings setting(GM_INSTANCE->m_confPath, QSettings::IniFormat, &app);
    setting.beginGroup("listener");

    HttpHandler handler(&app);
    stefanfrings::HttpListener listener(&setting, &handler, &app);

    int result = app.exec();
    if (result)
        LOG_WARNING().noquote() << "Something went wrong. Result code is " << result;
    return result;
}
