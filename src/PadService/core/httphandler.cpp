#include "httphandler.h"

#include "Logger.h"
#include "bend/bizhandler.h"
#include "core/baseexception.h"
#include "utils/datadealutils.h"

using namespace Utils;
using namespace stefanfrings;

HttpHandler::HttpHandler(QObject *parent)
    : HttpRequestHandler{parent}
{}

HttpHandler::~HttpHandler() {}

void HttpHandler::service(HttpRequest &request, HttpResponse &response)
{
    QByteArray reqBody = request.getBody();
    LOG_INFO().noquote() << QString("request size: %1 ==>\n%2").arg(reqBody.size()).arg(QString(reqBody.left(1024)));

    if (reqBody.size() <= 0)
        return;

    bool jsonOk = false;
    QString jsonErr;
    QVariantMap aMap = DataDealUtils::jsonToMap(reqBody, &jsonOk, &jsonErr);

    try {
        if (!jsonOk) {
            LOG_ERROR().noquote() << "Json解析异常:" << jsonErr;
            throw BaseException(1, "响应失败: json整体数据解析异常");
        }

        int cmdType = aMap["cmdType"].toInt();
        QVariantMap dataMap = aMap["data"].toMap();
        if (dataMap.isEmpty())
            throw BaseException(1, "响应失败: Json解析异常(data字段内容为空)");

        BizHandler handler;
        QString dealtData = handler.doMainDeal(cmdType, dataMap, reqBody);

        aMap.remove("data");
        aMap.insert("data", dealtData);
        QByteArray sendData = DataDealUtils::mapToJson(aMap);

        LOG_INFO().noquote() << QString("response size: %1 <==\n%2").arg(sendData.size()).arg(QString(sendData.left(1024)));
        response.write(sendData, true);
    } catch (const BaseException &e) {
        QString dealtData = QString::fromUtf8(R"({"status":"%1","desc":"%2"})").arg(e.errCode()).arg(e.errDesc());

        aMap.remove("data");
        aMap.insert("data", dealtData);
        QByteArray sendData = DataDealUtils::mapToJson(aMap);

        LOG_INFO().noquote() << QString("response size: %1 <==\n%2").arg(sendData.size()).arg(QString(sendData.left(1024)));
        response.write(sendData, true);
    }
}
