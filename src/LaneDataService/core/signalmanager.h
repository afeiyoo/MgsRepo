#pragma once

#include <QObject>

class SignalManager : public QObject
{
    Q_OBJECT
public:
    explicit SignalManager(QObject *parent = nullptr);

signals:
    void sigFullBlackFirstCheckFinished();
    void sigCleanETCBlackCard(QString tableName);
    void sigCleanETCBlackCardFinished(int affected);
};
