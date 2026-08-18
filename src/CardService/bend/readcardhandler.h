#pragma once

#include "HttpServer/httprequesthandler.h"

class ReadCardHandler : public stefanfrings::HttpRequestHandler
{
    Q_OBJECT
public:
    explicit ReadCardHandler(QObject *parent = nullptr);
    ~ReadCardHandler() override;

    void service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response) override;
};
