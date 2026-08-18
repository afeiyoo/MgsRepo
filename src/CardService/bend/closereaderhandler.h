#pragma once

#include "HttpServer/httprequesthandler.h"

class CloseReaderHandler : public stefanfrings::HttpRequestHandler
{
    Q_OBJECT
public:
    explicit CloseReaderHandler(QObject *parent = nullptr);
    ~CloseReaderHandler() override;

    void service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response) override;
};
