#pragma once

#include <QObject>
#include <QReadWriteLock>

#include "env/defines.h"

class Environment : public QObject
{
    Q_OBJECT
public:
    explicit Environment(QObject *parent = nullptr);
    ~Environment() override;

    void init();

    // 获取业务变量快照
    ST_EnvSnap getEnvSnap() const;

    void updateFullBlackEnvs(bool isValid, int status, const QString &version);

    void updateDeltaBlackEnvs(bool isValid, int status, const QString &version);

private:
    mutable QReadWriteLock m_lock;
    bool m_isDeltaBlackValid = false;                 // 当前是否存在一致、可查询的增量数据视图
    int m_deltaBlackStatus = DeltaBlackDBUnavailable; // 当前处理阶段或最近一次失败原因
    QString m_deltaBlackVersion;                      // 最近一次成功提交的增量版本

    bool m_isFullBlackValid = false;              // 当前全量是否可用
    int m_fullBlackStatus = FullBlackCheckFailed; // 全量状态
    QString m_fullBlackVersion;                   // 全量版本
};
