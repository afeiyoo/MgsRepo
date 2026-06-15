#pragma once

#include <QObject>

class SignalCtrl : public QObject
{
    Q_OBJECT
public:
    explicit SignalCtrl(QObject *parent = nullptr);

signals:
};
