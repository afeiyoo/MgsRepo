#include "growthblackworker.h"

#include "HttpClient/src/http.h"
#include "Logger.h"
#include "config/config.h"
#include "core/globalmanager.h"
#include "env/environment.h"
#include "utils/datadealutils.h"
#include "utils/stdafx.h"

using namespace Utils;

GrowthBlackWorker::GrowthBlackWorker(QObject *parent)
    : QObject{parent}
{
    m_timer = new QTimer(this);
}

GrowthBlackWorker::~GrowthBlackWorker()
{
    SAFE_DELETE(m_client);
}

void GrowthBlackWorker::onInit()
{
    m_client = new Http();

    m_timer->setInterval(5 * 60 * 1000);
    m_timer->start();
}

void GrowthBlackWorker::onCheckGrowthBlack()
{
    LOG_INFO().noquote() << "开始检查增量...";

    while (1) {
        QByteArray data = getGrowthBlackJson();
        if (data.isEmpty()) {
            LOG_ERROR().noquote() << "获取增量状态名单异常";
            break;
        }

        bool saveOk = saveGrowthBlackJson(data);
        if (!saveOk)
            break;
        QThread::msleep(1000 * 5);
    }
}

QByteArray GrowthBlackWorker::getGrowthBlackJson()
{
    // TODO 获取当前增量版本（数据库查询）
    QString curGrowthBlackVersion;
    QString curFullBlackVersion = GM_INS->m_env->fullBlackVersion();

    if (curGrowthBlackVersion.isEmpty() && curFullBlackVersion.isEmpty()) {
        LOG_ERROR().noquote() << "当前全量版本和增量版本都为空，无法确定增量版本";
        return "";
    }

    if (curGrowthBlackVersion.isEmpty()) {
        // 当前没有增量版本，以全量版本当天零点为基准
        LOG_INFO().noquote() << "当前增量版本为空，以全量版本为基线开始获取增量";
        QString tempStr = curFullBlackVersion + QStringLiteral("0000");
        QDateTime dt = QDateTime::fromString(tempStr, "yyyyMMddhhmm").addSecs(-5 * 60);
        curGrowthBlackVersion = dt.toString("yyyyMMddhhmmss");
    } else if (!curFullBlackVersion.isEmpty() && curGrowthBlackVersion.left(8) < curFullBlackVersion) {
        // 全量版本和增量版本都有，但是全量版本日期更新
        LOG_INFO().noquote() << "当前增量版本不为空，但是小于全量版本，以全量版本为基线获取增量";
        QString tempStr = curFullBlackVersion + QStringLiteral("0000");
        QDateTime dt = QDateTime::fromString(tempStr, "yyyyMMddhhmm").addSecs(-5 * 60);
        curGrowthBlackVersion = dt.toString("yyyyMMddhhmmss");
    }

    // 下载下一个版本增量
    QDateTime dts = QDateTime::fromString(curGrowthBlackVersion, "yyyyMMddhhmm").addSecs(5 * 60);
    QString version = dts.toString("yyyyMMddhhmm");

    QVariantMap reqMap;
    reqMap["version"] = version;
    reqMap["queryType"] = "queryETCBlack";
    QByteArray reqJson = DataDealUtils::mapToJson(reqMap);

    QByteArray resp;
    bool netOk = m_client->postSync(resp, QUrl(GM_INS->m_conf->m_stationServiceURL), reqJson, "application/json");
    if (!netOk) {
        LOG_ERROR().noquote() << "向" << GM_INS->m_conf->m_stationServiceURL << "请求增量失败: 版本" << version << "原因" << resp;
        return "";
    }
    if (resp.isEmpty()) {
        LOG_ERROR().noquote() << "增量内容返回为空: 版本" << version;
        return "";
    }

    return resp;
}

bool GrowthBlackWorker::saveGrowthBlackJson(const QByteArray &data) {}
