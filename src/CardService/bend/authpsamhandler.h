#pragma once

#include "HttpServer/httprequesthandler.h"

class AuthPsamHandler : public stefanfrings::HttpRequestHandler
{
    Q_OBJECT
public:
    explicit AuthPsamHandler(QObject *parent = nullptr);
    ~AuthPsamHandler() override;

    void service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response) override;
};
