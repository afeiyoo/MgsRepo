#include "gateway.h"

#include <QtConcurrent>

GateWay::GateWay(QObject *parent)
    : QObject{parent}
{}

GateWay::~GateWay() {}

void GateWay::send(int api, const QJsonValue &values)
{
    QtConcurrent::run([=]() {
        try {
            dispatch(api, values);
        } catch (...) {
        }
    });
}

void GateWay::dispatch(int api, const QJsonValue &values) {}
