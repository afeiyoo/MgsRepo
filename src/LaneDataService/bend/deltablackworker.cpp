#include "deltablackworker.h"

#include "EasyQtSql.h"
#include "HttpClient/src/http.h"
#include "Logger.h"
#include "T_ETCBlackCardList_1.h"
#include "T_ETCBlackCardList_2.h"
#include "config/config.h"
#include "core/globalmanager.h"
#include "core/signalmanager.h"
#include "env/defines.h"
#include "env/environment.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"
#include "utils/stdafx.h"

#include <memory>

using namespace Utils;
using namespace EasyQtSql;

DeltaBlackWorker::DeltaBlackWorker(QObject *parent)
    : QObject{parent}
{
    m_timer = new QTimer(this);
}

DeltaBlackWorker::~DeltaBlackWorker()
{
    if (m_dao.isOpen())
        m_dao.close();
    m_dao = QSqlDatabase();
    QSqlDatabase::removeDatabase("deltaBlack");

    m_timer->stop();

    SAFE_DELETE(m_client);
}

void DeltaBlackWorker::onInit()
{
    m_client = new Http();

    // 增量数据库连接初始化
    m_dao = QSqlDatabase::addDatabase("QSQLITE", "deltaBlack");

    // 每隔5分钟检查一次增量
    m_timer->setInterval(5 * 60 * 1000);
    connect(m_timer, &QTimer::timeout, this, &DeltaBlackWorker::onCheckDeltaBlack);

    // 首次全量检查完成后，立即进行增量检查
    connect(GM_INS->m_sigMan, &SignalManager::sigFullBlackFirstCheckFinished, this, &DeltaBlackWorker::onCheckDeltaBlack);

    m_timer->start();
}

bool DeltaBlackWorker::batchUpsertDeltaBlack(int operateTable, const QVariantList &blackDetails)
{
    if (blackDetails.isEmpty())
        return true;

    try {
        Transaction transaction(m_dao);
        int insertCount = 0;
        int updateCount = 0;

        for (int i = 0; i < blackDetails.size(); ++i) {
            const QVariantMap detail = blackDetails.at(i).toMap();
            if (detail.isEmpty()) {
                LOG_ERROR().noquote() << "保存增量数据失败: 第" << i << "条blackDetail不是有效对象";
                return false;
            }

            const QString insertTime = detail["InsertTime"].toString();
            const QString creationTime = detail["CreationTime"].toString();
            const QString cardId = detail["CardID"].toString().trimmed();
            if (insertTime.isEmpty() || creationTime.isEmpty() || cardId.isEmpty()) {
                LOG_ERROR().noquote() << "保存增量数据失败: 第" << i << "条blackDetail字段无效";
                return false;
            }

            // 构造对象
            std::unique_ptr<QObject> record;
            if (operateTable == 1) {
                record.reset(new T_ETCBlackCardList_1);
            } else {
                record.reset(new T_ETCBlackCardList_2);
            }

            QVariantMap recordData = detail;
            recordData["InsertTime"] = QDateTime::fromString(insertTime, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
            recordData["CreationTime"] = QDateTime::fromString(creationTime, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
            recordData["CardID"] = cardId;
            recordData["UpdateTime"] = QDateTime::currentDateTime();
            DataDealUtils::qvariant2qobject(recordData, record.get());

            const QString existSql = DataDealUtils::getExistSql(record.get());
            if (existSql.isEmpty()) {
                LOG_ERROR().noquote() << "保存增量数据失败: 第" << i << "条数据无法生成存在性查询SQL";
                return false;
            }

            const bool exists = transaction.scalar<int>(existSql) > 0;
            const QString saveSql = exists ? DataDealUtils::getUpdateSql(record.get()) : DataDealUtils::getInsertSql(record.get());
            if (saveSql.isEmpty()) {
                LOG_ERROR().noquote() << "保存增量数据失败: 第" << i << "条数据无法生成" << (exists ? "更新" : "插入") << "SQL";
                return false;
            }

            transaction.execNonQuery(saveSql);

            if (exists) {
                ++updateCount;
            } else {
                ++insertCount;
            }
        }

        if (!transaction.commit()) {
            LOG_ERROR().noquote() << "提交增量SQLite事务失败";
            return false;
        }

        LOG_INFO().noquote() << "增量SQLite保存成功: table" << (operateTable == 1 ? "T_ETCBlackCardList_1" : "T_ETCBlackCardList_2") << "接收数量"
                             << blackDetails.size() << "插入数量" << insertCount << "更新数量" << updateCount;
        return true;
    } catch (const DBException &e) {
        LOG_ERROR().noquote() << e.lastError << "\t" << e.lastQuery;
        return false;
    }
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

    // 增量数据库初始化
    FileName dbPath = FileName::fromString(GM_INS->m_conf->getConfigSnap().deltaBlackPath + QStringLiteral("/ETCBlackListDelta.db"));
    if (!dbPath.exists()) {
        LOG_INFO().noquote() << "增量异常: 增量SQLite文件不存在";
        setStatus(false, -1);
        return;
    }

    m_dao.setDatabaseName(dbPath.toString());
    if (!m_dao.isOpen() && !m_dao.open()) {
        LOG_ERROR().noquote() << "增量异常: 初始化增量SQLite数据库失败" << m_dao.lastError().text();
        setStatus(false, -2);
        return;
    }

    while (true) {
        QByteArray data = getDeltaBlackJson();
        if (data.isEmpty())
            break;

        bool saveOk = saveDeltaBlackJson(data);
        if (!saveOk)
            break;
        QThread::msleep(1000);
    }
}

QByteArray DeltaBlackWorker::getDeltaBlackJson()
{
    QString curFullBlackVersion = GM_INS->m_env->getEnvSnap().fullBlackVersion; // 获取当前全量版本
    QString curDeltaBlackVersion = getDeltaBlackVersion();                      // 获取当前增量版本

    if (!curDeltaBlackVersion.isEmpty())
        m_version = curDeltaBlackVersion;

    LOG_INFO().noquote() << "当前全量版本号:" << curFullBlackVersion << "当前增量版本号:" << curDeltaBlackVersion;

    if (curDeltaBlackVersion.isEmpty() && curFullBlackVersion.isEmpty()) {
        LOG_ERROR().noquote() << "当前全量版本号和增量版本号都为空，无法获取增量数据";
        setStatus(m_isValid, -3);
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

    ST_ConfigSnap snap = GM_INS->m_conf->getConfigSnap();
    LOG_INFO().noquote() << "请求获取增量数据:" << snap.stationServiceURL << "版本:" << reqDeltaBlackVersion;

    QByteArray resp;
    bool netOk = m_client->postSync(resp, QUrl(snap.stationServiceURL), reqData, "application/json");
    if (!netOk) {
        LOG_ERROR().noquote() << QString("增量异常: 获取增量数据失败. 原因 %1").arg(QString::fromUtf8(resp));
        setStatus(m_isValid, -4);
        return "";
    }
    if (resp.isEmpty()) {
        LOG_ERROR().noquote() << QString("增量异常: 获取增量数据返回为空");
        setStatus(m_isValid, -5);
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
        setStatus(m_isValid, -5);
        return false;
    }

    int queryRes = -1;
    QString version;
    int amount = 0;
    int operateTable = -1;
    QString errMsg;
    QVariantList blackDetails;
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
    if (oneMap.contains("blackDetail"))
        blackDetails = oneMap["blackDetail"].toList();

    LOG_INFO().noquote() << "下载得到的增量数据: version" << version << "amount:" << amount << "operateTable:" << operateTable
                         << "errorMessage:" << errMsg;

    if (queryRes == 2) { // 增量已追平，等待下一版本
        setStatus(true, 0);
        return false;
    }
    if (queryRes != 1) {
        LOG_ERROR().noquote() << "增量异常: 返回增量状态查询结果" << queryRes << errMsg;
        setStatus(m_isValid, -6);
        return false;
    }
    if (version.isEmpty()) {
        LOG_ERROR().noquote() << "增量异常: 返回version为空";
        setStatus(m_isValid, -5);
        return false;
    }
    if (amount != blackDetails.size()) {
        LOG_ERROR().noquote() << "增量异常: amount和blackDetail数量不一致 amount" << amount << "blackDetail" << blackDetails.size();
        setStatus(m_isValid, -5);
        return false;
    }

    // 有增量数据，将增量插入SQLite表
    if (queryRes == 1) {
        if (!batchUpsertDeltaBlack(operateTable, blackDetails)) {
            setStatus(m_isValid, -7);
            return false;
        }
    }

    int affected = updateDeltaBlackVersion(version);
    if (affected == 1) {
        m_version = version;
        setStatus(true, 0);
        return true;
    } else {
        setStatus(m_isValid, -8);
        return false;
    }
}

QString DeltaBlackWorker::getDeltaBlackVersion()
{
    QString sql = R"(SELECT envvalue FROM t_lanebaseenv WHERE envkey='BlackVer')";
    try {
        Database db(m_dao);
        QString version = db.scalar<QString>(sql);
        return version;
    } catch (const DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery;
        return "";
    }
}

int DeltaBlackWorker::updateDeltaBlackVersion(const QString &ver)
{
    try {
        Database db(m_dao);
        NonQueryResult res = db.update("t_lanebaseenv").set("envvalue", ver).where("envkey='BlackVer'");
        return res.numRowsAffected();
    } catch (const DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery;
        return -1;
    }
}
