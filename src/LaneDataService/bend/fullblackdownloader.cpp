#include "fullblackdownloader.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSaveFile>
#include <QSet>
#include <QStorageInfo>
#include <QXmlStreamReader>

#include "HttpClient/src/http.h"
#include "HttpClient/src/httpdownloadreply.h"
#include "minizqt.h"
#include "utils/datadealutils.h"

using namespace Utils;

FullBlackDownloader::FullBlackDownloader(QObject *parent)
    : QObject{parent}
    , m_httpClient(new Http())
{
    m_httpClient->setReadTimeout(DownloadReadTimeoutMs);
}

FullBlackDownloader::~FullBlackDownloader()
{
    m_running = false;

    // HttpReply/HttpDownloadReply internally keep a reference to Http. If a request is still
    // active, keep Http alive until that request has emitted finished.
    if (m_activeDownload) {
        Http *client = m_httpClient;
        m_httpClient = nullptr;
        disconnect(m_activeDownload, nullptr, this, nullptr);
        connect(m_activeDownload, &HttpDownloadReply::finished, m_activeDownload, [client](const HttpDownloadReply &) { delete client; });
        m_activeDownload->cancel();
    } else if (m_manifestReply) {
        Http *client = m_httpClient;
        m_httpClient = nullptr;
        disconnect(m_manifestReply, nullptr, this, nullptr);
        connect(m_manifestReply, &HttpReply::finished, m_manifestReply, [client](const HttpReply &) { delete client; });
    }

    delete m_httpClient;
}

bool FullBlackDownloader::isRunning() const
{
    return m_running;
}

FullBlackDownloader::Stage FullBlackDownloader::stage() const
{
    return m_stage;
}

FullBlackDownloader::Error FullBlackDownloader::error() const
{
    return m_error;
}

QString FullBlackDownloader::errorString() const
{
    return m_errorString;
}

QString FullBlackDownloader::batchNo() const
{
    return m_manifest.batchNo;
}

QString FullBlackDownloader::assembledFilePath() const
{
    return m_assembledFilePath;
}

QString FullBlackDownloader::extractedFilePath() const
{
    return m_extractedFilePath;
}

void FullBlackDownloader::start(const QString &stationServiceUrl, const QString &fullBlackPath)
{
    if (m_running)
        return;

    reset();
    m_stationServiceUrl = QUrl(stationServiceUrl.trimmed());
    m_fullBlackPath = QDir::cleanPath(fullBlackPath.trimmed());

    if (!m_stationServiceUrl.isValid() || m_stationServiceUrl.host().isEmpty()
        || (m_stationServiceUrl.scheme() != QStringLiteral("http") && m_stationServiceUrl.scheme() != QStringLiteral("https"))) {
        finishFailure(InvalidArgument, QStringLiteral("站级服务地址无效: %1").arg(stationServiceUrl));
        return;
    }
    if (m_fullBlackPath.isEmpty() || m_fullBlackPath == QStringLiteral(".")) {
        finishFailure(InvalidArgument, QStringLiteral("全量文件路径为空"));
        return;
    }

    m_running = true;
    fetchManifest();
}

void FullBlackDownloader::cancel()
{
    if (!m_running)
        return;

    m_cancelled = true;
    if (m_activeDownload) {
        m_activeDownload->cancel();
        return;
    }

    finishFailure(Cancelled, QStringLiteral("全量下载已取消"));
}

void FullBlackDownloader::reset()
{
    m_cancelled = false;
    m_error = NoError;
    m_errorString.clear();
    m_stationServiceUrl.clear();
    m_fullBlackPath.clear();
    m_workPath.clear();
    m_assembledFilePath.clear();
    m_extractedFilePath.clear();
    m_manifest = Manifest();
    m_partIndex = 0;
    m_partDownloadAttempt = 0;
    m_manifestReply.clear();
    m_activeDownload.clear();
    setStage(Idle);
}

void FullBlackDownloader::setStage(FullBlackDownloader::Stage stage)
{
    if (m_stage == stage)
        return;
    m_stage = stage;
    emit stageChanged(stage);
}

void FullBlackDownloader::fetchManifest()
{
    setStage(FetchingManifest);

    HttpReply *reply = m_httpClient->get(blackUpdateUrl(QStringLiteral("BlackUpdate.xml")));
    m_manifestReply = reply;
    connect(reply, &HttpReply::finished, this, [this](const HttpReply &reply) {
        m_manifestReply.clear();
        if (!m_running)
            return;
        if (m_cancelled) {
            finishFailure(Cancelled, QStringLiteral("全量下载已取消"));
            return;
        }
        if (!reply.isSuccessful()) {
            finishFailure(ManifestDownloadFailed,
                          QStringLiteral("下载BlackUpdate.xml失败: HTTP %1 %2").arg(reply.statusCode()).arg(reply.reasonPhrase()));
            return;
        }

        Manifest manifest;
        QString parseError;
        if (!parseManifest(reply.body(), &manifest, &parseError) || !validateManifest(manifest, &parseError)) {
            finishFailure(InvalidManifest, parseError);
            return;
        }

        m_manifest = manifest;
        prepareDownloadDirectory();
    });
}

bool FullBlackDownloader::parseManifest(const QByteArray &data, Manifest *manifest, QString *errorString) const
{
    if (!manifest || !errorString)
        return false;

    QXmlStreamReader reader(data);
    if (!reader.readNextStartElement() || reader.name() != QStringLiteral("blackupdate")) {
        *errorString = QStringLiteral("BlackUpdate.xml根节点不是blackupdate");
        return false;
    }

    while (reader.readNextStartElement()) {
        if (reader.name() == QStringLiteral("batchno")) {
            manifest->batchNo = reader.readElementText().trimmed();
        } else if (reader.name() == QStringLiteral("totalmd5")) {
            while (reader.readNextStartElement()) {
                if (reader.name() == QStringLiteral("paramtype"))
                    manifest->totalParamType = reader.readElementText().trimmed().toInt();
                else if (reader.name() == QStringLiteral("md5"))
                    manifest->totalMd5 = reader.readElementText().trimmed().toUpper();
                else
                    reader.skipCurrentElement();
            }
        } else if (reader.name() == QStringLiteral("parameter")) {
            PartInfo part;
            while (reader.readNextStartElement()) {
                if (reader.name() == QStringLiteral("paramtype"))
                    part.paramType = reader.readElementText().trimmed().toInt();
                else if (reader.name() == QStringLiteral("zipfile"))
                    part.fileName = reader.readElementText().trimmed();
                else if (reader.name() == QStringLiteral("md5"))
                    part.md5 = reader.readElementText().trimmed().toUpper();
                else if (reader.name() == QStringLiteral("operatelevel"))
                    part.operateLevel = reader.readElementText().trimmed().toInt();
                else
                    reader.skipCurrentElement();
            }
            manifest->parts.append(part);
        } else {
            reader.skipCurrentElement();
        }
    }

    if (reader.hasError()) {
        *errorString = QStringLiteral("解析BlackUpdate.xml失败: %1").arg(reader.errorString());
        return false;
    }
    return true;
}

bool FullBlackDownloader::validateManifest(const Manifest &manifest, QString *errorString) const
{
    const QRegularExpression batchRegex(QStringLiteral("^\\d{8}$"));
    const QRegularExpression md5Regex(QStringLiteral("^[0-9A-F]{32}$"));
    const QRegularExpression fileRegex(QStringLiteral("^ETCBlackCard_(\\d{8})\\.zip\\.(\\d{2})$"));

    if (!batchRegex.match(manifest.batchNo).hasMatch()) {
        *errorString = QStringLiteral("BlackUpdate.xml中的batchno无效: %1").arg(manifest.batchNo);
        return false;
    }
    if (manifest.totalParamType != 515 || !md5Regex.match(manifest.totalMd5).hasMatch()) {
        *errorString = QStringLiteral("BlackUpdate.xml中的totalmd5无效");
        return false;
    }
    if (manifest.parts.isEmpty()) {
        *errorString = QStringLiteral("BlackUpdate.xml中没有全量分包");
        return false;
    }

    QSet<QString> fileNames;
    for (int i = 0; i < manifest.parts.size(); ++i) {
        const PartInfo &part = manifest.parts.at(i);
        const QRegularExpressionMatch match = fileRegex.match(part.fileName);
        if (part.paramType != 515 || part.operateLevel != 4 || !match.hasMatch() || !md5Regex.match(part.md5).hasMatch()) {
            *errorString = QStringLiteral("第%1个全量分包信息无效").arg(i + 1);
            return false;
        }
        if (match.captured(1) != manifest.batchNo || match.captured(2).toInt() != i + 1) {
            *errorString = QStringLiteral("全量分包名称或顺序无效: %1").arg(part.fileName);
            return false;
        }
        if (fileNames.contains(part.fileName)) {
            *errorString = QStringLiteral("BlackUpdate.xml中存在重复分包: %1").arg(part.fileName);
            return false;
        }
        fileNames.insert(part.fileName);
    }
    return true;
}

QUrl FullBlackDownloader::blackUpdateUrl(const QString &fileName) const
{
    QUrl url = m_stationServiceUrl;
    url.setPath(QStringLiteral("/BlackUpdate/") + fileName);
    url.setQuery(QString());
    url.setFragment(QString());
    return url;
}

void FullBlackDownloader::prepareDownloadDirectory()
{
    m_workPath = QDir(m_fullBlackPath).filePath(QStringLiteral(".full_download/%1").arg(m_manifest.batchNo));
    if (!QDir().mkpath(m_workPath)) {
        finishFailure(CreateDirectoryFailed, QStringLiteral("创建全量下载目录失败: %1").arg(m_workPath));
        return;
    }

    m_partIndex = 0;
    m_partDownloadAttempt = 0;
    setStage(DownloadingParts);
    downloadNextPart();
}

void FullBlackDownloader::downloadNextPart()
{
    if (!m_running)
        return;
    if (m_cancelled) {
        finishFailure(Cancelled, QStringLiteral("全量下载已取消"));
        return;
    }
    if (m_partIndex >= m_manifest.parts.size()) {
        assembleParts();
        return;
    }

    const PartInfo &part = m_manifest.parts.at(m_partIndex);
    const QString filePath = QDir(m_workPath).filePath(part.fileName);
    if (QFileInfo::exists(filePath) && verifyMd5(filePath, part.md5)) {
        emit partReady(part.fileName, m_partIndex + 1, m_manifest.parts.size(), true);
        ++m_partIndex;
        m_partDownloadAttempt = 0;
        downloadNextPart();
        return;
    }

    downloadCurrentPart();
}

void FullBlackDownloader::downloadCurrentPart()
{
    const PartInfo part = m_manifest.parts.at(m_partIndex);
    const int index = m_partIndex;
    const QString filePath = QDir(m_workPath).filePath(part.fileName);
    ++m_partDownloadAttempt;

    HttpDownloadReply *reply = m_httpClient->download(blackUpdateUrl(part.fileName), filePath);
    m_activeDownload = reply;
    connect(reply, &HttpDownloadReply::progress, this, [this, part, index](qint64 received, qint64 total) {
        if (m_running)
            emit partProgress(part.fileName, index + 1, m_manifest.parts.size(), received, total);
    });
    connect(reply, &HttpDownloadReply::finished, this, [this, part, index, filePath](const HttpDownloadReply &reply) {
        m_activeDownload.clear();
        if (!m_running)
            return;
        if (m_cancelled) {
            finishFailure(Cancelled, QStringLiteral("全量下载已取消"));
            return;
        }
        if (index != m_partIndex)
            return;
        if (!reply.isSuccessful()) {
            handlePartFailure(PartDownloadFailed,
                              QStringLiteral("下载全量分包失败: %1, %2").arg(part.fileName, reply.errorString()));
            return;
        }
        if (!verifyMd5(filePath, part.md5)) {
            handlePartFailure(PartMd5Mismatch, QStringLiteral("全量分包MD5校验失败: %1").arg(part.fileName));
            return;
        }

        emit partReady(part.fileName, index + 1, m_manifest.parts.size(), false);
        ++m_partIndex;
        m_partDownloadAttempt = 0;
        downloadNextPart();
    });
}

void FullBlackDownloader::handlePartFailure(FullBlackDownloader::Error error, const QString &message)
{
    if (m_partDownloadAttempt < MaxPartDownloadAttempts) {
        downloadCurrentPart();
        return;
    }
    finishFailure(error, QStringLiteral("%1，已尝试%2次").arg(message).arg(MaxPartDownloadAttempts));
}

bool FullBlackDownloader::verifyMd5(const QString &filePath, const QString &expectedMd5) const
{
    bool ok = false;
    const QString actualMd5 = DataDealUtils::bigFileMd5(filePath, &ok);
    return ok && actualMd5.compare(expectedMd5, Qt::CaseInsensitive) == 0;
}

void FullBlackDownloader::assembleParts()
{
    setStage(Assembling);

    const QString assembledPath = QDir(m_workPath).filePath(QStringLiteral("ETCBlackCard_%1.zip").arg(m_manifest.batchNo));
    QSaveFile output(assembledPath);
    if (!output.open(QIODevice::WriteOnly)) {
        finishFailure(AssembleFailed, QStringLiteral("创建完整全量文件失败: %1").arg(output.errorString()));
        return;
    }

    QCryptographicHash totalHash(QCryptographicHash::Md5);
    for (const PartInfo &part : m_manifest.parts) {
        QFile input(QDir(m_workPath).filePath(part.fileName));
        if (!input.open(QIODevice::ReadOnly)) {
            output.cancelWriting();
            finishFailure(AssembleFailed, QStringLiteral("打开全量分包失败: %1").arg(part.fileName));
            return;
        }

        while (!input.atEnd()) {
            const QByteArray data = input.read(1024 * 1024);
            if (data.isEmpty() && input.error() != QFile::NoError) {
                output.cancelWriting();
                finishFailure(AssembleFailed, QStringLiteral("读取全量分包失败: %1").arg(part.fileName));
                return;
            }
            if (output.write(data) != data.size()) {
                output.cancelWriting();
                finishFailure(AssembleFailed, QStringLiteral("组装完整全量文件失败: %1").arg(output.errorString()));
                return;
            }
            totalHash.addData(data);
        }
    }

    const QString actualTotalMd5 = QString::fromLatin1(totalHash.result().toHex()).toUpper();
    if (actualTotalMd5 != m_manifest.totalMd5) {
        output.cancelWriting();
        finishFailure(TotalMd5Mismatch,
                      QStringLiteral("完整全量MD5校验失败，期望: %1，实际: %2").arg(m_manifest.totalMd5, actualTotalMd5));
        return;
    }
    if (!output.commit()) {
        finishFailure(AssembleFailed, QStringLiteral("保存完整全量文件失败: %1").arg(output.errorString()));
        return;
    }

    m_assembledFilePath = assembledPath;
    extractArchive(assembledPath);
}

void FullBlackDownloader::extractArchive(const QString &archivePath)
{
    setStage(Extracting);

    const QString expectedFileName = QStringLiteral("ETCBlackCard_%1.db").arg(m_manifest.batchNo);
    int databaseFileIndex = -1;
    quint64 databaseSize = 0;
    bool entriesOk = false;
    QString archiveError;
    const QList<MinizQt::ArchiveEntry> entries = MinizQt::archiveEntries(archivePath, &entriesOk, &archiveError);
    if (!entriesOk) {
        finishFailure(InvalidArchive, archiveError);
        return;
    }

    for (const MinizQt::ArchiveEntry &entry : entries) {
        if (entry.isDirectory)
            continue;
        if (!entry.isSupported || entry.isEncrypted) {
            finishFailure(InvalidArchive, QStringLiteral("ZIP中包含不支持或加密的文件"));
            return;
        }
        const QString archiveFileName = entry.fileName;
        if (QDir::isAbsolutePath(archiveFileName) || archiveFileName.startsWith('/') || archiveFileName.contains(QStringLiteral("../"))
            || QFileInfo(archiveFileName).fileName() != expectedFileName || databaseFileIndex >= 0) {
            finishFailure(InvalidArchive, QStringLiteral("ZIP内文件不符合预期: %1").arg(archiveFileName));
            return;
        }

        databaseFileIndex = entry.index;
        databaseSize = entry.uncompressedSize;
    }

    if (databaseFileIndex < 0 || databaseSize == 0) {
        finishFailure(InvalidArchive, QStringLiteral("ZIP中未找到有效的%1").arg(expectedFileName));
        return;
    }

    const QString extractPath = QDir(m_workPath).filePath(QStringLiteral("extract"));
    if (!QDir().mkpath(extractPath)) {
        finishFailure(ExtractFailed, QStringLiteral("创建全量解压目录失败: %1").arg(extractPath));
        return;
    }

    const QStorageInfo storage(extractPath);
    if (storage.isValid() && storage.bytesAvailable() >= 0 && databaseSize > static_cast<quint64>(storage.bytesAvailable())) {
        finishFailure(ExtractFailed, QStringLiteral("磁盘空间不足，无法解压全量数据库"));
        return;
    }

    const QString databasePath = QDir(extractPath).filePath(expectedFileName);
    if (!MinizQt::extractFile(archivePath, databaseFileIndex, databasePath, &archiveError)) {
        finishFailure(ExtractFailed, archiveError);
        return;
    }

    finishSuccess(databasePath);
}

void FullBlackDownloader::finishSuccess(const QString &extractedFilePath)
{
    if (!m_running)
        return;
    m_extractedFilePath = extractedFilePath;
    m_error = NoError;
    m_errorString.clear();
    m_running = false;
    setStage(Idle);
    emit finished(true);
}

void FullBlackDownloader::finishFailure(FullBlackDownloader::Error error, const QString &message)
{
    if (!m_running && error != InvalidArgument)
        return;
    m_error = error;
    m_errorString = message;
    m_running = false;
    setStage(Idle);
    emit finished(false);
}
