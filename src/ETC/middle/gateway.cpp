#include "gateway.h"

#include "Logger.h"
#include "biz/bizhandler.h"
#include "global/apis.h"
#include "global/baseexception.h"
#include "global/errcode.h"
#include "global/globalmanager.h"
#include "json/infodialogparams.h"

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
            case API::LOGIN: {
                apiLogin(values);
            } break;
            case API::SYSTEM_QUIT::REQUEST: {
                apiSystemQuitRequest();
            } break;
            case API::SYSTEM_QUIT::QUERY: {
                apiSystemQuitQuery(values);
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

void GateWay::apiSystemQuitRequest()
{
    GM_INSTANCE->m_bizHandler->quitSystemRequest();
}

void GateWay::apiSystemQuitQuery(const QJsonValue &values)
{
    InfoDialogResponse resp = InfoDialogResponse::fromJson(values.toObject());
    if (resp.yes)
        GM_INSTANCE->m_bizHandler->quitSystemQuery();
}
