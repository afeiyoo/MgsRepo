#include "fullblackmaster.h"

#include "core/globalmanager.h"
#include "core/signalmanager.h"
#include "fullblackworker.h"
#include "utils/stdafx.h"

FullBlackMaster::FullBlackMaster(QObject *parent)
    : QObject{parent}
{
    m_td = new QThread(this);
    m_timer = new QTimer(this);
}

FullBlackMaster::~FullBlackMaster()
{
    m_timer->stop();
    if (m_td->isRunning()) {
        m_td->quit();
        if (!m_td->wait(1000)) {
            m_td->terminate();
            m_td->wait(1000);
        }
    }
    SAFE_DELETE(m_td);
}

void FullBlackMaster::init()
{
    m_worker = new FullBlackWorker(); // 不能传父对象，否则无法移入子线程
    connect(m_td, &QThread::finished, m_worker, &FullBlackWorker::deleteLater);
    connect(m_td, &QThread::started, m_worker, &FullBlackWorker::onInit);

    connect(GM_INS->m_sigMan, &SignalManager::sigUpdateFullBlackStatus, this, &FullBlackMaster::onUpdateFullBlackStatus);

    // 子线程化
    m_worker->moveToThread(m_td);
    m_td->start();

    // 每隔10分钟检查一次全量
    m_timer->setInterval(10 * 60 * 1000);
    connect(m_timer, &QTimer::timeout, m_worker, &FullBlackWorker::onCheckFullBlack);
    m_timer->start();
}

void FullBlackMaster::onUpdateFullBlackStatus(const ST_FullBlackStatus &st)
{
    m_fullBlackStatus = st;
}

ST_FullBlackStatus FullBlackMaster::fullBlackStatus() const
{
    return m_fullBlackStatus;
}
