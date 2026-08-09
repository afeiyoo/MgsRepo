#pragma once

#include <QObject>
#include <QPointer>
#include <QSqlDatabase>
#include <QTimer>
#include <QUrl>

#include "env/defines.h"

class Http;
class HttpDownloadReply;
class FullBlackWorker : public QObject
{
    Q_OBJECT
public:
    explicit FullBlackWorker(QObject *parent = nullptr);
    ~FullBlackWorker() override;

public slots:
    void onCheckFullBlack();
    void onInit();
    void onCleanETCBlackCardFinished(int fullBatchNo, const QString &tableName, bool success, int affected, const QString &error);

private:
    // ---------- 清单检查与批次决策 ----------
    // 读取本地BlackUpdate.xml
    bool readLocalManifest(ST_FullManifest *manifest, QString *error) const;
    // 获取远程BlackUpdate.xml
    bool fetchRemoteManifest(ST_FullManifest *manifest, QString *error) const;
    // 解析并校验BlackUpdate.xml。manifestUrl非空时，同时生成切片下载地址
    bool parseFullManifest(const QByteArray &data, const QUrl &manifestUrl, ST_FullManifest *manifest, QString *error) const;
    // 远程权威清单已确认且本地批次一致时，加载本地全量；加载失败则按远程清单恢复
    void loadOrRecoverLocalFullBlack(const ST_FullManifest &localManifest, const ST_FullManifest &remoteManifest);
    // 当前活动批次已与远程一致时，只修复本地清单，不重新发布数据库
    void repairLocalManifestForActiveBatch(const ST_FullManifest &remoteManifest);
    // 首次获得可用活动全量后发送一次增量启动通知
    void notifyDeltaReadyOnce();
    // 当前检查周期结束后，重新计时下一次全量检查
    void scheduleNextCheck();

    // ---------- 更新准备与下载 ----------
    // 按远程权威清单启动全量更新：优先复用完整ZIP，缓存不可用时下载远程切片
    void startFullUpdate(const ST_FullManifest &manifest, bool cleanupRequired = true);
    // 清空一次更新流程产生的临时状态，不影响当前活动全量
    void resetUpdateContext();
    // 获取并校验全量临时目录路径，确保它严格位于fullBlackPath下
    bool resolveStagingPath(QString *path, QString *error) const;
    // 初始化并创建.staging目录
    bool initializeStagingDirectory(QString *path, QString *error);
    // 清理.staging中的切片、写入临时文件，仅保留普通完整ZIP文件
    bool cleanupStagingTempFiles(QString *error) const;
    // 按清单顺序下载下一个全量切片
    void downloadNextFullSlice();
    // 校验单个已下载切片的MD5
    bool validateFullSlice(const ST_FullSliceInfo &slice, const QString &filePath, QString *error) const;
    // 校验完整ZIP的totalmd5
    bool validateFullZip(const QString &zipPath, QString *error) const;
    // 按清单顺序拼接切片，并校验完整ZIP的MD5
    bool mergeAndValidateFullZip(QString *zipPath, QString *error) const;

    // ---------- 校验、发布与切换 ----------
    // 将已通过totalmd5校验的完整ZIP解压、校验并发布
    void publishValidatedFullZip();
    // 将全量数据库原子解压到正式目录
    bool extractFullDatabase(QString *dbPath, QString *error) const;
    // 原子提交指定BlackUpdate.xml到本地正式目录
    bool publishLocalManifest(const ST_FullManifest &manifest, QString *error) const;
    // 候选全量已校验完成，请求增量线程清表并暂停增量追赶
    void requestCandidateTableCleanup();
    // 清表成功后提交候选全量；此时不再允许继续使用旧全量
    void commitPreparedFull();
    // 清表失败时放弃候选全量并保留旧活动全量
    void abandonPreparedFull(const QString &error);
    // 结束失败的更新流程并清理下载切片，保留完整ZIP
    void failFullUpdate(const QString &error, EM_FullBlackStatus status);
    // 完成已激活的数据库更新流程
    void finishFullUpdate(EM_FullBlackStatus status = FullBlackReady, bool pruneFiles = true);

    // ---------- 数据库加载与校验 ----------
    // 加载全量并切换活动连接
    bool loadFullBlack(int batchNo, const QString &path);
    // 使用候选连接打开并校验全量，成功后保持候选连接打开
    bool openAndValidateFullBlack(int batchNo, const QString &path, QString *version, QString *cleanTable, QString *error);
    // 校验全量数据库业务元数据
    bool validateFullBlack(const QSqlDatabase &dao, int batchNo, QString *version, QString *cleanTable);

    // ---------- 状态与文件清理 ----------
    // 清理全量目录下除当前批次外的其他数据库文件
    void pruneOtherFullBlackFiles(int batchNo);
    // 清理.staging目录下除当前批次外的其他完整ZIP
    void pruneOtherFullZipFiles(int batchNo);
    // 更新当前全量状态
    void setStatus(bool isValid, EM_FullBlackStatus status);

private:
    // 当前是否存在已加载、可查询的活动全量
    bool m_isValid = false;
    // 全量版本
    QString m_version;
    // 当前进程实际加载的全量批次
    int m_activeBatchNo = 0;
    // 是否已经发送过增量启动通知
    bool m_deltaReadyNotified = false;
    // 是否已经进入全量更新流程
    bool m_updateRunning = false;
    // 当前等待处理的BlackUpdate.xml
    ST_FullManifest m_pendingManifest;
    // 初始化时创建并保存的.staging目录；运行期间不自动重建
    QString m_stagingPath;
    // 当前正在下载的切片下标
    int m_downloadSliceIndex = 0;
    // 拼接并校验通过的完整ZIP路径
    QString m_fullZipPath;
    // 已发布到正式目录、尚未切换连接的数据库路径
    QString m_publishedDbPath;
    // 候选全量中经校验的版本及待清理增量表
    QString m_candidateVersion;
    QString m_candidateCleanTable;
    // 本次候选全量是否必须先完成清表
    bool m_cleanupRequired = true;
    // 是否已发出清表请求、正在等待增量线程回应
    bool m_waitingForDeltaCleanup = false;
    // 全量数据库连接 [0]: 活动连接 [1]: 候选连接，非加载期间处于关闭状态
    QSqlDatabase m_dao[2];
    // 定时器
    QTimer *m_timer = nullptr;

    Http *m_http = nullptr;
    // 当前流式下载任务
    QPointer<HttpDownloadReply> m_downloadReply;
};
