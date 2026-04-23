#pragma once

#include <QObject>

#include "HttpServer/httprequesthandler.h"

class HttpHandler : public stefanfrings::HttpRequestHandler
{
    Q_OBJECT
public:
    explicit HttpHandler(QObject *parent = nullptr);

    ~HttpHandler() override;

    void service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response) override;
};
