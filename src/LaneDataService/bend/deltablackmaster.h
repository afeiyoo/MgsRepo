#pragma once

#include <QObject>
#include <QThread>

class DeltaBlackMaster : public QObject
{
    Q_OBJECT
public:
    explicit DeltaBlackMaster(QObject *parent = nullptr);
    ~DeltaBlackMaster() override;

    void init();

private:
    QThread *m_td = nullptr;
};
