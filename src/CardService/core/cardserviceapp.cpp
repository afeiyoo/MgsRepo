#include "cardserviceapp.h"

#include <QCoreApplication>
#include <QSettings>

#include "HttpServer/httplistener.h"
#include "Logger.h"
#include "core/globalmanager.h"
#include "core/readerctrl.h"
#include "core/requestmapper.h"
#include "utils/stdafx.h"

CardServiceApp::CardServiceApp(int argc, char **argv)
    : QtService<QCoreApplication>(argc, argv, "CardService")
{
    setServiceDescription("CardService");
    setStartupType(QtServiceController::AutoStartup);
    setServiceFlags(QtServiceBase::NeedsStopOnShutdown);
}

CardServiceApp::~CardServiceApp()
{
    cleanup();
}

void CardServiceApp::start()
{
    QCoreApplication *app = application();
    if (!app) {
        logMessage("QCoreApplication is not available.", QtServiceBase::Error);
        return;
    }

    const int initResult = GM_INS->init();
    if (initResult < 0) {
        logMessage(QString("CardService init failed: %1").arg(initResult), QtServiceBase::Error);
        app->exit(initResult);
        return;
    }
    m_initialized = true;

    LOG_INFO().noquote() << "<<< Starting the application: CardService >>>";

    m_listenerSettings = new QSettings(GM_INS->m_confPath, QSettings::IniFormat);
    QString listenerGroup;
    const QStringList groups = m_listenerSettings->childGroups();
    for (const QString &group : groups) {
        if (group.compare("Listener", Qt::CaseInsensitive) == 0) {
            listenerGroup = group;
            break;
        }
    }

    if (listenerGroup.isEmpty()) {
        const QString error = "CardService listener configuration is missing.";
        LOG_ERROR().noquote() << error;
        logMessage(error, QtServiceBase::Error);
        cleanup();
        app->exit(-101);
        return;
    }

    m_listenerSettings->beginGroup(listenerGroup);
    bool portOk = false;
    const uint port = m_listenerSettings->value("port").toUInt(&portOk);
    if (!portOk || port == 0 || port > 65535) {
        const QString error = "CardService listener port is invalid.";
        LOG_ERROR().noquote() << error;
        logMessage(error, QtServiceBase::Error);
        cleanup();
        app->exit(-101);
        return;
    }

    m_requestMapper = new RequestMapper();
    m_listener = new stefanfrings::HttpListener(m_listenerSettings, m_requestMapper);

    if (!m_listener->isListening()) {
        const QString error = QString("CardService failed to listen: %1").arg(m_listener->errorString());
        LOG_ERROR().noquote() << error;
        logMessage(error, QtServiceBase::Error);
        cleanup();
        app->exit(-101);
    }
}

void CardServiceApp::stop()
{
    LOG_INFO().noquote() << "<<< Stopping the application: CardService >>>";
    cleanup();

    QCoreApplication *app = application();
    if (app)
        app->quit();
}

void CardServiceApp::cleanup()
{
    SAFE_DELETE(m_listener);

    if (m_initialized && GM_INS->m_reader)
        GM_INS->m_reader->closeReader();
    m_initialized = false;

    SAFE_DELETE(m_requestMapper);
    SAFE_DELETE(m_listenerSettings);
}
