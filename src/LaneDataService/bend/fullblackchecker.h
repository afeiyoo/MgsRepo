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
    // 更新全量的可用状态 true:全量正常 false:全量异常
    void onUpdateFullBlackStatus(bool status, const QString &desc);
    void onUpdateFullBlackVersion(const QString &version);

public:
    // 全量状态
    bool m_fullBlackStatus = false;
    QString m_fullBlackDesc;
    // 全量版本
    QString m_fullBlackVersion = 0;

private:
    QThread *m_td = nullptr;
    FullBlackWorker *m_worker = nullptr;
    QTimer *m_timer = nullptr;
};
