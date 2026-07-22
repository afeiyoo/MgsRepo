#pragma once

#include <QList>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QUrl>

class Http;
class HttpDownloadReply;
class HttpReply;

class FullBlackDownloader : public QObject
{
    Q_OBJECT
public:
    enum Stage
    {
        Idle,
        FetchingManifest,
        DownloadingParts,
        Assembling,
        Extracting
    };
    Q_ENUM(Stage)

    enum Error
    {
        NoError = 0,
        InvalidArgument = -1,
        ManifestDownloadFailed = -2,
        InvalidManifest = -3,
        CreateDirectoryFailed = -4,
        PartDownloadFailed = -5,
        PartMd5Mismatch = -6,
        AssembleFailed = -7,
        TotalMd5Mismatch = -8,
        Cancelled = -9,
        InvalidArchive = -10,
        ExtractFailed = -11
    };
    Q_ENUM(Error)

    explicit FullBlackDownloader(QObject *parent = nullptr);
    ~FullBlackDownloader() override;

    bool isRunning() const;
    Stage stage() const;
    Error error() const;
    QString errorString() const;
    QString batchNo() const;
    QString assembledFilePath() const;
    QString extractedFilePath() const;

public slots:
    void start(const QString &stationServiceUrl, const QString &fullBlackPath);
    void cancel();

signals:
    void stageChanged(FullBlackDownloader::Stage stage);
    void partProgress(const QString &fileName, int index, int count, qint64 bytesReceived, qint64 bytesTotal);
    void partReady(const QString &fileName, int index, int count, bool reused);
    void finished(bool success);

private:
    struct PartInfo
    {
        int paramType = -1;
        int operateLevel = -1;
        QString fileName;
        QString md5;
    };

    struct Manifest
    {
        QString batchNo;
        int totalParamType = -1;
        QString totalMd5;
        QList<PartInfo> parts;
    };

    void reset();
    void setStage(Stage stage);
    void fetchManifest();
    bool parseManifest(const QByteArray &data, Manifest *manifest, QString *errorString) const;
    bool validateManifest(const Manifest &manifest, QString *errorString) const;
    QUrl blackUpdateUrl(const QString &fileName) const;

    void prepareDownloadDirectory();
    void downloadNextPart();
    void downloadCurrentPart();
    void handlePartFailure(Error error, const QString &message);
    bool verifyMd5(const QString &filePath, const QString &expectedMd5) const;
    void assembleParts();
    void extractArchive(const QString &archivePath);

    void finishSuccess(const QString &extractedFilePath);
    void finishFailure(Error error, const QString &message);

private:
    static const int DownloadReadTimeoutMs = 5 * 60 * 1000;
    static const int MaxPartDownloadAttempts = 3;

    Http *m_httpClient = nullptr;
    QPointer<HttpReply> m_manifestReply;
    QPointer<HttpDownloadReply> m_activeDownload;

    bool m_running = false;
    bool m_cancelled = false;
    Stage m_stage = Idle;
    Error m_error = NoError;
    QString m_errorString;

    QUrl m_stationServiceUrl;
    QString m_fullBlackPath;
    QString m_workPath;
    QString m_assembledFilePath;
    QString m_extractedFilePath;
    Manifest m_manifest;
    int m_partIndex = 0;
    int m_partDownloadAttempt = 0;
};
