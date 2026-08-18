#include "authpsamhandler.h"

#include "Logger.h"
#include "core/globalmanager.h"
#include "core/readerctrl.h"
#include "utils/datadealutils.h"

using namespace Utils;

AuthPsamHandler::AuthPsamHandler(QObject *parent)
    : HttpRequestHandler{parent}
{}

AuthPsamHandler::~AuthPsamHandler() {}

void AuthPsamHandler::service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response)
{
    const QByteArray reqData = request.getBody();
    LOG_INFO().noquote() << "远程辅助请求PSAM卡授权:" << reqData;

    QVariantMap resMap;

    bool ok = false;
    QString errDesc;
    QVariantMap reqMap = DataDealUtils::jsonToMap(reqData, &ok, &errDesc);

    if (!ok) {
        LOG_ERROR().noquote() << "PSAM卡授权失败: 请求数据解析失败" << errDesc;
        resMap["ErrorCode"] = 1;
        resMap["HexReply"] = "";
        QByteArray resData = DataDealUtils::mapToJson(resMap);
        LOG_INFO().noquote() << "向远程辅助返回:" << resData;
        response.write(resData, true);
        return;
    }

    QString hexReply;
    QString hexCosCommand = reqMap["HexCosCommand"].toString();
    int errCode = GM_INS->m_reader->authPsam(hexCosCommand, hexReply);
    if (errCode == 0) {
        LOG_INFO().noquote() << "PSAM卡授权成功";
    } else {
        LOG_INFO().noquote() << "PSAM卡授权失败";
    }

    resMap["ErrorCode"] = errCode;
    resMap["HexReply"] = hexReply;
    QByteArray resData = DataDealUtils::mapToJson(resMap);
    LOG_INFO().noquote() << "向远程辅助返回:" << resData;
    response.write(resData, true);
}
