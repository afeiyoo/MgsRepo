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
    m_worker = new FullBlackWorker();
    connect(m_td, &QThread::finished, m_worker, &FullBlackWorker::deleteLater); // m_worker属于工作线程，应该由工作线程销毁
    m_worker->moveToThread(m_td);
    m_td->start();

    // 程序启动，先加载当前版本全量
    emit GM_INS->m_sigMan->sigCheckCurFullBlack();

    // 之后每隔10分钟检查一次全量更新
    m_timer->setInterval(10 * 60 * 1000);
    connect(m_timer, &QTimer::timeout, this, []() { emit GM_INS->m_sigMan->sigCheckNewFullBlack(); });
    m_timer->start();
}

void FullBlackChecker::onUpdateFullBlackStatus(int status)
{
    m_fullBlackStatus = status;
}
