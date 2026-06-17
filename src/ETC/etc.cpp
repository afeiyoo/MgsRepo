#include "etc.h"

#include <QJsonObject>
#include <QJsonValue>

#include "IPageController.h"
#include "Logger.h"
#include "global/apis.h"
#include "global/globalmanager.h"
#include "middle/gateway.h"
#include "middle/signalctrl.h"
#include "utils/bizutils.h"
#include "json/dialogparams.h"
#include "json/infodialogparams.h"

using namespace Utils;

ETC::ETC(QObject *parent)
    : QObject{parent}
{}

ETC::~ETC() {}

int ETC::init(int argc, char *argv[])
{
    int ret = GM_INSTANCE->init(argc, argv);
    if (ret < 0 || !GM_INSTANCE->m_sigCtrl) {
        return ret;
    }

    connect(GM_INSTANCE->m_sigCtrl, &SignalCtrl::sigShowDialogRequest, this, &ETC::sigShowDialogRequest);

    return ret;
}

void ETC::bindUi(IEtcPageController *ui)
{
    m_ui = ui;
}

void ETC::onKeyPress(int key)
{
    LOG_INFO().noquote() << "按键:" << BizUtils::getKeyDescByCode(GM_INSTANCE->m_keyBoard, key);
    switch (key) {
    case Qt::Key_F1: { // 抓拍测试
        m_ui->setTradeHint("你好");
    } break;
    case Qt::Key_F7: { // 上下班
    } break;
    case Qt::Key_F8: { // 节假日模式启用/关闭
    } break;
    case Qt::Key_F10: { // 维护
    } break;
    case Qt::Key_F11: { // 班次过车数据查看
    } break;
    case Qt::Key_F12: { // 情报板内容显示选择
    } break;
    case Qt::Key_X:
    case Qt::Key_Escape: { // 系统退出
        GM_INSTANCE->m_gate->send(API::SYSTEM_QUIT::REQUEST, QJsonObject());
    } break;
    case Qt::Key_I: { // 上班
    } break;
    case Qt::Key_G:
    case Qt::Key_U: { // 下班
    } break;
    case Qt::Key_C: { // 线圈1模拟
    } break;
    case Qt::Key_V: { // 线圈2模拟
    } break;
    case Qt::Key_B: { // 线圈3模拟
    } break;
    case Qt::Key_N: { // 线圈4模拟
    } break;
    case Qt::Key_H: { // 线圈5模拟
    } break;
    case Qt::Key_J: { // 线圈6模拟
    } break;
    case Qt::Key_M: { // 抓拍线圈模拟
    } break;
    case Qt::Key_L: { // 落杆线圈模拟
    } break;
    default:
        break;
    }
}

void ETC::onShowDialogResp(const QString &dialog, const QJsonValue &values)
{
    if (dialog == "infoDialog") {
        auto resp = DialogParams<InfoDialogResponse>::fromJson(values);
        GM_INSTANCE->m_gate->send(resp.api, resp.data.toJson());
    }
}
