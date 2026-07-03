#include "requestmapper.h"

#include "HttpServer/staticfilecontroller.h"
#include "Logger.h"
#include "globalmanager.h"
#include "httphandler.h"

RequestMapper::RequestMapper(QObject *parent)
    : HttpRequestHandler{parent}
{}

RequestMapper::~RequestMapper() {}

void RequestMapper::service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response)
{
    QByteArray path = request.getPath();

    LOG_INFO().noquote() << "RequestMapper: path=" << path.data();
    if (path == "/") {
        HttpHandler handler;
        handler.service(request, response);
    } else if (path.startsWith("/BlackUpdate")) {
        GM_INS->m_staticFileController->service(request, response);
    } else {
        response.setStatus(404, "Not Found");
        response.write("The Url is Wrong!");
    }
}
