#pragma once

#include <QObject>

class GateWay : public QObject
{
    Q_OBJECT
public:
    explicit GateWay(QObject *parent = nullptr);
    ~GateWay() override;

    void send(int api, const QJsonValue &value);

private:
    void dispatch(int api, const QJsonValue &value);
};
