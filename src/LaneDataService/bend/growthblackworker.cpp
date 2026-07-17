#include "growthblackworker.h"

#include "Logger.h"

GrowthBlackWorker::GrowthBlackWorker(QObject *parent)
    : QObject{parent}
{
    m_timer = new QTimer(this);
}

GrowthBlackWorker::~GrowthBlackWorker() {}

void GrowthBlackWorker::onInit()
{
    m_timer->setInterval(5 * 60 * 1000);
    m_timer->start();
}

void GrowthBlackWorker::onCheckGrowthBlack()
{
    LOG_INFO().noquote() << "开始检查增量...";
    // TODO 获取当前增量版本（数据库查询）
}
