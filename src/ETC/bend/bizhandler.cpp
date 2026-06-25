#include "bizhandler.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>

#include "Logger.h"
#include "dao/configs/config.h"
#include "environment.h"
#include "global/apis.h"
#include "global/etcdefs.h"
#include "global/globalmanager.h"
#include "middle/signalctrl.h"
#include "utils/datadealutils.h"

using namespace Utils;

BizHandler::BizHandler(QObject *parent)
    : QObject{parent}
{}

BizHandler::~BizHandler() {}

bool BizHandler::isShiftedStarted()
{
    return GM_INSTANCE->m_env->m_isShiftStarted;
}

void BizHandler::systemInit()
{
    // 界面设备图标栏初始化
    QList<uint> devs;
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QStringList devStrs = GM_INSTANCE->m_conf->m_showDevsList.split("-", Qt::SkipEmptyParts);
#else
    QStringList devStrs = GM_INSTANCE->m_conf->m_showDevsList.split("-", QString::SkipEmptyParts);
#endif
    for (const QString &str : devStrs) {
        devs.append(str.toUInt());
    }
    emit GM_INSTANCE->m_sigCtrl->sigUpdateDevsList(devs);

    // 界面交易提示初始化
    emit GM_INSTANCE->m_sigCtrl->sigUpdateTradeHint("等待用户上班");
    emit GM_INSTANCE->m_sigCtrl->sigUpdateHelpHint("按【F7】上班");
}

void BizHandler::quitSystemRequest()
{
    if (GM_INSTANCE->m_env->m_isShiftStarted) {
        emit GM_INSTANCE->m_sigCtrl->sigUpdateTradeHint("当前正在上班，无法退出系统");
        emit GM_INSTANCE->m_sigCtrl->sigUpdateHelpHint("请按【F7】操作下班");
        return;
    }

    emit GM_INSTANCE->m_sigCtrl->sigShowInfoDialog(API::SYSTEM_QUIT::QUERY, "请确认是否退出系统?", {"按【确认】键退出，按【返回】键取消"});
}

void BizHandler::quitSystemQuery()
{
    LOG_INFO().noquote() << "系统退出: " << DataDealUtils::curDateTimeStr();
    qApp->exit(0);
}

void BizHandler::exchangeShiftRequest()
{
    if (GM_INSTANCE->m_env->m_isShiftStarted) {
        emit GM_INSTANCE->m_sigCtrl->sigShowInfoDialog(API::EXCHANGE_SHIFT::END_SHIFT_QUERY, "请确认是否下班?",
                                                       {"按【确认】键下班，按【返回】键取消"});
    } else {
        if (GM_INSTANCE->m_conf->m_isTest) { // 测试模式

        } else {
            // TODO 车道授权检查
        }
    }
}

void BizHandler::endShift()
{
    LOG_INFO().noquote() << "下班:" << DataDealUtils::curDateTimeStr();
}
