#pragma once

#include "servicehub_global.h"

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QStringList>

struct SERVICEHUB_EXPORT CallResult
{
    int code = 0;
    QString message;
    QByteArray data;

    bool isSuccess() const { return code == 0; }
};

class SERVICEHUB_EXPORT IServiceHub
{
public:
    enum ErrorCode { Success = 0, MethodNotFound = 1, InvokeFailed = 2, ServiceDestroyed = 3, ServiceThreadNotRunning = 4 };

    virtual ~IServiceHub() = default;

    virtual bool registerService(QObject *service, const QString &serviceName = QString()) = 0;
    // ServiceHub 不负责销毁服务对象
    virtual bool unregisterService(const QString &serviceName) = 0;
    virtual bool hasService(const QString &serviceName) const = 0;
    virtual QStringList serviceNames() const = 0;
    virtual QStringList methodNames(const QString &serviceName = QString()) const = 0;
    virtual CallResult call(const QString &fullMethodName, const QByteArray &json = QByteArray()) const = 0;
    virtual QString lastError() const = 0;
};

extern "C" SERVICEHUB_EXPORT IServiceHub *createServiceHub();
extern "C" SERVICEHUB_EXPORT void destroyServiceHub(IServiceHub *hub);
