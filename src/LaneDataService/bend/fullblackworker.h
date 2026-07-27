#pragma once

#include <QList>
#include <QObject>
#include <QSqlDatabase>
#include <QTimer>
#include <QUrl>

#include "env/defines.h"
#include "utils/optional.h"

class Http;
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
    void setStatus(bool isValid, EM_FullBlackStatus status);
    // 加载全量
    bool loadFullBlack(int batchNo, const QString &path);
    // 校验全量数据库
    bool validateFullBlack(const QSqlDatabase &db, int batchNo, QString *version, QString *cleanTable);

    // 解析并校验BlackUpdate.xml。manifestUrl非空时，同时生成切片下载地址
    bool parseFullManifest(const QByteArray &data, const QUrl &manifestUrl, ST_FullManifest *manifest, QString *error) const;
    // 读取本地BlackUpdate.xml
    bool readLocalManifest(ST_FullManifest *manifest, QString *error) const;
    // 获取远程BlackUpdate.xml
    bool fetchRemoteManifest(ST_FullManifest *manifest, QString *error) const;
    // 保存待下载清单并进入下载等待状态（下一阶段补充实际下载）
    void prepareFullDownload(const ST_FullManifest &manifest, EM_FullBlackStatus status = FullBlackDownloading);

    // 获取本地BlackUpdate.xml的批次号（后续由清单决策流程替代）
    int getLocalBatchNo();
    // 获取远程BlackUpdate.xml的批次号（后续由清单决策流程替代）
    int getRemoteBatchNo();

private:
    // 当前是否存在已加载、可查询的活动全量
    bool m_isValid = false;
    // 当前处理阶段或最近一次失败原因；不等同于全量可用性
    EM_FullBlackStatus m_curStatus = FullBlackUnavailable;
    // 全量版本
    QString m_version;
    // 当前进程实际加载的全量批次
    int m_activeBatchNo = 0;
    // 待清理全量表
    QString m_cleanTable;
    // 是否首次加载
    bool m_isFirst = true;
    // 是否已经进入全量更新流程
    bool m_updateRunning = false;
    // 当前等待下载的远程清单
    ST_FullManifest m_pendingManifest;
    // 全量数据库连接 [0]: 活动连接 [1]: 候选连接，非加载期间处于关闭状态
    QSqlDatabase m_dao[2];
    // 定时器
    QTimer *m_timer = nullptr;

    Http *m_http = nullptr;
};
