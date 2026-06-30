#include "httphandler.h"

#include "Logger.h"
#include "bend/mainhandler.h"
#include "core/baseexception.h"
#include "utils/datadealutils.h"

using namespace Utils;

HttpHandler::HttpHandler(QObject *parent)
    : HttpRequestHandler{parent}
{}

HttpHandler::~HttpHandler() {}

void HttpHandler::service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response)
{
    QString peerIP = QHostAddress(request.getPeerAddress().toIPv4Address()).toString();
    QByteArray reqBody = request.getBody();

    LOG_INFO().noquote() << QString("request size: %1 ==>\n%2").arg(reqBody.size()).arg(QString::fromUtf8(reqBody.left(1024)));
    if (reqBody.size() <= 0)
        return;

    try {
        MainHandler handler(peerIP);
        QString dealtData = handler.doMainDeal(reqBody);

        QByteArray sendData = dealtData.toUtf8();
        LOG_INFO().noquote() << QString("response size: %1 <==\n%2").arg(sendData.size()).arg(QString::fromUtf8(sendData.left(1024)));
        response.write(sendData, true);
    } catch (const BaseException &e) {
        QVariantMap resMap;
        resMap["errCode"] = e.status();
        resMap["errorMessage"] = e.desc();

        QByteArray sendData = DataDealUtils::mapToJson(resMap);
        LOG_INFO().noquote() << QString("response size: %1 <==\n%2").arg(sendData.size()).arg(QString::fromUtf8(sendData.left(1024)));
        response.write(sendData, true);
    }
}
