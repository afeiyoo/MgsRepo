#include "verifycardhandler.h"

#include "Logger.h"
#include "core/globalmanager.h"
#include "core/readerctrl.h"
#include "utils/datadealutils.h"

using namespace Utils;

VerifyCardHandler::VerifyCardHandler(QObject *parent)
    : HttpRequestHandler{parent}
{}

VerifyCardHandler::~VerifyCardHandler() {}

void VerifyCardHandler::service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response)
{
    const QByteArray reqData = request.getBody();
    LOG_INFO().noquote() << "远程辅助请求身份卡验证:" << reqData;

    QVariantMap resMap;

    bool ok = false;
    QString errDesc;
    QVariantMap reqMap = DataDealUtils::jsonToMap(reqData, &ok, &errDesc);
    if (!ok) {
        LOG_INFO().noquote() << "修改配置失败: 请求数据解析失败" << errDesc;
        resMap["ErrorCode"] = -1;
        QByteArray resData = DataDealUtils::mapToJson(resMap);
        LOG_INFO().noquote() << "向远程辅助返回:" << resData;
        response.write(resData, true);
        return;
    }

    QString cardNum = reqMap["CardNum"].toString();
    QString passWord = reqMap["Password"].toString();
    int dealStatus = reqMap["DealStatus"].toInt();

    int errCode = GM_INS->m_reader->verifyCard(cardNum, passWord, dealStatus);
    if (errCode == 0) {
        LOG_INFO().noquote() << "身份卡验证成功";
    } else {
        LOG_ERROR().noquote() << "身份卡验证失败";
    }

    resMap["ErrorCode"] = errCode;
    QByteArray resData = DataDealUtils::mapToJson(resMap);
    LOG_INFO().noquote() << "向远程辅助返回:" << resData;
    response.write(resData, true);
}
