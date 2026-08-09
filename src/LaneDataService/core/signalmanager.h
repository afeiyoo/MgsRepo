#pragma once

#include <QObject>

class SignalManager : public QObject
{
    Q_OBJECT
public:
    explicit SignalManager(QObject *parent = nullptr);

signals:
    void sigFullBlackReadyForDelta();
    void sigCleanETCBlackCard(int fullBatchNo, QString tableName);
    void sigCleanETCBlackCardFinished(int fullBatchNo, QString tableName, bool success, int affected, QString error);
    void sigFullBlackSwitchFinished(int fullBatchNo, bool activated);
};
