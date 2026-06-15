#include "etc.h"

#include <QJsonValue>

#include "Logger.h"
#include "global/apis.h"
#include "global/globalmanager.h"
#include "middle/gateway.h"

ETC::ETC(QObject *parent)
    : QObject{parent}
{}

ETC::~ETC() {}

int ETC::init(int argc, char *argv[])
{
    return GM_INSTANCE->init(argc, argv);
}

void ETC::onKeyPress(int key)
{
    LOG_INFO().noquote() << "按键:" << key;
    switch (key) {
    case Qt::Key_F7: { // 上下班
        GM_INSTANCE->m_gate->send(API::LOGIN, QJsonValue());
    } break;
    default:
        break;
    }
}
