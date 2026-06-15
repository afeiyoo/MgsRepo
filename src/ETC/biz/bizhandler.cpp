#include "bizhandler.h"

#include <QCoreApplication>

#include "environment.h"
#include "global/globalmanager.h"
#include "middle/signalctrl.h"

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

void BizHandler::quitSystem()
{
    if (!m_env->m_isInShifted) {
        emit GM_INSTANCE->m_sigCtrl->sigSetScrollTip("当前车道未上班");
        return;
    }
    qApp->exit(0);
}
