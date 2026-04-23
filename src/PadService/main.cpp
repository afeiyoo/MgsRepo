#include <QCoreApplication>

#include "HttpServer/httplistener.h"
#include "Logger.h"
#include "bend/httphandler.h"
#include "global/globalmanager.h"
#include "utils/fileutils.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    GM_INSTANCE->init();

    LOG_INFO().noquote() << "<<< Starting the application >>>";
    Utils::FileName configFile = Utils::FileName::fromString(Utils::FileUtils::curApplicationDirPath() + "/config/config.ini");
    QSettings setting(configFile.toString(), QSettings::IniFormat, &app);
    setting.beginGroup("listener");

    HttpHandler handler(&app);
    stefanfrings::HttpListener listener(&setting, &handler, &app);

    int result = app.exec();
    if (result)
        LOG_WARNING().noquote() << "Something went wrong. Result code is " << result;
    return result;
}
