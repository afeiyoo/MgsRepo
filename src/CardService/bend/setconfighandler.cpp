#include "setconfighandler.h"

#include "Logger.h"
#include "config/config.h"
#include "core/globalmanager.h"
#include "utils/datadealutils.h"

using namespace Utils;

SetConfigHandler::SetConfigHandler(QObject *parent)
    : HttpRequestHandler{parent}
{}

SetConfigHandler::~SetConfigHandler() {}

void SetConfigHandler::service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response)
{
    const QByteArray reqData = request.getBody();
    LOG_INFO().noquote() << "远程辅助请求修改配置:" << reqData;

    QVariantMap resMap;

    bool ok = false;
    QString errDesc;
    QVariantMap reqMap = DataDealUtils::jsonToMap(reqData, &ok, &errDesc);
    if (!ok) {
        LOG_INFO().noquote() << "修改配置失败: 请求数据解析失败" << errDesc;
        resMap["ErrorCode"] = -1;
        resMap["Desc"] = errDesc;
        QByteArray resData = DataDealUtils::mapToJson(resMap);
        LOG_INFO().noquote() << "向远程辅助返回:" << resData;
        response.write(resData, true);
        return;
    }

    QString apiName = reqMap["ApiName"].toString();
    QString comPort = reqMap["ComPort"].toString();
    int slotNo = reqMap["SlotNo"].toInt();
    int apiType = reqMap["ApiType"].toInt();

    GM_INS->m_config->updateConfig(apiType, apiName, comPort, slotNo);

    LOG_INFO().noquote() << QString("配置修改成功: apiName %1, apiType %2, comPort %3, slotNo %4").arg(apiName).arg(apiType).arg(comPort).arg(slotNo);
    resMap["ErrorCode"] = 0;
    resMap["Desc"] = "成功修改配置";
    QByteArray resData = DataDealUtils::mapToJson(resMap);
    LOG_INFO().noquote() << "向远程辅助返回:" << resData;
    response.write(resData, true);
}
