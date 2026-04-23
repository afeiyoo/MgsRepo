#include "networkutils.h"

#include <QHostInfo>

using namespace Utils;

NetworkUtils::NetworkUtils(QObject *parent)
    : QObject{parent}
{}

NetworkUtils::~NetworkUtils() {}

QStringList NetworkUtils::getLocalAddresses()
{
    QStringList addressList;
    QHostInfo hostInfo = QHostInfo::fromName(QHostInfo::localHostName());
    if (hostInfo.error() != QHostInfo::NoError) {
        return addressList;
    }
    foreach (const QHostAddress &address, hostInfo.addresses()) {
        if (!address.isLoopback() && address.protocol() == QAbstractSocket::IPv4Protocol) {
            addressList.append(address.toString());
        }
    }

    return addressList;
}
