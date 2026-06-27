#include <QCoreApplication>

#include <QSettings>

#include "HttpServer/httplistener.h"
#include "Logger.h"
#include "core/globalmanager.h"
#include "core/httphandler.h"
#include "utils/fileutils.h"

using namespace Utils;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int res = GM_INS->init();
    if (res < 0) {
        LOG_INFO().noquote() << "Application initialize failed! Error code is" << res;
        return res;
    }

    LOG_INFO().noquote() << "<<< Starting the application: StationService >>>";

    FileName configFile = FileName::fromString(FileUtils::curApplicationDirPath() + "/config/StationServiceCfg.ini");
    QSettings *listenerSettings = new QSettings(FileUtils::canonicalPath(configFile).toString(), QSettings::IniFormat, &a);
    listenerSettings->beginGroup("Listener");

    HttpHandler *handler = new HttpHandler(&a);
    new stefanfrings::HttpListener(listenerSettings, handler, &a);

    int result = a.exec();
    if (result)
        LOG_WARNING().noquote() << "Something went wrong. Result code is" << result;
    return result;
}
