#pragma once

#include "HttpServer/httprequesthandler.h"

class VerifyCardHandler : public stefanfrings::HttpRequestHandler
{
    Q_OBJECT
public:
    explicit VerifyCardHandler(QObject *parent = nullptr);
    ~VerifyCardHandler() override;

    void service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response) override;
};
