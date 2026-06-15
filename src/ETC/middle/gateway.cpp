#include "gateway.h"

#include "Logger.h"
#include "biz/bizhandler.h"
#include "global/apis.h"
#include "global/baseexception.h"
#include "global/errcode.h"
#include "global/globalmanager.h"
#include "utils/bizutils.h"

#include <QtConcurrent>

using namespace Utils;

GateWay::GateWay(QObject *parent)
    : QObject{parent}
{}

GateWay::~GateWay() {}

void GateWay::send(int api, const QJsonValue &value)
{
    QtConcurrent::run([=]() {
        try {
            this->dispatch(api, value);
        } catch (...) {
            BaseException e(EC_100000, QString::fromLocal8Bit("未知错误"));
            LOG_ERROR().noquote() << e.msg();
        }
    });
}

void GateWay::dispatch(int api, const QJsonValue &value)
{
    switch (api) {
    case API::KEY_PRESS: {
        apiKeyPress(value);
    } break;
    case API::LOGIN: {
        apiLogin(value);
    } break;
    default: {
    } break;
    }
}

void GateWay::apiLogin(const QJsonValue &value)
{
    // 执行登录 biz

    LOG_INFO().noquote() << "登录了 哈哈哈哈";
}

void GateWay::apiKeyPress(const QJsonValue &value)
{
    int key = value["key"].toInt();

    LOG_INFO().noquote() << "按键:" << BizUtils::getKeyDescByCode(GM_INSTANCE->m_keyBoard, key);
    switch (key) {
    case Qt::Key_F1: { // 抓拍测试
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
        GM_INSTANCE->m_bizHandler->quitSystem();
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
