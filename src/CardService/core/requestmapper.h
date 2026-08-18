#pragma once

#include "HttpServer/httprequesthandler.h"

class RequestMapper : public stefanfrings::HttpRequestHandler
{
    Q_OBJECT
public:
    explicit RequestMapper(QObject *parent = nullptr);
    ~RequestMapper() override;

    void service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response) override;
};
