#pragma once

#include <QJsonValue>
#include <QObject>

class GateWay : public QObject
{
    Q_OBJECT
public:
    explicit GateWay(QObject *parent = nullptr);
    ~GateWay() override;

    void send(int api, const QJsonValue &values);

private:
    void apiLogin(const QJsonValue &values);
    void apiSystemQuitRequest();
    void apiSystemQuitQuery(const QJsonValue &values);
};
