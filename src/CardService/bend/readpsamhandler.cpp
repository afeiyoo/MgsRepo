#include "readpsamhandler.h"

#include "Logger.h"
#include "core/globalmanager.h"
#include "core/readerctrl.h"
#include "utils/datadealutils.h"

using namespace Utils;

ReadPsamHandler::ReadPsamHandler(QObject *parent)
    : HttpRequestHandler{parent}
{}

ReadPsamHandler::~ReadPsamHandler() {}

void ReadPsamHandler::service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response)
{
    const QByteArray reqData = request.getBody();
    LOG_INFO().noquote() << "远程辅助请求读取PSAM卡信息:" << reqData;

    ST_PsamReadResult result{};
    bool ok = GM_INS->m_reader->readPsam(result);
    if (!ok) {
        LOG_ERROR().noquote() << "读取PSAM卡信息失败";
    } else {
        LOG_INFO().noquote() << "读取PSAM卡信息成功";
    }

    QVariantMap resMap;
    resMap["ErrorCode"] = result.errorCode;
    resMap["TermNo"] = result.termNo;
    resMap["PsamNo"] = result.psamNo;
    resMap["IsNeedAuth"] = result.isNeedAuth;
    resMap["Random"] = result.random;
    QByteArray resData = DataDealUtils::mapToJson(resMap);
    LOG_INFO().noquote() << "向远程辅助返回:" << resData;

    response.write(resData, true);
}
