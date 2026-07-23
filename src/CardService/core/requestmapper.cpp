#include "requestmapper.h"

#include "Logger.h"
#include "bend/authpsamhandler.h"
#include "bend/closereaderhandler.h"
#include "bend/readcardhandler.h"
#include "bend/readpsamhandler.h"
#include "bend/setconfighandler.h"
#include "bend/verifycardhandler.h"

RequestMapper::RequestMapper(QObject *parent)
    : HttpRequestHandler{parent}
{}

RequestMapper::~RequestMapper() {}

void RequestMapper::service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response)
{
    // 设置跨域响应头
    response.setHeader("Access-Control-Allow-Origin", "*");
    response.setHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    response.setHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    if (request.getMethod() == "OPTIONS") {
        response.setStatus(200, "OK");
        response.write("", true);
        return;
    }

    QByteArray path = request.getPath();
    LOG_INFO().noquote() << "RequestMapper: path=" << path.data();

    if (path == "/api/verifyIdCard") {
        VerifyCardHandler handler;
        handler.service(request, response);
    } else if (path == "/api/authPsam") {
        AuthPsamHandler handler;
        handler.service(request, response);
    } else if (path == "/api/readPsam") {
        ReadPsamHandler handler;
        handler.service(request, response);
    } else if (path == "/api/readCard") {
        ReadCardHandler handler;
        handler.service(request, response);
    } else if (path == "/api/setConfig") {
        SetConfigHandler handler;
        handler.service(request, response);
    } else if (path == "/api/closeReader") {
        CloseReaderHandler handler;
        handler.service(request, response);
    } else {
        response.setStatus(404, "Not found");
        response.write("The URL is wrong!", true);
    }
}
