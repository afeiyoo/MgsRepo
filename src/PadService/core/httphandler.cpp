#include "httphandler.h"

#include "Logger.h"
#include "NlohmannJson/nlojson.hpp"
#include "bend/bizhandler.h"
#include "core/baseexception.h"

using namespace stefanfrings;

HttpHandler::HttpHandler(QObject *parent)
    : HttpRequestHandler{parent}
{}

HttpHandler::~HttpHandler() {}

void HttpHandler::service(HttpRequest &request, HttpResponse &response)
{
    QByteArray reqBody = request.getBody();
    LOG_INFO().noquote() << QString("request size: %1 ==>\n%2").arg(reqBody.size()).arg(QString::fromUtf8(reqBody.left(1024)));

    if (reqBody.size() <= 0)
        return;

    NloJson nloJson;

    bool ok;
    QVariantMap aMap = nloJson.parse(reqBody, &ok).toMap();
    if (!ok) {
        LOG_ERROR().noquote() << "Json解析异常，数据整体内容错误";
        return;
    }

    try {
        int cmdType = aMap["cmdType"].toInt();
        QVariantMap dataMap = aMap["data"].toMap();
        if (dataMap.isEmpty())
            throw BaseException(1, "响应失败: Json解析异常(data字段内容为空)");

        BizHandler handler;
        QString dealtData = handler.doMainDeal(cmdType, dataMap, reqBody);

        aMap.remove("data");
        aMap.insert("data", dealtData);
        QByteArray sendData = nloJson.serialize(aMap);

        LOG_INFO().noquote() << QString("response size: %1 <==\n%2").arg(sendData.size()).arg(QString::fromUtf8(sendData.left(1024)));
        response.write(sendData, true);
    } catch (const BaseException &e) {
        QString dealtData = QString::fromUtf8(R"({"status":"%1","desc":"%2"})").arg(e.errCode()).arg(e.errDesc());

        aMap.remove("data");
        aMap.insert("data", dealtData);
        QByteArray sendData = nloJson.serialize(aMap);

        LOG_INFO().noquote() << QString("response size: %1 <==\n%2").arg(sendData.size()).arg(QString::fromUtf8(sendData.left(1024)));
        response.write(sendData, true);
    }
}
