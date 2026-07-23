#include "closereaderhandler.h"

#include "Logger.h"
#include "core/globalmanager.h"
#include "core/readerctrl.h"
#include "utils/datadealutils.h"

using namespace Utils;

CloseReaderHandler::CloseReaderHandler(QObject *parent)
    : HttpRequestHandler{parent}
{}

CloseReaderHandler::~CloseReaderHandler() {}

void CloseReaderHandler::service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response)
{
    const QByteArray reqData = request.getBody();
    LOG_INFO().noquote() << "远程辅助请求关闭读卡器:" << reqData;

    GM_INS->m_reader->closeReader();

    QVariantMap resMap;
    resMap["Result"] = 1;
    resMap["Desc"] = "读卡器已关闭";
    QByteArray resData = DataDealUtils::mapToJson(resMap);
    LOG_INFO().noquote() << "向远程辅助返回:" << resData;

    response.write(resData, true);
}
