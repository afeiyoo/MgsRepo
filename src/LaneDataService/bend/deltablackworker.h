#pragma once

#include <QObject>
#include <QHash>
#include <QSqlDatabase>
#include <QTimer>

#include "env/defines.h"

class Http;

class DeltaBlackWorker : public QObject
{
    Q_OBJECT
public:
    explicit DeltaBlackWorker(QObject *parent = nullptr);
    ~DeltaBlackWorker() override;

public slots:
    void onInit();
    void onCheckDeltaBlack();
    void onCleanETCBlackCard(int fullBatchNo, QString tableName);

private:
    // 增量检查调度：每次只处理一个批次，让事件循环能够及时处理全量切表信号。
    void processNextDeltaBatch();
    void finishCurrentCheck();
    void scheduleNextCheck();

    // 站级请求与响应处理。
    bool requestNextDeltaData(QByteArray &responseData);
    // 返回true继续追赶下一批，返回false结束本轮检查。
    bool processDeltaResponse(const QByteArray &responseData);

    // 增量SQLite连接、校验及版本读取。
    bool ensureDatabaseConnected();
    bool validateDatabase(QString &error);
    QString fetchDeltaVersion();

    // 清表和增量落库。
    bool tryCleanETCBlackCard(const QString &tableName, int &affected, QString &error);
    bool applyDeltaBatch(int operateTable, const QVariantList &blackDetails, const QString &version);

    void setStatus(bool isValid, EM_DeltaBlackStatus status);

private:
    // 当前是否存在一致、可查询的增量数据视图
    bool m_isValid = false;
    // 当前处理阶段或最近一次失败原因
    EM_DeltaBlackStatus m_status = DeltaBlackDBUnavailable;
    // SQLite中最后一次成功提交的BlackVer；读取数据库是在恢复该值，事务成功才会推进该值。
    QString m_version;
    // 等待在下次写入前清理的历史增量表及对应全量批次；清理失败不阻塞另一张活动表
    QHash<QString, int> m_pendingCleanBatches;
    // 防止定时器或信号重复启动多条增量追赶链
    bool m_checkInProgress = false;

    QSqlDatabase m_dao;

    QTimer *m_timer = nullptr;
    Http *m_http = nullptr;
};
