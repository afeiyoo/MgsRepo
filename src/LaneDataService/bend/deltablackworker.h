#pragma once

#include <QObject>
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
    void onFullBlackReadyForDelta();
    void onCheckDeltaBlack();
    void onCleanETCBlackCard(int fullBatchNo, QString tableName);
    void onFullBlackSwitchFinished(int fullBatchNo, bool activated);

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
    bool validateDatabase(QString &error);
    QString fetchDeltaVersion();

    // 清表和增量落库。
    bool tryCleanETCBlackCard(const QString &tableName, int &affected, QString &error);
    bool applyDeltaBatch(int operateTable, const QVariantList &blackDetails, const QString &version);

    void setStatus(bool isValid, EM_DeltaBlackStatus status);
    bool checkDatabase();

private:
    // 最近一次向站级检查时是否已确认追平最新增量版本
    bool m_isValid = false;
    // 当前处理阶段或最近一次失败原因
    EM_DeltaBlackStatus m_status = DeltaBlackDBUnavailable;
    // SQLite中最后一次成功提交的BlackVer；读取数据库是在恢复该值，事务成功才会推进该值。
    QString m_version;
    // 增量SQLite数据库文件路径，用于通知查询线程建立自己的只读连接。
    QString m_dbPath;
    // 首次活动全量就绪后才允许启动增量检查
    bool m_fullBlackReady = false;
    // 候选全量清表及切换期间暂停继续追赶增量
    bool m_fullSwitchInProgress = false;
    int m_switchFullBatchNo = 0;
    // 防止定时器或信号重复启动多条增量追赶链
    bool m_checkInProgress = false;
    // 数据库是否正常
    bool m_isDBNormal = false;

    QSqlDatabase m_dao;

    QTimer *m_timer = nullptr;
    Http *m_http = nullptr;
};
