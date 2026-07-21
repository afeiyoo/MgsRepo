#pragma once

#include <QObject>
#include <QThread>

class DeltaBlackWorker;
class DeltaBlackMaster : public QObject
{
    Q_OBJECT
public:
    explicit DeltaBlackMaster(QObject *parent = nullptr);
    ~DeltaBlackMaster() override;

    void init();

private:
    QThread *m_td = nullptr;
    DeltaBlackWorker *m_worker = nullptr;
};
