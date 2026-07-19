#pragma once

#include <QObject>
#include <QThread>

class FullBlackWorker;
class FullBlackMaster : public QObject
{
    Q_OBJECT
public:
    explicit FullBlackMaster(QObject *parent = nullptr);
    ~FullBlackMaster() override;

    void init();

private:
    QThread *m_td = nullptr;
    FullBlackWorker *m_worker = nullptr;
};
