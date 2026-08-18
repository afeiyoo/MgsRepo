#pragma once

#include "iservicehub.h"

#include <QHash>
#include <QMetaMethod>
#include <QPointer>
#include <QReadWriteLock>

#include <functional>

class ServiceHub : public IServiceHub
{
public:
    ServiceHub();
    ~ServiceHub() override;

    ServiceHub(const ServiceHub &) = delete;
    ServiceHub &operator=(const ServiceHub &) = delete;

    bool registerService(QObject *service, const QString &serviceName = QString()) override;
    bool unregisterService(const QString &serviceName) override;
    bool hasService(const QString &serviceName) const override;
    QStringList serviceNames() const override;
    QStringList methodNames(const QString &serviceName = QString()) const override;

    CallResult call(const QString &fullMethodName, const QByteArray &json = QByteArray()) const override;

    QString lastError() const override;

private:
    using MethodHandler = std::function<CallResult(const QByteArray &json)>;

    struct ServiceRecord
    {
        QObject *service = nullptr;
    };

    bool registerMethod(const QString &fullMethodName, MethodHandler handler);
    bool registerQObjectMethods(QObject *service, const QString &serviceName);
    static bool isRpcMethod(const QMetaMethod &method);
    static CallResult invokeQObjectMethod(QObject *service, const QMetaMethod &method, const QByteArray &json);

    mutable QReadWriteLock m_lock;
    QHash<QString, ServiceRecord> m_services;
    QHash<QString, MethodHandler> m_methods;
    mutable QString m_lastError;
};
