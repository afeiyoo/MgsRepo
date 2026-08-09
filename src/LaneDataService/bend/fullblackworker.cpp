#include "fullblackworker.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSqlError>

#include "EasyQtSql.h"
#include "HttpClient/src/http.h"
#include "HttpClient/src/httpdownloadreply.h"
#include "Logger.h"
#include "config/config.h"
#include "core/globalmanager.h"
#include "core/signalmanager.h"
#include "env/environment.h"
#include "minizqt.h"
#include "utils/algorithm.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"
#include "utils/stdafx.h"

using namespace EasyQtSql;
using namespace Utils;

FullBlackWorker::FullBlackWorker(QObject *parent)
    : QObject{parent}
{
    m_timer = new QTimer(this);
    m_http = new Http();

    connect(GM_INS->m_sigMan, &SignalManager::sigCleanETCBlackCardFinished, this, &FullBlackWorker::onCleanETCBlackCardFinished);
}

FullBlackWorker::~FullBlackWorker()
{
    if (m_downloadReply) {
        disconnect(m_downloadReply, nullptr, this, nullptr);
        delete m_downloadReply.data();
    }

    QString stagingError;
    if (!cleanupStagingTempFiles(&stagingError))
        LOG_WARNING().noquote() << "析构时清理全量暂存文件失败:" << stagingError;

    QString connectionNames[2];
    for (int i = 0; i < 2; ++i) {
        connectionNames[i] = m_dao[i].connectionName();
        if (m_dao[i].isOpen())
            m_dao[i].close();
    }

    // 先释放所有QSqlDatabase句柄
    m_dao[0] = QSqlDatabase();
    m_dao[1] = QSqlDatabase();
    for (const QString &name : connectionNames)
        QSqlDatabase::removeDatabase(name);

    m_timer->stop();

    SAFE_DELETE(m_http);
}

void FullBlackWorker::onCheckFullBlack()
{
    LOG_INFO().noquote() << "开始检查全量...";

    if (m_updateRunning) {
        LOG_INFO().noquote() << "全量更新流程正在进行，本次检查跳过";
        return;
    }

    // 以本轮检查真正结束的时间作为下一次检查的计时起点。
    m_timer->stop();
    ST_FullManifest localManifest;
    ST_FullManifest remoteManifest;
    QString localError;
    QString remoteError;
    const bool localOk = readLocalManifest(&localManifest, &localError);
    const bool remoteOk = fetchRemoteManifest(&remoteManifest, &remoteError);

    if (localOk) {
        LOG_INFO().noquote() << "本地BlackUpdate.xml有效，batchno:" << localManifest.batchNo;
    } else {
        LOG_WARNING().noquote() << "本地BlackUpdate.xml无效:" << localError;
    }
    if (remoteOk) {
        LOG_INFO().noquote() << "远程BlackUpdate.xml有效，batchno:" << remoteManifest.batchNo;
    } else {
        LOG_WARNING().noquote() << "远程BlackUpdate.xml无效:" << remoteError;
    }

    // 远程清单是全量批次的唯一权威来源。远程不可用时，不允许根据本地清单加载或恢复全量。
    if (!remoteOk) {
        LOG_ERROR().noquote() << "无法获取远程权威批次，停止本次全量更新检查";
        setStatus(m_isValid, FullBlackCheckFailed);
        scheduleNextCheck();
        return;
    }

    // 从这里开始，远程清单有效，是本轮批次决策的权威来源。

    // 本地清单无效：活动全量已是权威批次时只修复清单，否则更新权威批次。
    if (!localOk) {
        LOG_WARNING().noquote() << "无法获取本地全量批次，根据远程权威批次进行检查";
        if (m_isValid && m_activeBatchNo == remoteManifest.batchNo) {
            repairLocalManifestForActiveBatch(remoteManifest);
        } else {
            startFullUpdate(remoteManifest);
        }
        return;
    }

    // 从这里开始，本地和远程清单都有效，可以安全比较批次。

    // 本地批次与远程权威批次不一致时，统一向远程批次收敛，包括必要的版本回退。
    if (localManifest.batchNo != remoteManifest.batchNo) {
        LOG_WARNING().noquote() << "本地全量批次与远程权威批次不一致，根据远程权威批次进行检查";

        // 当前活动全量已经是远程权威批次时，只需修复本地清单；否则按远程清单更新或回退。
        if (m_isValid && m_activeBatchNo == remoteManifest.batchNo) {
            repairLocalManifestForActiveBatch(remoteManifest);
        } else {
            startFullUpdate(remoteManifest);
        }
        return;
    }

    // 两端清单批次相等：加载本地全量；当前连接已加载该批次时直接复用。
    loadOrRecoverLocalFullBlack(localManifest, remoteManifest);
}

void FullBlackWorker::repairLocalManifestForActiveBatch(const ST_FullManifest &remoteManifest)
{
    QString error;
    if (!publishLocalManifest(remoteManifest, &error)) {
        LOG_ERROR().noquote() << error;
        setStatus(m_isValid, FullBlackPublishFailed);
        scheduleNextCheck();
        return;
    }

    pruneOtherFullBlackFiles(m_activeBatchNo);
    pruneOtherFullZipFiles(m_activeBatchNo);
    setStatus(true, FullBlackReady);
    scheduleNextCheck();
    LOG_INFO().noquote() << "当前活动全量与远程权威批次相同，仅修复本地BlackUpdate.xml，批次:" << m_activeBatchNo;
}

void FullBlackWorker::loadOrRecoverLocalFullBlack(const ST_FullManifest &localManifest, const ST_FullManifest &remoteManifest)
{
    const int batchNo = localManifest.batchNo;

    if (m_isValid && batchNo == m_activeBatchNo) {
        LOG_INFO().noquote() << "当前活动全量与本地BlackUpdate.xml中相同，无需重复加载，批次:" << batchNo;
        pruneOtherFullZipFiles(batchNo);
        setStatus(true, FullBlackReady);
        scheduleNextCheck();
        return;
    }

    const ST_ConfigSnap snap = GM_INS->m_conf->getConfigSnap();
    const QString dbPath = QDir(snap.fullBlackPath).filePath(QString("ETCBlackCard_%1.db").arg(batchNo));

    if (!loadFullBlack(batchNo, dbPath)) {
        LOG_ERROR().noquote() << "本地全量加载失败，批次:" << batchNo;
        // 本地BlackUpdate.xml是该批次已成功清表并发布的提交标记，恢复同批次数据库时不重复清表。
        startFullUpdate(remoteManifest, false);
        return;
    }

    LOG_INFO().noquote() << "本地全量加载成功，批次:" << batchNo;
    setStatus(true, FullBlackReady);
    pruneOtherFullBlackFiles(batchNo);
    pruneOtherFullZipFiles(batchNo);
    notifyDeltaReadyOnce();
    scheduleNextCheck();
}

bool FullBlackWorker::resolveStagingPath(QString *path, QString *error) const
{
    path->clear();
    error->clear();

    const QString configuredPath = GM_INS->m_conf->getConfigSnap().fullBlackPath.trimmed();
    if (configuredPath.isEmpty()) {
        *error = "fullBlackPath为空";
        return false;
    }

    FileName fullBlackPath = FileUtils::canonicalPath(FileName::fromString(configuredPath));
    QString stagingTemp = QDir(fullBlackPath.toString()).filePath(".staging");
    FileName stagingPath = FileUtils::canonicalPath(FileName::fromString(stagingTemp));
    if (!stagingPath.isChildOf(fullBlackPath)) {
        *error = QString("全量临时目录不在fullBlackPath内: %1").arg(stagingPath.toString());
        return false;
    }

    *path = stagingPath.toString();
    return true;
}

bool FullBlackWorker::cleanupStagingTempFiles(QString *error) const
{
    error->clear();

    QString stagingPath;
    if (!resolveStagingPath(&stagingPath, error))
        return false;

    QDir stagingDir(stagingPath);
    if (!stagingDir.exists())
        return true;

    const QRegularExpression fullZipRegex("^ETCBlackCard_\\d+\\.zip$");
    QStringList delErrors;

    const QStringList files = stagingDir.entryList(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot);
    for (const QString &fileName : files) {
        if (fullZipRegex.match(fileName).hasMatch())
            continue;

        QFile file(stagingDir.filePath(fileName));
        file.setPermissions(file.permissions() | QFileDevice::WriteUser);
        if (!file.remove()) {
            delErrors.append(fileName);
            continue;
        }
    }

    if (!delErrors.isEmpty()) {
        *error = QString("删除全量暂存文件失败: %1").arg(delErrors.join(", "));
        return false;
    }

    return true;
}

bool FullBlackWorker::readLocalManifest(ST_FullManifest *manifest, QString *error) const
{
    LOG_INFO().noquote() << "开始读取本地BlackUpdate.xml文件信息";
    const ST_ConfigSnap snap = GM_INS->m_conf->getConfigSnap();

    const QString localFile = QDir(snap.fullBlackPath).filePath("BlackUpdate.xml");
    if (!QFile::exists(localFile)) {
        *error = "本地BlackUpdate.xml不存在";
        return false;
    }

    FileReader reader;
    if (!reader.fetch(localFile)) {
        *error = QString("读取本地BlackUpdate.xml失败: %1").arg(reader.errorString());
        return false;
    }

    return parseFullManifest(reader.data(), QUrl(), manifest, error);
}

bool FullBlackWorker::fetchRemoteManifest(ST_FullManifest *manifest, QString *error) const
{
    LOG_INFO().noquote() << "开始读取远程BlackUpdate.xml文件信息";
    const ST_ConfigSnap snap = GM_INS->m_conf->getConfigSnap();

    QString baseUrl = snap.stationServiceURL.trimmed();
    while (baseUrl.endsWith('/'))
        baseUrl.chop(1);
    const QUrl manifestUrl(baseUrl + "/BlackUpdate/BlackUpdate.xml");
    if (!manifestUrl.isValid() || manifestUrl.isEmpty()) {
        *error = QString("远程BlackUpdate.xml获取地址无效: %1").arg(manifestUrl.toString());
        return false;
    }

    QByteArray data;
    if (!m_http->getSync(data, manifestUrl)) {
        *error = QString("远程BlackUpdate.xml获取失败: %1").arg(QString::fromUtf8(data));
        return false;
    }

    return parseFullManifest(data, manifestUrl, manifest, error);
}

bool FullBlackWorker::parseFullManifest(const QByteArray &data, const QUrl &manifestUrl, ST_FullManifest *manifest, QString *error) const
{
    *manifest = ST_FullManifest();
    error->clear();

    bool xmlOk = false;
    QString xmlError;
    const QVariantMap root = DataDealUtils::xmlToMap(data, &xmlOk, &xmlError);
    if (!xmlOk) {
        *error = QString("XML解析失败: %1").arg(xmlError);
        return false;
    }

    bool batchOk = false;
    const QString batchText = root.value("batchno").toString().trimmed();
    const int batchNo = batchText.toInt(&batchOk);
    if (!batchOk || batchNo <= 0) {
        *error = QString("batchno无效: %1").arg(batchText);
        return false;
    }

    // 获取完整zip文件的MD5
    const QVariantMap totalMd5Node = root.value("totalmd5").toMap();
    const QString totalMd5 = totalMd5Node.value("md5").toString().trimmed().toUpper();

    // 解析处理全量切片信息
    QList<ST_FullSliceInfo> slices;
    QVariantList parameterNodes;
    const QVariant parameterValue = root.value("parameter");
    if (parameterValue.type() == QVariant::List) {
        parameterNodes = parameterValue.toList();
    } else if (parameterValue.type() == QVariant::Map) {
        parameterNodes.append(parameterValue);
    }
    const QRegularExpression sliceRegex(QString("^ETCBlackCard_%1\\.zip\\.(\\d{2})$").arg(batchText));
    for (const QVariant &node : parameterNodes) {
        const QVariantMap one = node.toMap();
        const QString fileName = one.value("zipfile").toString().trimmed();
        const QString md5 = one.value("md5").toString().trimmed().toUpper();

        const QRegularExpressionMatch match = sliceRegex.match(fileName);
        if (!match.hasMatch()) {
            *error = QString("全量切片文件名无效: %1").arg(fileName);
            return false;
        }

        bool indexOk = false;
        const int index = match.captured(1).toInt(&indexOk);
        if (!indexOk || index <= 0) {
            *error = QString("全量切片序号无效: %1").arg(fileName);
            return false;
        }

        ST_FullSliceInfo slice;
        slice.index = index;
        slice.fileName = fileName;
        slice.md5 = md5;
        if (!manifestUrl.isEmpty())
            slice.url = manifestUrl.resolved(QUrl(fileName));
        slices.append(slice);
    }
    if (slices.isEmpty()) {
        *error = "全量切片文件为空";
        return false;
    }

    sort(slices, &ST_FullSliceInfo::index);
    for (int i = 0; i < slices.size(); ++i) {
        const int expectedIndex = i + 1;
        if (slices.at(i).index != expectedIndex) {
            *error = QString("全量切片序号不连续，期望%1，实际%2").arg(expectedIndex, 2, 10, QChar('0')).arg(slices.at(i).index, 2, 10, QChar('0'));
            return false;
        }
    }

    manifest->batchNo = batchNo;
    manifest->totalMd5 = totalMd5;
    manifest->slices = slices;
    manifest->rawXml = data;
    return true;
}

bool FullBlackWorker::initializeStagingDirectory(QString *path, QString *error)
{
    path->clear();
    error->clear();

    QString stagingRootPath;
    if (!resolveStagingPath(&stagingRootPath, error))
        return false;

    if (!QDir().mkpath(stagingRootPath)) {
        *error = QString("创建全量临时目录失败: %1").arg(stagingRootPath);
        return false;
    }

    *path = QDir::cleanPath(stagingRootPath);
    return true;
}

void FullBlackWorker::startFullUpdate(const ST_FullManifest &manifest, bool cleanupRequired)
{
    resetUpdateContext();

    if (m_stagingPath.isEmpty() || !QDir(m_stagingPath).exists()) {
        LOG_ERROR().noquote() << "全量临时目录不存在，无法启动更新:" << m_stagingPath;
        setStatus(m_isValid, FullBlackDownloadFailed);
        scheduleNextCheck();
        return;
    }

    m_pendingManifest = manifest;
    m_updateRunning = true;
    m_cleanupRequired = cleanupRequired;

    // 优先使用本地缓存
    const QString reusableZipPath = QDir(m_stagingPath).filePath(QString("ETCBlackCard_%1.zip").arg(manifest.batchNo));
    if (QFileInfo::exists(reusableZipPath)) {
        QString verifyError;
        if (validateFullZip(reusableZipPath, &verifyError)) {
            m_fullZipPath = reusableZipPath;
            LOG_INFO().noquote() << "本地完整全量ZIP校验通过，跳过下载，批次:" << manifest.batchNo << "路径:" << reusableZipPath;
            publishValidatedFullZip();
            return;
        }

        LOG_WARNING().noquote() << "本地完整全量ZIP不可复用，将重新下载，批次:" << manifest.batchNo << "原因:" << verifyError;

        if (!QFile::remove(reusableZipPath)) {
            failFullUpdate(QString("删除无效完整全量ZIP失败: %1").arg(reusableZipPath), FullBlackVerifyFailed);
            return;
        }
    }

    // 缓存找不到或不可复用时，按远程权威清单下载全量
    LOG_INFO().noquote() << "需要下载远程全量，批次:" << manifest.batchNo << "切片数量:" << manifest.slices.size() << "临时目录:" << m_stagingPath;
    downloadNextFullSlice();
}

void FullBlackWorker::resetUpdateContext()
{
    m_downloadReply.clear();
    if (m_dao[1].isOpen())
        m_dao[1].close();
    m_downloadSliceIndex = 0;
    m_pendingManifest = ST_FullManifest();
    m_updateRunning = false;
    m_fullZipPath.clear();
    m_publishedDbPath.clear();
    m_candidateVersion.clear();
    m_candidateCleanTable.clear();
    m_cleanupRequired = true;
    m_waitingForDeltaCleanup = false;
}

void FullBlackWorker::downloadNextFullSlice()
{
    if (!m_updateRunning)
        return;

    // 所有全量切片已下载，进行校验
    if (m_downloadSliceIndex >= m_pendingManifest.slices.size()) {
        QString zipPath;
        QString verifyError;
        if (!mergeAndValidateFullZip(&zipPath, &verifyError)) {
            failFullUpdate(verifyError, FullBlackVerifyFailed);
            return;
        }

        m_fullZipPath = zipPath;
        publishValidatedFullZip();
        return;
    }

    const int sliceIndex = m_downloadSliceIndex;
    const ST_FullSliceInfo slice = m_pendingManifest.slices.at(sliceIndex);
    const QString filePath = QDir(m_stagingPath).filePath(slice.fileName);

    LOG_INFO().noquote() << "开始下载全量切片:" << slice.url.toString() << "保存路径:" << filePath;
    HttpDownloadReply *reply = m_http->download(slice.url, filePath);
    m_downloadReply = reply;
    connect(reply, &HttpDownloadReply::finished, this, [this, sliceIndex, slice](const HttpDownloadReply &result) {
        if (!m_updateRunning || sliceIndex != m_downloadSliceIndex)
            return;

        m_downloadReply.clear();
        if (!result.isSuccessful()) {
            failFullUpdate(QString("切片%1下载失败: %2").arg(slice.fileName, result.errorString()), FullBlackDownloadFailed);
            return;
        }

        QString verifyError;
        if (!validateFullSlice(slice, result.filePath(), &verifyError)) {
            failFullUpdate(verifyError, FullBlackVerifyFailed);
            return;
        }

        LOG_INFO().noquote() << "全量切片下载及MD5校验完成:" << slice.fileName << "文件大小:" << result.bytesReceived();
        ++m_downloadSliceIndex;
        downloadNextFullSlice();
    });
}

bool FullBlackWorker::validateFullSlice(const ST_FullSliceInfo &slice, const QString &filePath, QString *error) const
{
    bool md5Ok = false;
    const QString actualMd5 = DataDealUtils::bigFileMd5(filePath, &md5Ok);
    if (!md5Ok) {
        *error = QString("读取切片%1计算MD5失败").arg(slice.fileName);
        return false;
    }

    if (actualMd5.compare(slice.md5, Qt::CaseInsensitive) != 0) {
        *error = QString("切片%1的MD5校验失败，期望:%2，实际:%3").arg(slice.fileName, slice.md5, actualMd5);
        return false;
    }

    return true;
}

bool FullBlackWorker::validateFullZip(const QString &zipPath, QString *error) const
{
    error->clear();

    bool md5Ok = false;
    const QString actualMd5 = DataDealUtils::bigFileMd5(zipPath, &md5Ok);
    if (!md5Ok) {
        *error = QString("读取完整全量ZIP计算MD5失败: %1").arg(zipPath);
        return false;
    }

    if (actualMd5.compare(m_pendingManifest.totalMd5, Qt::CaseInsensitive) != 0) {
        *error = QString("完整全量ZIP的MD5校验失败，期望:%1，实际:%2").arg(m_pendingManifest.totalMd5, actualMd5);
        return false;
    }

    return true;
}

bool FullBlackWorker::mergeAndValidateFullZip(QString *zipPath, QString *error) const
{
    zipPath->clear();
    error->clear();

    const QString targetPath = QDir(m_stagingPath).filePath(QString("ETCBlackCard_%1.zip").arg(m_pendingManifest.batchNo));
    FileSaver saver(targetPath);
    if (saver.hasError()) {
        *error = QString("创建完整全量ZIP失败: %1").arg(saver.errorString());
        return false;
    }

    for (const ST_FullSliceInfo &slice : m_pendingManifest.slices) {
        QFile input(QDir(m_stagingPath).filePath(slice.fileName));
        if (!input.open(QIODevice::ReadOnly)) {
            *error = QString("打开全量切片%1失败: %2").arg(slice.fileName, input.errorString());
            return false;
        }

        while (!input.atEnd()) {
            const QByteArray data = input.read(1024 * 1024);
            if (data.isEmpty()) {
                if (input.error() != QFile::NoError) {
                    *error = QString("读取全量切片%1失败: %2").arg(slice.fileName, input.errorString());
                    return false;
                }
                break;
            }
            if (!saver.write(data)) {
                *error = QString("写入完整全量ZIP失败: %1").arg(saver.errorString());
                return false;
            }
        }
    }

    if (!saver.finalize()) {
        *error = QString("保存完整全量ZIP失败: %1").arg(saver.errorString());
        return false;
    }

    if (!validateFullZip(targetPath, error)) {
        if (QFile::exists(targetPath) && !QFile::remove(targetPath))
            error->append(QString("；删除无效完整全量ZIP失败: %1").arg(targetPath));
        return false;
    }

    *zipPath = targetPath;
    return true;
}

void FullBlackWorker::publishValidatedFullZip()
{
    if (!m_updateRunning || m_fullZipPath.isEmpty())
        return;

    QString dbPath;
    QString extractError;
    if (!extractFullDatabase(&dbPath, &extractError)) {
        failFullUpdate(extractError, FullBlackVerifyFailed);
        return;
    }

    m_publishedDbPath = dbPath;
    QString prepareError;
    if (!openAndValidateFullBlack(m_pendingManifest.batchNo, m_publishedDbPath, &m_candidateVersion, &m_candidateCleanTable, &prepareError)) {
        if (QFile::exists(m_publishedDbPath) && !QFile::remove(m_publishedDbPath))
            LOG_WARNING().noquote() << "删除本次发布的正式全量数据库失败:" << m_publishedDbPath;
        failFullUpdate(prepareError, FullBlackVerifyFailed);
        return;
    }

    if (m_cleanupRequired) {
        requestCandidateTableCleanup();
    } else {
        commitPreparedFull();
    }
}

bool FullBlackWorker::extractFullDatabase(QString *dbPath, QString *error) const
{
    dbPath->clear();
    error->clear();

    bool entriesOk = false;
    const QList<MinizQt::ArchiveEntry> entries = MinizQt::archiveEntries(m_fullZipPath, &entriesOk, error);
    if (!entriesOk)
        return false;

    QList<MinizQt::ArchiveEntry> files;
    for (const MinizQt::ArchiveEntry &entry : entries) {
        if (!entry.isDirectory)
            files.append(entry);
    }
    if (files.size() != 1) {
        *error = QString("完整全量ZIP中的文件数量不是1，实际:%1").arg(files.size());
        return false;
    }

    const QString expectedFileName = QString("ETCBlackCard_%1.db").arg(m_pendingManifest.batchNo);
    const MinizQt::ArchiveEntry entry = files.first();
    if (entry.fileName != expectedFileName) {
        *error = QString("完整全量ZIP中的数据库文件名无效，期望:%1，实际:%2").arg(expectedFileName, entry.fileName);
        return false;
    }

    const QString targetPath = QDir(GM_INS->m_conf->getConfigSnap().fullBlackPath).filePath(expectedFileName);
    if (!MinizQt::extractFile(m_fullZipPath, entry.index, targetPath, error))
        return false;

    *dbPath = targetPath;
    return true;
}

void FullBlackWorker::requestCandidateTableCleanup()
{
    if (!m_updateRunning || m_waitingForDeltaCleanup)
        return;

    m_waitingForDeltaCleanup = true;
    LOG_INFO().noquote() << "候选全量校验完成，请求清理增量表，批次:" << m_pendingManifest.batchNo << "表:" << m_candidateCleanTable;
    emit GM_INS->m_sigMan->sigCleanETCBlackCard(m_pendingManifest.batchNo, m_candidateCleanTable);
}

void FullBlackWorker::commitPreparedFull()
{
    if (!m_updateRunning || !m_dao[1].isOpen())
        return;

    const int batchNo = m_pendingManifest.batchNo;
    QString manifestError;
    const bool manifestPublished = publishLocalManifest(m_pendingManifest, &manifestError);

    // 清表一旦成功，旧全量已不再是完整视图。即使本地清单提交失败，也必须激活候选全量。
    std::swap(m_dao[0], m_dao[1]);
    if (m_dao[1].isOpen())
        m_dao[1].close();

    m_version = m_candidateVersion;
    m_activeBatchNo = batchNo;
    if (!manifestPublished) {
        LOG_ERROR().noquote() << "增量表已清理且候选全量已激活，但本地BlackUpdate.xml提交失败:" << manifestError;
        finishFullUpdate(FullBlackPublishFailed, false);
        return;
    }

    LOG_INFO().noquote() << "新全量数据库及本地清单提交完成，批次:" << batchNo << "正式路径:" << m_publishedDbPath;
    finishFullUpdate();
}

void FullBlackWorker::abandonPreparedFull(const QString &error)
{
    const int batchNo = m_pendingManifest.batchNo;
    const QString candidatePath = m_publishedDbPath;

    if (m_dao[1].isOpen())
        m_dao[1].close();
    if (QFile::exists(candidatePath) && !QFile::remove(candidatePath))
        LOG_WARNING().noquote() << "放弃候选全量时删除数据库失败:" << candidatePath;

    failFullUpdate(error, FullBlackCleanupFailed);
    emit GM_INS->m_sigMan->sigFullBlackSwitchFinished(batchNo, false);
}

bool FullBlackWorker::publishLocalManifest(const ST_FullManifest &manifest, QString *error) const
{
    error->clear();
    if (manifest.rawXml.isEmpty()) {
        *error = "待发布BlackUpdate.xml原始内容为空";
        return false;
    }

    FileSaver saver(QDir(GM_INS->m_conf->getConfigSnap().fullBlackPath).filePath("BlackUpdate.xml"));
    if (saver.hasError()) {
        *error = QString("创建本地BlackUpdate.xml失败: %1").arg(saver.errorString());
        return false;
    }
    if (!saver.write(manifest.rawXml)) {
        *error = QString("写入本地BlackUpdate.xml失败: %1").arg(saver.errorString());
        return false;
    }
    if (!saver.finalize()) {
        *error = QString("提交本地BlackUpdate.xml失败: %1").arg(saver.errorString());
        return false;
    }

    return true;
}

void FullBlackWorker::failFullUpdate(const QString &error, EM_FullBlackStatus status)
{
    LOG_ERROR().noquote() << error;

    QString cleanupError;
    if (!cleanupStagingTempFiles(&cleanupError))
        LOG_WARNING().noquote() << "全量更新失败后清理暂存文件失败:" << cleanupError;

    resetUpdateContext();
    setStatus(m_isValid, status);
    scheduleNextCheck();
}

void FullBlackWorker::finishFullUpdate(EM_FullBlackStatus status, bool pruneFiles)
{
    const int activeBatchNo = m_activeBatchNo;
    QString cleanupError;
    if (!cleanupStagingTempFiles(&cleanupError))
        LOG_WARNING().noquote() << "全量更新成功后清理暂存文件失败:" << cleanupError;

    if (pruneFiles) {
        pruneOtherFullBlackFiles(activeBatchNo);
        pruneOtherFullZipFiles(activeBatchNo);
    }

    resetUpdateContext();
    setStatus(true, status);
    notifyDeltaReadyOnce();
    scheduleNextCheck();
    LOG_INFO().noquote() << "全量更新流程完成，当前活动批次:" << activeBatchNo;
}

void FullBlackWorker::notifyDeltaReadyOnce()
{
    // 无可用活动全量时，增量检查不启动
    if (m_deltaReadyNotified || !m_isValid)
        return;

    m_deltaReadyNotified = true;
    emit GM_INS->m_sigMan->sigFullBlackReadyForDelta();
}

void FullBlackWorker::scheduleNextCheck()
{
    m_timer->start();
}

void FullBlackWorker::onInit()
{
    // 全量暂存目录只在初始化时创建；运行期间若被删除，后续更新直接报错，不自动重建。
    QString stagingError;
    if (!initializeStagingDirectory(&m_stagingPath, &stagingError)) {
        LOG_ERROR().noquote() << "初始化全量暂存目录失败:" << stagingError;
    } else if (!cleanupStagingTempFiles(&stagingError)) {
        LOG_WARNING().noquote() << "全量检查启动时，清理全量暂存文件失败:" << stagingError;
    }

    // 全量数据库连接初始化
    for (int i = 0; i < 2; ++i) {
        const QString connName = QString("fullBlack_%1").arg(i, 2, 10, QChar('0'));
        m_dao[i] = QSqlDatabase::addDatabase("QSQLITE", connName);
    }

    // 每隔10分钟检查一次全量
    m_timer->setInterval(10 * 60 * 1000);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &FullBlackWorker::onCheckFullBlack);

    // 程序加载时，立即进行全量检查
    onCheckFullBlack();
}

void FullBlackWorker::onCleanETCBlackCardFinished(int fullBatchNo, const QString &tableName, bool success, int affected, const QString &error)
{
    if (!m_updateRunning || !m_waitingForDeltaCleanup || fullBatchNo != m_pendingManifest.batchNo
        || tableName.compare(m_candidateCleanTable, Qt::CaseInsensitive) != 0) {
        LOG_WARNING().noquote() << "忽略与当前候选全量不匹配的清表结果，批次:" << fullBatchNo << "表:" << tableName;
        return;
    }

    m_waitingForDeltaCleanup = false;
    if (!success) {
        const QString cleanupError
            = QString("增量表清理失败，批次:%1，表:%2，错误:%3，影响行数:%4").arg(fullBatchNo).arg(tableName).arg(error).arg(affected);
        abandonPreparedFull(cleanupError);
        return;
    }

    LOG_INFO().noquote() << "增量表清理完成，批次:" << fullBatchNo << "表:" << tableName << "影响行数:" << affected;

    commitPreparedFull();
    emit GM_INS->m_sigMan->sigFullBlackSwitchFinished(fullBatchNo, true);
}

void FullBlackWorker::pruneOtherFullBlackFiles(int batchNo)
{
    const FileName dirPath = FileName::fromString(GM_INS->m_conf->getConfigSnap().fullBlackPath);
    if (!dirPath.exists())
        return;

    const FileNameList fileList = FileUtils::getFilesWithSuffix(dirPath, ".db");
    if (fileList.isEmpty())
        return;

    QStringList delErrors;
    const QString activeFileName = QString("ETCBlackCard_%1.db").arg(batchNo);
    for (const auto &file : fileList) {
        const QString fileName = file.fileName();
        if (fileName == activeFileName)
            continue;

        if (!QFile::remove(file.toString())) {
            delErrors.append(fileName);
            continue;
        }
    }

    if (!delErrors.isEmpty()) {
        LOG_WARNING().noquote() << "删除废弃全量数据库文件失败:" << delErrors.join(", ");
        return;
    }
}

void FullBlackWorker::pruneOtherFullZipFiles(int batchNo)
{
    QString stagingPath;
    QString error;
    if (!resolveStagingPath(&stagingPath, &error)) {
        LOG_WARNING().noquote() << "解析全量ZIP目录失败:" << error;
        return;
    }

    QDir stagingDir(stagingPath);
    if (!stagingDir.exists())
        return;

    QStringList delErrors;
    const QString activeFileName = QString("ETCBlackCard_%1.zip").arg(batchNo);
    const QRegularExpression zipRegex("^ETCBlackCard_\\d+\\.zip$");
    const QStringList files = stagingDir.entryList(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot);
    for (const QString &fileName : files) {
        if (fileName == activeFileName || !zipRegex.match(fileName).hasMatch())
            continue;

        if (!QFile::remove(stagingDir.filePath(fileName))) {
            delErrors.append(fileName);
            continue;
        }
    }

    if (!delErrors.isEmpty()) {
        LOG_WARNING().noquote() << "删除其他批次完整全量ZIP失败:" << delErrors.join(", ");
        return;
    }
}

void FullBlackWorker::setStatus(bool isValid, EM_FullBlackStatus status)
{
    m_isValid = isValid;
    GM_INS->m_env->updateFullBlackEnvs(m_isValid, status, m_version);
}

bool FullBlackWorker::loadFullBlack(int batchNo, const QString &path)
{
    QString candidateVersion;
    QString candidateCleanTable;
    QString error;
    if (!openAndValidateFullBlack(batchNo, path, &candidateVersion, &candidateCleanTable, &error)) {
        LOG_ERROR().noquote() << error;
        return false;
    }

    // 交换活动连接
    std::swap(m_dao[0], m_dao[1]);
    if (m_dao[1].isOpen())
        m_dao[1].close();

    m_version = candidateVersion;
    m_activeBatchNo = batchNo;
    return true;
}

bool FullBlackWorker::openAndValidateFullBlack(int batchNo, const QString &path, QString *version, QString *cleanTable, QString *error)
{
    version->clear();
    cleanTable->clear();
    error->clear();

    LOG_INFO().noquote() << "加载并校核全量文件:" << path << "批次:" << batchNo;
    if (m_dao[1].isOpen())
        m_dao[1].close();
    m_dao[1].setDatabaseName(path);
    if (!m_dao[1].open()) {
        *error = QString("全量文件打开失败: %1").arg(m_dao[1].lastError().text());
        m_dao[1].close();
        return false;
    }

    if (!validateFullBlack(m_dao[1], batchNo, version, cleanTable)) {
        *error = QString("全量文件业务校验失败，批次:%1").arg(batchNo);
        m_dao[1].close();
        version->clear();
        cleanTable->clear();
        return false;
    }

    LOG_INFO().noquote() << "全量文件校核成功，批次:" << batchNo;
    return true;
}

bool FullBlackWorker::validateFullBlack(const QSqlDatabase &dao, int batchNo, QString *version, QString *cleanTable)
{
    const QString sql = "SELECT version, cleantable FROM t_operatectrl WHERE paramtype = ? AND batchno = ?";

    try {
        Database db(dao);
        PreparedQuery query = db.prepare(sql);
        QueryResult res = query.exec(515, batchNo);

        LOG_INFO().noquote() << "执行SQL语句:" << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next()) {
            LOG_ERROR().noquote() << "未查询到全量元数据, 批次" << batchNo;
            return false;
        }

        const QString candidateVersion = res.value("version").toString();
        const QString candidateCleanTable = res.value("cleantable").toString();
        if (candidateVersion.isEmpty() || candidateCleanTable.isEmpty()) {
            LOG_ERROR().noquote() << "全量version或cleantable为空";
            return false;
        }

        *version = candidateVersion;
        *cleanTable = candidateCleanTable;
        return true;
    } catch (const DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery;
        return false;
    }
}
