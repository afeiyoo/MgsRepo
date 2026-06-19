#include "pagehandler.h"

#include "IPageController.h"
#include "global/globalmanager.h"
#include "middle/signalctrl.h"

PageHandler::PageHandler(QObject *parent)
    : QObject{parent}
{}

PageHandler::~PageHandler() {}

void PageHandler::init(IEtcPageController *newUi)
{
    m_ui = newUi;

    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigUpdateTradeHint, this, &PageHandler::onUpdateTradeHint);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigUpdateHelpHint, this, &PageHandler::onUpdateHelpHint);
    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigShowInfoDialog, this, &PageHandler::onShowInfoDialog);
}

void PageHandler::onUpdateTradeHint(const QString &info, bool isWarn) const
{
    if (m_ui)
        m_ui->setTradeHint(info, isWarn);
}

void PageHandler::onUpdateHelpHint(const QString &info) const
{
    if (m_ui)
        m_ui->setScrollTip(info);
}

void PageHandler::onShowInfoDialog(int api, const QString &title, const QStringList &strs, bool switchLine)
{
    if (m_ui) {
        m_ui->setApi(api);
        m_ui->showInfoDialog(title, strs, switchLine);
    }
}
