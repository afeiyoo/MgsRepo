#pragma once

#include <QObject>
#include <QReadWriteLock>

struct ST_EnvSnap;
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

private:
    mutable QReadWriteLock m_lock;
    bool m_isGrowthBlackVersion;  // 当前增量是否可用
    QString m_growthBlackVersion; // 增量版本

    bool m_isFullBlackValid = false; // 当前全量是否可用
    int m_fullBlackStatus = -1;      // 全量状态
    QString m_fullBlackVersion;      // 全量版本
};
