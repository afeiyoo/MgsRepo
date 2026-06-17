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
#include "json/dialogparams.h"
#include "json/infodialogparams.h"

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
        return;
    }

    DialogParams<InfoDialogRequest> params;
    params.api = API::SYSTEM_QUIT::QUERY;
    params.data.title = "请确认是否退出系统";
    params.data.strs = QStringList({"按【确认】键退出，按【返回】键取消"});
    params.data.switchLine = true;

    emit GM_INSTANCE->m_sigCtrl->sigShowDialogRequest("infoDialog", params.toJsonObj());
}

void BizHandler::quitSystemQuery()
{
    LOG_INFO().noquote() << "系统退出: " << DataDealUtils::curDateTimeStr();
    qApp->exit(0);
}
