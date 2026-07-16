#pragma once

#include <QObject>
#include <QThread>
#include <QTimer>

#include "defs/defines.h"

class FullBlackWorker;
class FullBlackMaster : public QObject
{
    Q_OBJECT
public:
    explicit FullBlackMaster(QObject *parent = nullptr);
    ~FullBlackMaster() override;

    void init();

    ST_FullBlackStatus fullBlackStatus() const;

public slots:
    void onUpdateFullBlackStatus(const ST_FullBlackStatus &st);

private:
    QThread *m_td = nullptr;
    QTimer *m_timer = nullptr;
    FullBlackWorker *m_worker = nullptr;

    // 当前全量状态
    ST_FullBlackStatus m_fullBlackStatus;
};
