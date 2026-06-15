#include "gateway.h"

#include "Logger.h"
#include "global/baseexception.h"
#include "global/errcode.h"

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

void GateWay::dispatch(int api, const QJsonValue &value) {}
