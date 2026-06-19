#include "bizhandler.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>

#include "Logger.h"
#include "environment.h"
#include "global/apis.h"
#include "global/globalmanager.h"
#include "middle/signalctrl.h"
#include "utils/datadealutils.h"

using namespace Utils;

BizHandler::BizHandler(QObject *parent)
    : QObject{parent}
{
    m_env = new Environment(this);
}

BizHandler::~BizHandler() {}

bool BizHandler::isInShifted()
{
    return m_env->m_isInShifted;
}

void BizHandler::quitSystemRequest()
{
    if (m_env->m_isInShifted) {
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
