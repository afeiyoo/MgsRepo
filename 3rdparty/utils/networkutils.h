#pragma once

#include <QObject>

namespace Utils {

class NetworkUtils : public QObject
{
    Q_OBJECT
public:
    explicit NetworkUtils(QObject *parent = nullptr);
    ~NetworkUtils() override;

    // 获取本机所有的IP地址
    static QStringList getLocalAddresses();

    // 拼接URL
    static QUrl appendUrlPath(const QUrl &baseUrl, const QString &pathPart);

    // 检查是否合法IP地址
    static bool isIpV4Address(const QString &str);
};

} // namespace Utils
