#include "stationserviceapp.h"

#include <QCoreApplication>
#include <QSettings>
#include <QTimer>

#include "HttpServer/httplistener.h"
#include "Logger.h"
#include "bend/cron.h"
#include "core/globalmanager.h"
#include "core/requestmapper.h"
#include "utils/stdafx.h"

StationServiceApp::StationServiceApp(int argc, char **argv)
    : QtService<QCoreApplication>(argc, argv, "StationService")
{
    setServiceDescription("StationService");
    setStartupType(QtServiceController::AutoStartup);
    setServiceFlags(QtServiceBase::NeedsStopOnShutdown);
}

StationServiceApp::~StationServiceApp()
{
    cleanup();
}

void StationServiceApp::start()
{
    QCoreApplication *app = application();
    if (!app) {
        logMessage("QCoreApplication is not available.", QtServiceBase::Error);
        return;
    }

    int res = GM_INS->init();
    if (res < 0) {
        logMessage(QString("StationService init failed: %1").arg(res), QtServiceBase::Error);
        app->exit(res);
        return;
    }

    LOG_INFO().noquote() << "<<< Starting the application: StationService >>>";

    m_checker = new Cron();
    m_checker->start();

    m_listenerSettings = new QSettings(GM_INS->m_confPath, QSettings::IniFormat);
    m_listenerSettings->beginGroup("Listener");
    m_requestMapper = new RequestMapper();
    m_listener = new stefanfrings::HttpListener(m_listenerSettings, m_requestMapper);
}

void StationServiceApp::stop()
{
    LOG_INFO().noquote() << "<<< Stopping the application: StationService >>>";
    cleanup();

    QCoreApplication *app = application();
    if (app)
        app->quit();
}

void StationServiceApp::cleanup()
{
    SAFE_DELETE(m_listener);
    SAFE_DELETE(m_checker);
    SAFE_DELETE(m_requestMapper);
    SAFE_DELETE(m_listenerSettings);
}
