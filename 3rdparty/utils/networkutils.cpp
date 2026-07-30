#include "networkutils.h"

#include <QHostInfo>
#include <QUrl>

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

QUrl NetworkUtils::appendUrlPath(const QUrl &baseUrl, const QString &pathPart)
{
    QUrl url(baseUrl);

    QString path = url.path();
    if (!path.endsWith('/'))
        path.append('/');

    path.append(pathPart);
    url.setPath(path);

    return url;
}
