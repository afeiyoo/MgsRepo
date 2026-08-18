#pragma once

#include "HttpServer/httprequesthandler.h"

class ReadPsamHandler : public stefanfrings::HttpRequestHandler
{
    Q_OBJECT
public:
    explicit ReadPsamHandler(QObject *parent = nullptr);
    ~ReadPsamHandler() override;

    void service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response) override;
};
