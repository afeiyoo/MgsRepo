#ifndef HTTP_H
#define HTTP_H

#include <QtNetwork>

#include "httpreply.h"
#include "httprequest.h"

class HttpDownloadReply;

class Http
{
public:
    static Http &instance();
    static const QMap<QByteArray, QByteArray> &getDefaultRequestHeaders();
    static void setDefaultReadTimeout(int timeout);

    Http();

    // 第三方库修改 2026-07-08 新增虚析构函数
    virtual ~Http() = default;

    void setRequestHeaders(const QMap<QByteArray, QByteArray> &headers);
    QMap<QByteArray, QByteArray> &getRequestHeaders();
    void addRequestHeader(const QByteArray &name, const QByteArray &value);

    void setReadTimeout(int timeout);
    int getReadTimeout() { return readTimeout; }

    int getMaxRetries() const;
    void setMaxRetries(int value);

    // 第三方库修改 2025-05-28 跳过证书校验
    void setSkipVerify(bool value);

    QNetworkReply *networkReply(const HttpRequest &req);
    virtual HttpReply *request(const HttpRequest &req);
    HttpReply *request(const QUrl &url,
                       QNetworkAccessManager::Operation operation = QNetworkAccessManager::GetOperation,
                       const QByteArray &body = QByteArray(),
                       uint offset = 0);
    HttpReply *get(const QUrl &url);
    HttpReply *head(const QUrl &url);
    HttpReply *post(const QUrl &url, const QMap<QString, QString> &params);
    HttpReply *post(const QUrl &url, const QByteArray &body, const QByteArray &contentType);
    HttpReply *put(const QUrl &url, const QByteArray &body, const QByteArray &contentType);
    HttpReply *deleteResource(const QUrl &url);

    // 第三方库修改 2026-07-21 支持大文件流式下载
    HttpDownloadReply *download(const QUrl &url, const QString &filePath);

    // 第三方库修改 2025-10-21 支持网络同步请求
    bool requestSync(QByteArray &resp,
                     const QUrl &url,
                     QNetworkAccessManager::Operation operation = QNetworkAccessManager::GetOperation,
                     const QByteArray &body = QByteArray(),
                     uint offset = 0);
    bool getSync(QByteArray &resp, const QUrl &url);
    bool headSync(QByteArray &resp, const QUrl &url);
    bool postSync(QByteArray &resp, const QUrl &url, const QMap<QString, QString> &params);
    bool postSync(QByteArray &resp, const QUrl &url, const QByteArray &body, const QByteArray &contentType);
    bool putSync(QByteArray &resp, const QUrl &url, const QByteArray &body, const QByteArray &contentType);
    bool deleteResourceSync(QByteArray &resp, const QUrl &url);

private:
    // 第三方库修改 2025-10-21 支持网络同步请求
    QPair<bool, QByteArray> blockUtilResponse(HttpReply *reply);

private:
    QMap<QByteArray, QByteArray> requestHeaders;
    int readTimeout;
    int maxRetries;

    // 第三方库修改 2025-05-28 跳过证书校验
    bool isSkipVerify = false;
};

#endif // HTTP_H
