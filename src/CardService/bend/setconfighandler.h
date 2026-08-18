#pragma once

#include "HttpServer/httprequesthandler.h"

class SetConfigHandler : public stefanfrings::HttpRequestHandler
{
    Q_OBJECT
public:
    explicit SetConfigHandler(QObject *parent = nullptr);
    ~SetConfigHandler() override;

    void service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response) override;
};
