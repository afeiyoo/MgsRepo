#include "networkhttpreply.h"

NetworkHttpReply::NetworkHttpReply(const HttpRequest &req, Http &http)
    : http(http), req(req), retryCount(0), finishedEmitted(false) {
    if (req.url.isEmpty()) {
        qWarning() << "Empty URL";
    }

    networkReply = http.networkReply(req);
    setParent(networkReply);
    setupReply();

    readTimeoutTimer = new QTimer(this);
    readTimeoutTimer->setInterval(http.getReadTimeout());
    readTimeoutTimer->setSingleShot(true);
    connect(readTimeoutTimer, &QTimer::timeout, this, &NetworkHttpReply::readTimeout,
            Qt::UniqueConnection);
    readTimeoutTimer->start();
}

void NetworkHttpReply::setupReply() {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(networkReply, &QNetworkReply::errorOccurred, this, &NetworkHttpReply::replyError,
            Qt::UniqueConnection);
#else
    connect(networkReply, SIGNAL(error(QNetworkReply::NetworkError)),
            SLOT(replyError(QNetworkReply::NetworkError)), Qt::UniqueConnection);
#endif
    connect(networkReply, &QNetworkReply::finished, this, &NetworkHttpReply::replyFinished,
            Qt::UniqueConnection);
    connect(networkReply, &QNetworkReply::downloadProgress, this,
            &NetworkHttpReply::downloadProgress, Qt::UniqueConnection);
}

QString NetworkHttpReply::errorMessage() {
    return url().toString() + QLatin1Char(' ') + QString::number(statusCode()) + QLatin1Char(' ') +
           reasonPhrase();
}

void NetworkHttpReply::emitError() {
    const QString msg = errorMessage();
#ifndef QT_NO_DEBUG_OUTPUT
    qDebug() << "Http:" << msg;
    if (!req.body.isEmpty()) qDebug() << "Http:" << req.body;
#endif
    emit error(msg);
    emitFinished();
}

void NetworkHttpReply::emitFinished() {
    if (finishedEmitted)
        return;
    finishedEmitted = true;

    readTimeoutTimer->stop();

    // disconnect to avoid replyFinished() from being called
    if (!networkReply) {
        emit finished(*this);
        return;
    }

    networkReply->disconnect();
    /** 2026-04-17 第三方库修改 断线重连问题 */
    emit finished(*this);

    // bye bye my reply
    // this will also delete this object and HttpReply as the QNetworkReply is their parent
    networkReply->deleteLater();
}

void NetworkHttpReply::replyFinished() {
#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    QUrl redirection = networkReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (redirection.isValid()) {
        HttpRequest redirectReq;
        if (redirection.isRelative()) redirection = networkReply->url().resolved(redirection);
        redirectReq.url = redirection;
        qDebug() << "Redirected to" << redirectReq.url;
        redirectReq.operation = req.operation;
        redirectReq.body = req.body;
        redirectReq.offset = req.offset;
        QNetworkReply *redirectReply = http.networkReply(redirectReq);
        setParent(redirectReply);
        networkReply->deleteLater();
        networkReply = redirectReply;
        setupReply();
        readTimeoutTimer->start();
        return;
    }
#endif

    if (isSuccessful()) {
        bytes = networkReply->readAll();
        emit data(bytes);

#ifndef QT_NO_DEBUG_OUTPUT
        if (!networkReply->attribute(QNetworkRequest::SourceIsFromCacheAttribute).toBool())
            qDebug() << statusCode() << networkReply->url().toString();
        else
            qDebug() << "CACHE" << networkReply->url().toString();
#endif
    }

    emitFinished();
}

void NetworkHttpReply::replyError(QNetworkReply::NetworkError code) {
    Q_UNUSED(code);
    const int status = statusCode();
    if (retryCount <= http.getMaxRetries() && status >= 500 && status < 600 &&
        (networkReply->operation() == QNetworkAccessManager::GetOperation ||
         networkReply->operation() == QNetworkAccessManager::HeadOperation)) {
        qDebug() << "Retrying" << status << QVariant(req.operation).toString() << req.url;
        networkReply->disconnect();
        networkReply->deleteLater();
        QNetworkReply *retryReply = http.networkReply(req);
        setParent(retryReply);
        networkReply = retryReply;
        setupReply();
        retryCount++;
        readTimeoutTimer->start();
    } else {
        emitError();
        return;
    }
}

void NetworkHttpReply::downloadProgress(qint64 bytesReceived, qint64 /* bytesTotal */) {
    // qDebug() << "Downloading" << bytesReceived << bytesTotal << networkReply->url();
    if (bytesReceived > 0 && readTimeoutTimer->isActive()) {
        readTimeoutTimer->stop();
        disconnect(networkReply, &QNetworkReply::downloadProgress, this,
                   &NetworkHttpReply::downloadProgress);
    }
}

void NetworkHttpReply::readTimeout() {
    qDebug() << "Timeout" << req.url;

    if (!networkReply) return;

    bool shouldRetry = (networkReply->operation() == QNetworkAccessManager::GetOperation ||
                        networkReply->operation() == QNetworkAccessManager::HeadOperation) &&
                       retryCount < http.getMaxRetries();

    if (!shouldRetry) {
        emitError();
        return;
    }

    networkReply->disconnect();
    networkReply->abort();
    networkReply->deleteLater();

    retryCount++;
    QNetworkReply *retryReply = http.networkReply(req);
    setParent(retryReply);
    networkReply = retryReply;
    setupReply();
    readTimeoutTimer->start();
}

QUrl NetworkHttpReply::url() const {
    return networkReply->url();
}

int NetworkHttpReply::statusCode() const {
    return networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}

QString NetworkHttpReply::reasonPhrase() const {
    /** 2026-06-22 第三方库修改 错误日志输出修改 */
    QString reason = networkReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    if (reason.isEmpty() && networkReply->error() != QNetworkReply::NoError)
        reason = networkReply->errorString();
    return reason;
}

const QList<QNetworkReply::RawHeaderPair> NetworkHttpReply::headers() const {
    return networkReply->rawHeaderPairs();
}

QByteArray NetworkHttpReply::header(const QByteArray &headerName) const {
    return networkReply->rawHeader(headerName);
}

QByteArray NetworkHttpReply::body() const {
    return bytes;
}
