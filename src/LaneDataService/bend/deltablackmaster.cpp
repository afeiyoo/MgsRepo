#include "deltablackmaster.h"

#include "deltablackworker.h"

DeltaBlackMaster::DeltaBlackMaster(QObject *parent)
    : QObject{parent}
{
    m_td = new QThread(this);
}

DeltaBlackMaster::~DeltaBlackMaster()
{
    if (m_td->isRunning()) {
        m_td->quit();
        if (!m_td->wait(1000)) {
            m_td->terminate();
            m_td->wait(1000);
        }
    }
}

void DeltaBlackMaster::init()
{
    m_worker = new DeltaBlackWorker();
    connect(m_td, &QThread::finished, m_worker, &DeltaBlackWorker::deleteLater);
    connect(m_td, &QThread::started, m_worker, &DeltaBlackWorker::onInit);

    m_worker->moveToThread(m_td);
    m_td->start();
}
