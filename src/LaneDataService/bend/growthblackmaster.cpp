#include "growthblackmaster.h"

#include <QThread>

#include "growthblackworker.h"
#include "utils/stdafx.h"

GrowthBlackMaster::GrowthBlackMaster(QObject *parent)
    : QObject{parent}
{
    m_td = new QThread(this);
}

GrowthBlackMaster::~GrowthBlackMaster()
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

void GrowthBlackMaster::init()
{
    m_worker = new GrowthBlackWorker();
    connect(m_td, &QThread::finished, m_worker, &GrowthBlackWorker::deleteLater);
    connect(m_td, &QThread::started, m_worker, &GrowthBlackWorker::onInit);

    m_worker->moveToThread(m_td);
    m_td->start();
}
