#include "growthblackworker.h"

#include "HttpClient/src/http.h"
#include "Logger.h"
#include "config/config.h"
#include "core/globalmanager.h"
#include "env/defines.h"
#include "env/environment.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"
#include "utils/stdafx.h"

using namespace Utils;

DeltaBlackWorker::DeltaBlackWorker(QObject *parent)
    : QObject{parent}
{
    m_timer = new QTimer(this);
}

DeltaBlackWorker::~DeltaBlackWorker()
{
    SAFE_DELETE(m_client);
}

void DeltaBlackWorker::onInit()
{
    m_client = new Http();

    FileUtils::makeSureDirExist(FileName::fromString(GM_INS->m_conf->getConfigSnap().deltaBlackPath));

    m_db = QSqlDatabase::addDatabase("QSQLITE", "deltaBlack");

    // 启动计时器
    m_timer->setInterval(5 * 60 * 1000);
    m_timer->start();
}

void DeltaBlackWorker::setStatus(bool isValid, int status)
{
    m_isValid = isValid;
    m_curStatus = status;
    GM_INS->m_env->updateDeltaBlackEnvs(m_isValid, m_curStatus, m_version);
}

void DeltaBlackWorker::onCheckDeltaBlack()
{
    LOG_INFO().noquote() << "开始检查增量...";

    while (true) {
        QByteArray data = getDeltaBlackJson();
        if (data.isEmpty())
            break;

        bool saveOk = saveDeltaBlackJson(data);
        if (!saveOk)
            break;
        QThread::msleep(1000 * 5);
    }
}

QByteArray DeltaBlackWorker::getDeltaBlackJson()
{
    ST_ConfigSnap snap = GM_INS->m_conf->getConfigSnap();
    QString curFullBlackVersion = snap.fullBatchNo;   // 获取当前全量批次
    QString curDeltaBlackVersion = snap.deltaBatchNo; // 获取当前增量批次

    LOG_INFO().noquote() << "当前全量版本号:" << curFullBlackVersion << "当前增量版本号:" << curDeltaBlackVersion;

    if (curDeltaBlackVersion.isEmpty() && curFullBlackVersion.isEmpty()) {
        LOG_ERROR().noquote() << "当前全量版本号和增量版本号都为空，无法获取增量数据";
        setStatus(m_isValid, -1);
        return "";
    }

    if (curDeltaBlackVersion.isEmpty()) {
        // 当前没有增量版本，以全量版本当天零点为基准
        LOG_INFO().noquote() << "当前增量版本号为空，以全量版本号为基线开始获取增量数据";
        QString tempStr = curFullBlackVersion + QStringLiteral("0000");
        curDeltaBlackVersion = QDateTime::fromString(tempStr, "yyyyMMddhhmm").addSecs(-5 * 60).toString("yyyyMMddhhmm");
    } else if (!curFullBlackVersion.isEmpty() && curDeltaBlackVersion.left(8) < curFullBlackVersion) {
        // 全量版本和增量版本都有，但是全量版本日期更新
        LOG_INFO().noquote() << "当前增量版本号不为空，但是小于全量版本号，以全量版本号为基线重新获取增量数据";
        QString tempStr = curFullBlackVersion + QStringLiteral("0000");
        curDeltaBlackVersion = QDateTime::fromString(tempStr, "yyyyMMddhhmm").addSecs(-5 * 60).toString("yyyyMMddhhmm");
    }

    // 开始获取增量数据
    QDateTime nextDt = QDateTime::fromString(curDeltaBlackVersion, "yyyyMMddhhmm").addSecs(5 * 60);
    QString reqDeltaBlackVersion = nextDt.toString("yyyyMMddhhmm");

    QVariantMap reqMap;
    reqMap["version"] = reqDeltaBlackVersion;
    reqMap["queryType"] = "queryETCBlack";
    QByteArray reqData = DataDealUtils::mapToJson(reqMap);
    LOG_INFO().noquote() << "请求获取增量数据:" << snap.stationServiceURL << "版本:" << reqDeltaBlackVersion;

    QByteArray resp;
    bool netOk = m_client->postSync(resp, QUrl(snap.stationServiceURL), reqData, "application/json");
    if (!netOk) {
        LOG_ERROR().noquote() << QString("增量异常: 获取版本增量数据失败. 原因 %1").arg(QString::fromUtf8(resp));
        setStatus(m_isValid, -2);
        return "";
    }
    if (resp.isEmpty()) {
        LOG_ERROR().noquote() << QString("增量异常: 获取增量数据返回为空");
        setStatus(m_isValid, -3);
        return "";
    }

    return resp;
}

bool DeltaBlackWorker::saveDeltaBlackJson(const QByteArray &data)
{
    bool jsonOk = false;
    QString jsonErrDesc;
    QVariantMap oneMap = DataDealUtils::jsonToMap(data, &jsonOk, &jsonErrDesc);
    if (!jsonOk) {
        LOG_ERROR().noquote() << "增量异常: 增量数据解析失败 原因" << jsonErrDesc;
        setStatus(m_isValid, -3);
        return "";
    }

    int queryRes = -1;
    QString version;
    int amount = 0;
    int operateTable = -1;
    QString errMsg;
    if (oneMap.contains("queryResult"))
        queryRes = oneMap["queryResult"].toInt();
    if (oneMap.contains("version"))
        version = oneMap["version"].toString();
    if (oneMap.contains("amount"))
        amount = oneMap["amount"].toInt();
    if (oneMap.contains("OperateTable"))
        operateTable = oneMap["OperateTable"].toInt();
    if (oneMap.contains("errorMessage"))
        errMsg = oneMap["errorMessage"].toString();

    LOG_INFO().noquote() << "下载得到的增量数据: version" << version << "amount:" << amount << "operateTable:" << operateTable
                         << "errorMessage:" << errMsg;

    if (version.isEmpty()) {
        LOG_ERROR().noquote() << "增量异常: 返回version为空";
        setStatus(m_isValid, -3);
        return false;
    }
    if (operateTable == -1) {
        LOG_ERROR().noquote() << "增量异常: 返回OperateTable为空";
        setStatus(m_isValid, -3);
        return false;
    }
    if (queryRes != 1 && queryRes != 0) {
        LOG_ERROR().noquote() << "增量异常: 返回增量状态查询结果" << queryRes << errMsg;
        setStatus(m_isValid, -4);
        return false;
    }

    if (operateTable == 1) {
    } else {
    }
}
