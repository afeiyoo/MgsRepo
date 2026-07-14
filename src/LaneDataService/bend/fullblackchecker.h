#pragma once

#include <QObject>
#include <QTimer>

class FullBlackWorker;
class QThread;

class FullBlackChecker : public QObject
{
    Q_OBJECT
public:
    explicit FullBlackChecker(QObject *parent = nullptr);
    ~FullBlackChecker() override;

    void init();

public slots:
    // 更新全量状态 0 全量状态正常 -1 全量状态异常
    void onUpdateFullBlackStatus(int status);

public:
    // 全量状态
    int m_fullBlackStatus = -1;

private:
    QThread *m_td = nullptr;
    FullBlackWorker *m_worker = nullptr;
    QTimer *m_timer = nullptr;
};
