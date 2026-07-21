#include "httpdownloadreply.h"

#include <QDir>
#include <QFileInfo>
#include <QSaveFile>
#include <QTimer>

#include "http.h"
#include "httprequest.h"

HttpDownloadReply::HttpDownloadReply(const QUrl &url, const QString &filePath, Http &http)
    : m_http(http)
    , m_url(url)
    , m_filePath(filePath)
    , m_file(new QSaveFile(filePath, this))
    , m_readTimeoutTimer(new QTimer(this))
{
    m_readTimeoutTimer->setSingleShot(true);
    connect(m_readTimeoutTimer, &QTimer::timeout, this, &HttpDownloadReply::readTimeout);

    QTimer::singleShot(0, this, &HttpDownloadReply::start);
}

HttpDownloadReply::~HttpDownloadReply()
{
    if (m_reply && !m_reply->isFinished())
        m_reply->abort();
    if (!m_finished && m_file)
        m_file->cancelWriting();
}

QUrl HttpDownloadReply::url() const
{
    return m_url;
}

QString HttpDownloadReply::filePath() const
{
    return m_filePath;
}

int HttpDownloadReply::statusCode() const
{
    return m_statusCode;
}

QString HttpDownloadReply::reasonPhrase() const
{
    return m_reasonPhrase;
}

QString HttpDownloadReply::errorString() const
{
    return m_errorString;
}

qint64 HttpDownloadReply::bytesReceived() const
{
    return m_bytesReceived;
}

qint64 HttpDownloadReply::bytesTotal() const
{
    return m_bytesTotal;
}

bool HttpDownloadReply::isSuccessful() const
{
    return m_finished && m_errorString.isEmpty() && m_statusCode >= 200 && m_statusCode < 300;
}

void HttpDownloadReply::cancel()
{
    if (m_finished)
        return;

    m_errorString = QStringLiteral("Download cancelled");
    if (m_reply)
        m_reply->abort();
    else
        finish(false, m_errorString);
}

void HttpDownloadReply::start()
{
    if (m_finished)
        return;
    if (!m_url.isValid() || m_url.isEmpty()) {
        finish(false, QStringLiteral("Invalid download URL: %1").arg(m_url.toString()));
        return;
    }

    if (m_filePath.isEmpty()) {
        finish(false, QStringLiteral("Download file path is empty"));
        return;
    }

    const QFileInfo targetInfo(m_filePath);
    QDir targetDir = targetInfo.dir();
    if (!targetDir.exists() && !targetDir.mkpath(QStringLiteral("."))) {
        finish(false, QStringLiteral("Cannot create download directory: %1").arg(targetDir.absolutePath()));
        return;
    }
    if (!m_file->open(QIODevice::WriteOnly)) {
        finish(false, QStringLiteral("Cannot open download file: %1").arg(m_file->errorString()));
        return;
    }

    HttpRequest request;
    request.url = m_url;
    request.operation = QNetworkAccessManager::GetOperation;
    m_reply = m_http.networkReply(request);
    if (!m_reply) {
        finish(false, QStringLiteral("Cannot create network reply"));
        return;
    }

    connect(m_reply, &QIODevice::readyRead, this, &HttpDownloadReply::readAvailableData);
    connect(m_reply, &QNetworkReply::downloadProgress, this, &HttpDownloadReply::downloadProgress);
    connect(m_reply, &QNetworkReply::finished, this, &HttpDownloadReply::replyFinished);

    if (m_http.getReadTimeout() > 0)
        m_readTimeoutTimer->start(m_http.getReadTimeout());
}

void HttpDownloadReply::readAvailableData()
{
    if (m_finished || m_writeFailed || !m_reply)
        return;

    if (m_http.getReadTimeout() > 0)
        m_readTimeoutTimer->start(m_http.getReadTimeout());

    while (m_reply->bytesAvailable() > 0) {
        const QByteArray data = m_reply->read(64 * 1024);
        if (data.isEmpty())
            break;

        const qint64 written = m_file->write(data);
        if (written != data.size()) {
            m_writeFailed = true;
            m_errorString = QStringLiteral("Cannot write download file: %1").arg(m_file->errorString());
            m_reply->abort();
            return;
        }
        m_bytesReceived += written;
    }
}

void HttpDownloadReply::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    m_bytesTotal = bytesTotal;
    emit progress(bytesReceived, bytesTotal);
}

void HttpDownloadReply::replyFinished()
{
    if (m_finished || !m_reply)
        return;

    readAvailableData();
    m_statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    m_reasonPhrase = m_reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

    const bool httpOk = m_statusCode >= 200 && m_statusCode < 300;
    const bool networkOk = m_reply->error() == QNetworkReply::NoError;
    if (!m_writeFailed && networkOk && httpOk) {
        if (!m_file->commit()) {
            finish(false, QStringLiteral("Cannot save download file: %1").arg(m_file->errorString()));
            return;
        }
        finish(true);
        return;
    }

    QString message = m_errorString;
    if (message.isEmpty()) {
        message = QStringLiteral("Download failed: HTTP %1 %2")
                      .arg(m_statusCode)
                      .arg(m_reasonPhrase.isEmpty() ? m_reply->errorString() : m_reasonPhrase);
    }
    finish(false, message);
}

void HttpDownloadReply::readTimeout()
{
    if (m_finished)
        return;

    m_errorString = QStringLiteral("Download read timeout after %1ms").arg(m_http.getReadTimeout());
    if (m_reply)
        m_reply->abort();
    else
        finish(false, m_errorString);
}

void HttpDownloadReply::finish(bool success, const QString &message)
{
    if (m_finished)
        return;

    m_finished = true;
    m_readTimeoutTimer->stop();

    if (!success) {
        m_errorString = message;
        if (m_file && m_file->isOpen())
            m_file->cancelWriting();
        emit error(m_errorString);
    }

    emit finished(*this);

    if (m_reply)
        m_reply->deleteLater();
    deleteLater();
}
