#include "gateway.h"

#include "Logger.h"
#include "biz/bizhandler.h"
#include "global/apis.h"
#include "global/baseexception.h"
#include "global/errcode.h"
#include "global/globalmanager.h"

#include <QtConcurrent>

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
    LOG_INFO().noquote() << "API:" << api << "Value:" << value;
    switch (api) {
    case API::LOGIN: {
        apiLogin(value);
    } break;
    case API::SYSTEM_QUIT::REQUEST: {
        apiSystemQuitRequest();
    } break;
    case API::SYSTEM_QUIT::QUERY: {
        apiSystemQuitQuery(value);
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

void GateWay::apiSystemQuitRequest()
{
    GM_INSTANCE->m_bizHandler->quitSystemRequest();
}

void GateWay::apiSystemQuitQuery(const QJsonValue &value)
{
    bool res = value["res"].toBool();
    if (res) {
        GM_INSTANCE->m_bizHandler->quitSystemQuery();
    }
}
