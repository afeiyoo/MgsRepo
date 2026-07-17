#include "fullblackmaster.h"

#include "core/globalmanager.h"
#include "core/signalmanager.h"
#include "fullblackworker.h"
#include "utils/stdafx.h"

FullBlackMaster::FullBlackMaster(QObject *parent)
    : QObject{parent}
{
    m_td = new QThread(this);
}

FullBlackMaster::~FullBlackMaster()
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

void FullBlackMaster::init()
{
    connect(GM_INS->m_sigMan, &SignalManager::sigUpdateFullBlackStatus, this, &FullBlackMaster::onUpdateFullBlackStatus);

    m_worker = new FullBlackWorker(); // 不能传父对象，否则无法移入子线程
    connect(m_td, &QThread::finished, m_worker, &FullBlackWorker::deleteLater);
    connect(m_td, &QThread::started, m_worker, &FullBlackWorker::onInit);

    // 子线程化
    m_worker->moveToThread(m_td);
    m_td->start();
}

void FullBlackMaster::onUpdateFullBlackStatus(const ST_FullBlackStatus &st)
{
    m_fullBlackStatus = st;
}

ST_FullBlackStatus FullBlackMaster::fullBlackStatus() const
{
    return m_fullBlackStatus;
}
