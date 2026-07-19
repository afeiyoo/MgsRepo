#pragma once

#include <QObject>
#include <QTimer>

class Http;
class GrowthBlackWorker : public QObject
{
    Q_OBJECT
public:
    explicit GrowthBlackWorker(QObject *parent = nullptr);
    ~GrowthBlackWorker() override;

public slots:
    void onInit();
    void onCheckGrowthBlack();

private:
    QByteArray getGrowthBlackJson();
    bool saveGrowthBlackJson(const QByteArray &data);

private:
    QTimer *m_timer = nullptr;
    Http *m_client = nullptr;
};
