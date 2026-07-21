#pragma once

#include "utils/optional.h"
#include <QObject>
#include <QSqlDatabase>
#include <QTimer>

class FullBlackWorker : public QObject
{
    Q_OBJECT
public:
    explicit FullBlackWorker(QObject *parent = nullptr);
    ~FullBlackWorker() override;

public slots:
    void onCheckFullBlack();
    void onInit();
    void onCleanETCBlackCardFinished(int affected);

private:
    // 标记首次全量检查完成并发送一次通知
    void finishFirstCheck();
    // 获取全量目录下全量文件的最大批次
    Utils::optional<int> getMaxBatchNoFromFiles(const QString &path) const;
    // 清理全量目录下批次小于batchNo的全量文件
    void pruneOldFiles(int batchNo);
    // 更新当前全量状态
    void setStatus(bool isValid, int status);
    // 加载全量
    bool loadFullBlack(int batchNo, const QString &path);
    // 校验全量数据库
    bool validateFullBlack(const QSqlDatabase &db, int batchNo, QString *version, QString *cleanTable);

private:
    // 当前全量是否正常 true:全量正常 false:全量异常
    bool m_isValid = false;
    // 0: 全量文件加载成功 => 全量正常
    // -1：程序启动，未找到全量文件 => 全量异常
    // -2: 检查全量时，未找到全量文件 => 取决上一次的全量状态
    // -3：程序启动，未找到当前批次全量文件 => 全量异常
    // -4: 检查全量时，未找到当前批次全量文件 => 取决上一次的全量状态
    // -5：程序启动，全量加载失败 => 全量异常
    // -6: 检查全量时，全量加载失败 => 取决上一次的全量状态
    int m_curStatus = -1;
    // 全量版本
    QString m_version;
    // 待清理全量表
    QString m_cleanTable;
    // 是否首次加载
    bool m_isFirst = true;
    // 全量数据库连接 [0]: 活动连接 [1]: 候选连接，非加载期间处于关闭状态
    QSqlDatabase m_dao[2];
    // 定时器
    QTimer *m_timer = nullptr;
};
