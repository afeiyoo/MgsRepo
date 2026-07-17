#pragma once

#include <QObject>
#include <QTimer>

class GrowthBlackWorker : public QObject
{
    Q_OBJECT
public:
    explicit GrowthBlackWorker(QObject *parent = nullptr);
    ~GrowthBlackWorker() override;

public slots:
    void onInit();
    void onCheckGrowthBlack();

signals:

private:
    QTimer *m_timer = nullptr;
};
