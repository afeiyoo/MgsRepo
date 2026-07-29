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
    void onCleanETCBlackCardFinished(int affected);

private:
    // 获取并校验全量临时目录路径，确保它严格位于fullBlackPath下
    bool resolveStagingPath(QString *path, QString *error) const;

    // 清理.staging中的下载切片，保留可复用的完整ZIP
    bool cleanupFullSlices(QString *error) const;

    // 读取本地BlackUpdate.xml
    bool readLocalManifest(ST_FullManifest *manifest, QString *error) const;

    // 获取远程BlackUpdate.xml
    bool fetchRemoteManifest(ST_FullManifest *manifest, QString *error) const;

    // 解析并校验BlackUpdate.xml。manifestUrl非空时，同时生成切片下载地址
    bool parseFullManifest(const QByteArray &data, const QUrl &manifestUrl, ST_FullManifest *manifest, QString *error) const;

    // 保存待下载清单并启动切片下载
    void prepareFullDownload(const ST_FullManifest &manifest, EM_FullBlackStatus status = FullBlackDownloading);

    // 清理遗留切片并创建.staging目录
    bool prepareStagingDirectory(QString *path, QString *error);

    // 按清单顺序下载下一个全量切片
    void downloadNextFullSlice();

    // 当前检查周期结束后，重新计时下一次全量检查
    void scheduleNextCheck();

    // 标记首次全量检查完成并发送一次通知
    void finishFirstCheck();

    // 清理全量目录下除当前批次外的其他数据库文件
    void pruneOtherFullBlackFiles(int batchNo);

    // 清理.staging目录下除当前批次外的其他完整ZIP
    void pruneOtherFullZipFiles(int batchNo);

    // 更新当前全量状态
    void setStatus(bool isValid, EM_FullBlackStatus status);

    // 加载全量
    bool loadFullBlack(int batchNo, const QString &path);

    // 使用候选连接打开并校验全量，成功后保持候选连接打开
    bool openAndValidateFullBlack(int batchNo, const QString &path, QString *version, QString *cleanTable, QString *error);

    // 校验全量数据库
    bool validateFullBlack(const QSqlDatabase &dao, int batchNo, QString *version, QString *cleanTable);

    // 校验单个已下载切片的MD5
    bool validateFullSlice(const ST_FullSliceInfo &slice, const QString &filePath, QString *error) const;

    // 校验完整ZIP的totalmd5
    bool validateFullZip(const QString &zipPath, QString *error) const;

    // 按清单顺序拼接切片，并校验完整ZIP的MD5
    bool mergeAndValidateFullZip(QString *zipPath, QString *error) const;

    // 将已通过totalmd5校验的完整ZIP解压、校验并发布
    void publishValidatedFullZip();

    // 校验ZIP条目并将唯一的全量数据库原子解压到正式目录
    bool extractFullDatabase(QString *dbPath, QString *error) const;

    // 原子提交远程BlackUpdate.xml到本地正式目录
    bool publishLocalManifest(QString *error) const;

    // 从正式路径加载新数据库并切换活动连接
    bool activatePublishedDatabase(QString *error, EM_FullBlackStatus *failureStatus);

    // 结束失败的更新流程并清理下载切片，保留完整ZIP
    void failFullUpdate(const QString &error, EM_FullBlackStatus status);

    // 完成成功的更新流程，保留当前完整ZIP并清理其他临时文件
    void finishFullUpdate();

private:
    // 当前是否存在已加载、可查询的活动全量
    bool m_isValid = false;
    // 全量版本
    QString m_version;
    // 当前进程实际加载的全量批次
    int m_activeBatchNo = 0;
    // 待清理增量表
    QString m_cleanTable;
    // 是否首次检查全量
    bool m_isFirstCheck = true;
    // 是否已经进入全量更新流程
    bool m_updateRunning = false;
    // 当前等待下载的BlackUpdate.xml
    ST_FullManifest m_pendingManifest;
    // 当前全量更新使用的批次临时目录
    QString m_stagingPath;
    // 当前正在下载的切片下标
    int m_downloadSliceIndex = 0;
    // 当前流式下载任务
    QPointer<HttpDownloadReply> m_downloadReply;
    // 拼接并校验通过的完整ZIP路径
    QString m_fullZipPath;
    // 已发布到正式目录、尚未切换连接的数据库路径
    QString m_publishedDbPath;
    // 全量数据库连接 [0]: 活动连接 [1]: 候选连接，非加载期间处于关闭状态
    QSqlDatabase m_dao[2];
    // 定时器
    QTimer *m_timer = nullptr;

    Http *m_http = nullptr;
};
