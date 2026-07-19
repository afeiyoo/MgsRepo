#include "fullblackmaster.h"

#include "fullblackworker.h"

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
}

void FullBlackMaster::init()
{
    m_worker = new FullBlackWorker(); // 不能传父对象，否则无法移入子线程
    connect(m_td, &QThread::finished, m_worker, &FullBlackWorker::deleteLater);
    connect(m_td, &QThread::started, m_worker, &FullBlackWorker::onInit);

    // 子线程化
    m_worker->moveToThread(m_td);
    m_td->start();
}
