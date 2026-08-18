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
    DeltaBlackWorker *worker = new DeltaBlackWorker();
    connect(m_td, &QThread::finished, worker, &DeltaBlackWorker::deleteLater);
    connect(m_td, &QThread::started, worker, &DeltaBlackWorker::onInit);

    worker->moveToThread(m_td);
    m_td->start();
}
