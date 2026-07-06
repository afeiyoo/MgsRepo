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

        BaseException::EM_ExceptionType type = e.type();
        if (type == BaseException::QUERY_REPEAT) {
            resMap["judgeResult"] = e.errCode();
        } else if (type == BaseException::SAVE_DATA) {
            resMap["errCode"] = e.errCode();
            resMap["errorMessage"] = e.errDesc();
        } else if (type == BaseException::QUERY_XZPASS) {
            resMap["errCode"] = e.errCode();
            resMap["returnMessage"] = e.errDesc();
        } else if (type == BaseException::QUERY_SHIFT) {
            resMap["errCode"] = e.errCode();
            resMap["errorMessage"] = e.errDesc();
            resMap["Count"] = 0;
        } else if (type == BaseException::QUERY_DATA) {
            resMap["errCode"] = e.errCode();
            resMap["errorMessage"] = e.errDesc();
        } else if (type == BaseException::QUERY_ETC_BLACK) {
            resMap["queryResult"] = e.errCode();
            resMap["version"] = "";
            resMap["OperateTable"] = 0;
            resMap["amount"] = 0;
            resMap["errorMessage"] = e.errDesc();
        } else {
            resMap["errCode"] = e.errCode();
            resMap["errorMessage"] = e.errDesc();
        }

        QByteArray sendData = DataDealUtils::mapToJson(resMap);
        LOG_INFO().noquote() << QString("response size: %1 <==\n%2").arg(sendData.size()).arg(QString::fromUtf8(sendData.left(1024)));
        response.write(sendData, true);
    }
}
