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
    void dispatch(int api, const QJsonValue &values);

signals:
};
