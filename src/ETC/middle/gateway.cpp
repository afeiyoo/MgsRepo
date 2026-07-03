#include "gateway.h"

#include "Logger.h"
#include "bend/bizhandler.h"
#include "bend/environment.h"
#include "global/apis.h"
#include "global/baseexception.h"
#include "global/errcode.h"
#include "global/globalmanager.h"

#include <QtConcurrent>

GateWay::GateWay(QObject *parent)
    : QObject{parent}
{}

GateWay::~GateWay() {}

void GateWay::send(int api, const QJsonValue &values)
{
    QtConcurrent::run([=]() {
        try {
            LOG_DEBUG().noquote() << "API:" << api << "Json:" << values.toObject();
            switch (api) {
            case API::EXCHANGE_SHIFT::REQUEST: {
                apiExchangeShiftRequest();
            } break;
            case API::EXCHANGE_SHIFT::END_SHIFT_QUERY: {
                apiEndShift();
            } break;
            case API::SYSTEM_QUIT::REQUEST: {
                apiSystemQuitRequest();
            } break;
            case API::SYSTEM_QUIT::QUERY: {
                apiSystemQuitQuery(values);
            } break;
            case API::TEST_CAP::REQUEST: {
            } break;
            case API::INIT::REQUEST: {
                apiSystemInit();
            } break;
            default: {
            } break;
            }
        } catch (...) {
            BaseException e(EC_100000, QString::fromLocal8Bit("未知错误"));
            LOG_ERROR().noquote() << e.msg();
        }
    });
}

void GateWay::apiLogin(const QJsonValue &values)
{
    // 执行登录 biz

    LOG_INFO().noquote() << "登录了 哈哈哈哈";
}

void GateWay::apiSystemInit()
{
    GM_INSTANCE->m_bizHandler->systemInit();
}

void GateWay::apiSystemQuitRequest()
{
    GM_INSTANCE->m_bizHandler->quitSystemRequest();
}

void GateWay::apiSystemQuitQuery(const QJsonValue &values)
{
    bool yes = values["yes"].toBool();
    if (yes)
        GM_INSTANCE->m_bizHandler->quitSystemQuery();
}

void GateWay::apiExchangeShiftRequest()
{
    GM_INSTANCE->m_bizHandler->exchangeShiftRequest();
}

void GateWay::apiEndShift() {}
