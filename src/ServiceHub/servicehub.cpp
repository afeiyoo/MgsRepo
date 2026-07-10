#include "servicehub.h"

#include <QMetaType>
#include <QReadLocker>
#include <QThread>
#include <QWriteLocker>

IServiceHub *createServiceHub()
{
    return new ServiceHub();
}

void destroyServiceHub(IServiceHub *hub)
{
    delete hub;
}

ServiceHub::ServiceHub() = default;

ServiceHub::~ServiceHub()
{
    QWriteLocker locker(&m_lock);
    m_methods.clear();
    m_services.clear();
}

bool ServiceHub::registerService(QObject *service, const QString &serviceName)
{
    if (!service) {
        QWriteLocker locker(&m_lock);
        m_lastError = QStringLiteral("service is null");
        return false;
    }

    const QString name = serviceName.isEmpty() ? QString::fromLatin1(service->metaObject()->className()) : serviceName;

    QWriteLocker locker(&m_lock);
    if (m_services.contains(name)) {
        m_lastError = QStringLiteral("service already registered: %1").arg(name);
        return false;
    }

    ServiceRecord record;
    record.service = service;
    m_services[name] = record;
    m_lastError.clear();
    locker.unlock();

    return registerQObjectMethods(service, name);
}

bool ServiceHub::unregisterService(const QString &serviceName)
{
    QWriteLocker locker(&m_lock);

    auto serviceIt = m_services.find(serviceName);
    if (serviceIt == m_services.end()) {
        m_lastError = QStringLiteral("service not found: %1").arg(serviceName);
        return false;
    }

    m_services.erase(serviceIt);

    const QString prefix = serviceName + QLatin1Char('.');
    for (auto it = m_methods.begin(); it != m_methods.end();) {
        if (it.key().startsWith(prefix)) {
            it = m_methods.erase(it);
        } else {
            ++it;
        }
    }

    m_lastError.clear();
    return true;
}

bool ServiceHub::hasService(const QString &serviceName) const
{
    QReadLocker locker(&m_lock);
    return m_services.contains(serviceName);
}

QStringList ServiceHub::serviceNames() const
{
    QReadLocker locker(&m_lock);
    return m_services.keys();
}

QStringList ServiceHub::methodNames(const QString &serviceName) const
{
    QReadLocker locker(&m_lock);

    QStringList names;
    const QString prefix = serviceName.isEmpty() ? QString() : serviceName + QLatin1Char('.');
    for (auto it = m_methods.constBegin(); it != m_methods.constEnd(); ++it) {
        if (prefix.isEmpty() || it.key().startsWith(prefix)) {
            names << it.key();
        }
    }
    names.sort();
    return names;
}

bool ServiceHub::registerMethod(const QString &fullMethodName, MethodHandler handler)
{
    if (fullMethodName.isEmpty() || !handler) {
        QWriteLocker locker(&m_lock);
        m_lastError = QStringLiteral("invalid method registration");
        return false;
    }

    QWriteLocker locker(&m_lock);
    if (m_methods.contains(fullMethodName)) {
        m_lastError = QStringLiteral("method already registered: %1").arg(fullMethodName);
        return false;
    }

    m_methods[fullMethodName] = std::move(handler);
    m_lastError.clear();
    return true;
}

CallResult ServiceHub::call(const QString &fullMethodName, const QByteArray &json) const
{
    MethodHandler handler;
    {
        QReadLocker locker(&m_lock);
        handler = m_methods.value(fullMethodName);
    }

    if (!handler) {
        const QString message = QStringLiteral("method not found: %1").arg(fullMethodName);
        QWriteLocker locker(&m_lock);
        m_lastError = message;
        return {MethodNotFound, message, QByteArray()};
    }

    CallResult result = handler(json);
    {
        QWriteLocker locker(&m_lock);
        m_lastError = result.isSuccess() ? QString() : result.message;
    }
    return result;
}

QString ServiceHub::lastError() const
{
    QReadLocker locker(&m_lock);
    return m_lastError;
}

bool ServiceHub::registerQObjectMethods(QObject *service, const QString &serviceName)
{
    const QMetaObject *meta = service->metaObject();
    for (int i = meta->methodOffset(); i < meta->methodCount(); ++i) {
        const QMetaMethod method = meta->method(i);
        if (!isRpcMethod(method)) {
            continue;
        }

        const QString fullName = serviceName + QLatin1Char('.') + QString::fromLatin1(method.name());
        QPointer<QObject> guardedService(service);
        registerMethod(fullName, [guardedService, method](const QByteArray &json) {
            if (guardedService.isNull()) {
                return CallResult{ServiceDestroyed,
                                  QStringLiteral("service object has been destroyed: %1").arg(QString::fromLatin1(method.methodSignature())),
                                  QByteArray()};
            }
            return invokeQObjectMethod(guardedService.data(), method, json);
        });
    }
    return true;
}

bool ServiceHub::isRpcMethod(const QMetaMethod &method)
{
    if (method.methodType() != QMetaMethod::Method && method.methodType() != QMetaMethod::Slot) {
        return false;
    }

    if (method.parameterCount() != 1) {
        return false;
    }

    const int returnType = QMetaType::type(method.typeName());
    if (returnType != QMetaType::QByteArray) {
        return false;
    }

    const QList<QByteArray> parameterTypes = method.parameterTypes();
    const int parameterType = QMetaType::type(parameterTypes.at(0));
    return parameterType == QMetaType::QByteArray;
}

CallResult ServiceHub::invokeQObjectMethod(QObject *service, const QMetaMethod &method, const QByteArray &json)
{
    QThread *targetThread = service->thread();
    const bool sameThread = targetThread == QThread::currentThread();
    if (!sameThread && (!targetThread || !targetThread->isRunning())) {
        const QString message = QStringLiteral("service thread is not running: %1").arg(QString::fromLatin1(method.methodSignature()));
        return {ServiceThreadNotRunning, message, QByteArray()};
    }

    const Qt::ConnectionType connectionType = sameThread ? Qt::DirectConnection : Qt::BlockingQueuedConnection;

    QByteArray data;
    const bool invoked = method.invoke(service, connectionType, Q_RETURN_ARG(QByteArray, data), Q_ARG(QByteArray, json));
    if (!invoked) {
        const QString message = QStringLiteral("invoke failed: %1").arg(QString::fromLatin1(method.methodSignature()));
        return {InvokeFailed, message, QByteArray()};
    }

    return {Success, QString(), data};
}
