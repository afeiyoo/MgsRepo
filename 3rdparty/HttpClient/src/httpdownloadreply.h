#ifndef HTTPDOWNLOADREPLY_H
#define HTTPDOWNLOADREPLY_H

#include <QtNetwork>

class Http;
class QSaveFile;

class HttpDownloadReply : public QObject
{
    Q_OBJECT

public:
    ~HttpDownloadReply() override;

    QUrl url() const;
    QString filePath() const;
    int statusCode() const;
    QString reasonPhrase() const;
    QString errorString() const;
    qint64 bytesReceived() const;
    qint64 bytesTotal() const;
    bool isSuccessful() const;

public slots:
    void cancel();

signals:
    void progress(qint64 bytesReceived, qint64 bytesTotal);
    void error(const QString &message);
    void finished(const HttpDownloadReply &reply);

private slots:
    void readAvailableData();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void replyFinished();
    void readTimeout();

private:
    friend class Http;
    HttpDownloadReply(const QUrl &url, const QString &filePath, Http &http);

    void start();
    void finish(bool success, const QString &message = QString());

private:
    Http &m_http;
    QUrl m_url;
    QString m_filePath;
    QNetworkReply *m_reply = nullptr;
    QSaveFile *m_file = nullptr;
    QTimer *m_readTimeoutTimer = nullptr;
    qint64 m_bytesReceived = 0;
    qint64 m_bytesTotal = -1;
    int m_statusCode = 0;
    QString m_reasonPhrase;
    QString m_errorString;
    bool m_finished = false;
    bool m_writeFailed = false;
};

#endif // HTTPDOWNLOADREPLY_H
