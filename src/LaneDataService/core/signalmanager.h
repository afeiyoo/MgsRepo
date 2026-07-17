#pragma once

#include "env/defines.h"
#include <QObject>

class SignalManager : public QObject
{
    Q_OBJECT
public:
    explicit SignalManager(QObject *parent = nullptr);

signals:
    void sigUpdateFullBlackStatus(const ST_FullBlackStatus &st);
};
