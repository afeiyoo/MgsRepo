#include "fullblackchecker.h"

#include <QMetaObject>
#include <QThread>

#include "core/globalmanager.h"
#include "core/signalmanager.h"
#include "fullblackworker.h"
#include "utils/stdafx.h"

FullBlackChecker::FullBlackChecker(QObject *parent)
    : QObject{parent}
{
    m_td = new QThread(this);
    m_timer = new QTimer(this);
}

FullBlackChecker::~FullBlackChecker()
{
    if (m_td->isRunning()) {
        m_td->quit();
        if (!m_td->wait(1000)) {
            m_td->terminate();
            m_td->wait(1000);
        }
    }

    SAFE_DELETE(m_td);
}

void FullBlackChecker::init()
{
    connect(GM_INS->m_sigMan, &SignalManager::sigUpdateFullBlackStatus, this, &FullBlackChecker::onUpdateFullBlackStatus);
    connect(GM_INS->m_sigMan, &SignalManager::sigUpdateFullBlackVersion, this, &FullBlackChecker::onUpdateFullBlackVersion);

    m_worker = new FullBlackWorker();
    connect(m_td, &QThread::finished, m_worker, &FullBlackWorker::deleteLater); // m_worker属于工作线程，应该由工作线程销毁
    m_worker->moveToThread(m_td);
    m_td->start();

    // 程序启动，检查全量文件，之后每隔10分钟检查一次全量更新
    emit GM_INS->m_sigMan->sigCheckFullBlack(true, m_fullBlackStatus);

    m_timer->setInterval(10 * 60 * 1000);
    connect(m_timer, &QTimer::timeout, this, [this]() { emit GM_INS->m_sigMan->sigCheckFullBlack(false, m_fullBlackStatus); });
    m_timer->start();
}

void FullBlackChecker::onUpdateFullBlackStatus(bool status, const QString &desc)
{
    m_fullBlackStatus = status;
    m_fullBlackDesc = desc;
}

void FullBlackChecker::onUpdateFullBlackVersion(const QString &version)
{
    m_fullBlackVersion = version;
}
