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
        emit GM_INSTANCE->m_sigCtrl->sigSetTradeHint("请先下班后退出系统");
        return;
    }
    QJsonObject params;
    params["title"] = "请确认是否退出系统";
    params["strs"] = QJsonArray{"按【确认】键退出，按【返回】键取消"};
    params["switchLine"] = true;

    emit GM_INSTANCE->m_sigCtrl->sigShowFormRequest(1, API::SYSTEM_QUIT::QUERY, params);
}

void BizHandler::quitSystemQuery()
{
    LOG_INFO().noquote() << "系统退出: " << DataDealUtils::curDateTimeStr();
    qApp->exit(0);
}
