#pragma once

#include <QObject>
#include <QThread>

class DeltaBlackWorker;
class GrowthBlackMaster : public QObject
{
    Q_OBJECT
public:
    explicit GrowthBlackMaster(QObject *parent = nullptr);
    ~GrowthBlackMaster() override;

    void init();

private:
    QThread *m_td = nullptr;
    DeltaBlackWorker *m_worker = nullptr;
};
