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
    m_http = new Http();

    // 首次获得可用活动全量后，立即进行增量检查
    connect(GM_INS->m_sigMan, &SignalManager::sigFullBlackReadyForDelta, this, &DeltaBlackWorker::onFullBlackReadyForDelta);
    connect(GM_INS->m_sigMan, &SignalManager::sigCleanETCBlackCard, this, &DeltaBlackWorker::onCleanETCBlackCard);
    connect(GM_INS->m_sigMan, &SignalManager::sigFullBlackSwitchFinished, this, &DeltaBlackWorker::onFullBlackSwitchFinished);
}

DeltaBlackWorker::~DeltaBlackWorker()
{
    if (m_dao.isOpen())
        m_dao.close();
    m_dao = QSqlDatabase();
    QSqlDatabase::removeDatabase("deltaBlack");

    m_timer->stop();

    SAFE_DELETE(m_http);
}

void DeltaBlackWorker::onInit()
{
    // 每隔5分钟检查一次增量
    m_timer->setInterval(5 * 60 * 1000);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &DeltaBlackWorker::onCheckDeltaBlack);

    // 启动时先尝试连接；失败不影响后续收到信号或定时重试
    if (ensureDatabaseConnected())
        setStatus(false, DeltaBlackWaitingForCheck);
}

void DeltaBlackWorker::onFullBlackReadyForDelta()
{
    m_fullBlackReady = true;
    onCheckDeltaBlack();
}

bool DeltaBlackWorker::applyDeltaBatch(int operateTable, const QVariantList &blackDetails, const QString &version)
{
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

            const QDateTime insertDateTime = QDateTime::fromString(insertTime, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
            const QDateTime creationDateTime = QDateTime::fromString(creationTime, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
            if (!insertDateTime.isValid() || !creationDateTime.isValid()) {
                LOG_ERROR().noquote() << "保存增量数据失败: 第" << i << "条blackDetail时间格式无效";
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
            recordData["InsertTime"] = insertDateTime;
            recordData["CreationTime"] = creationDateTime;
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

        const NonQueryResult versionResult = transaction.update("t_lanebaseenv").set("envvalue", version).where("envkey='BlackVer'");
        if (versionResult.numRowsAffected() != 1) {
            LOG_ERROR().noquote() << "保存增量数据失败: 更新增量版本影响行数" << versionResult.numRowsAffected();
            return false;
        }

        if (!transaction.commit()) {
            LOG_ERROR().noquote() << "提交增量数据与版本事务失败";
            return false;
        }

        m_version = version;
        LOG_INFO().noquote() << "增量SQLite保存成功: table" << (operateTable == 1 ? "T_ETCBlackCardList_1" : "T_ETCBlackCardList_2") << "接收数量"
                             << blackDetails.size() << "插入数量" << insertCount << "更新数量" << updateCount << "版本" << version;
        return true;
    } catch (const DBException &e) {
        LOG_ERROR().noquote() << e.lastError << "\t" << e.lastQuery;
        return false;
    }
}

void DeltaBlackWorker::setStatus(bool isValid, EM_DeltaBlackStatus status)
{
    m_isValid = isValid;
    m_status = status;
    GM_INS->m_env->updateDeltaBlackEnvs(m_isValid, m_status, m_version);
}

void DeltaBlackWorker::onCheckDeltaBlack()
{
    if (!m_fullBlackReady) {
        LOG_INFO().noquote() << "活动全量尚未就绪，增量检查继续等待";
        return;
    }

    if (m_fullSwitchInProgress) {
        LOG_INFO().noquote() << "全量清表及切换正在进行，暂停本次增量检查";
        return;
    }

    if (m_checkInProgress) {
        LOG_INFO().noquote() << "增量检查正在进行，本次触发跳过";
        return;
    }

    LOG_INFO().noquote() << "开始检查增量...";

    m_timer->stop();
    // 只有站级明确返回“已是最新版本”后，增量才重新变为有效。
    // 检查及追赶期间无法确认本地是否最新，因此先撤销有效性。
    setStatus(false, DeltaBlackApplying);
    m_checkInProgress = true;
    processNextDeltaBatch();
}

void DeltaBlackWorker::processNextDeltaBatch()
{
    if (m_fullSwitchInProgress)
        return;

    if (!ensureDatabaseConnected()) {
        finishCurrentCheck();
        return;
    }

    QByteArray responseData;
    if (!requestNextDeltaData(responseData)) {
        finishCurrentCheck();
        return;
    }

    if (!processDeltaResponse(responseData)) {
        finishCurrentCheck();
        return;
    }

    // 让出工作线程事件循环，使已经排队的全量清表信号先得到处理。
    QTimer::singleShot(1000, this, &DeltaBlackWorker::processNextDeltaBatch);
}

void DeltaBlackWorker::finishCurrentCheck()
{
    m_checkInProgress = false;
    scheduleNextCheck();
}

void DeltaBlackWorker::onCleanETCBlackCard(int fullBatchNo, QString tableName)
{
    m_fullSwitchInProgress = true;
    m_switchFullBatchNo = fullBatchNo;
    m_timer->stop();
    // 清表及全量切换完成后还需要重新向站级确认最新版本。
    setStatus(false, DeltaBlackApplying);

    tableName = tableName.trimmed();
    if (tableName.compare("T_ETCBlackCardList_1", Qt::CaseInsensitive) == 0) {
        tableName = "T_ETCBlackCardList_1";
    } else if (tableName.compare("T_ETCBlackCardList_2", Qt::CaseInsensitive) == 0) {
        tableName = "T_ETCBlackCardList_2";
    } else {
        const QString error = QString("非法增量清理表名: %1").arg(tableName);
        LOG_ERROR().noquote() << error;
        emit GM_INS->m_sigMan->sigCleanETCBlackCardFinished(fullBatchNo, tableName, false, 0, error);
        return;
    }

    int affected = 0;
    QString error;
    const bool success = tryCleanETCBlackCard(tableName, affected, error);
    emit GM_INS->m_sigMan->sigCleanETCBlackCardFinished(fullBatchNo, tableName, success, affected, error);
}

void DeltaBlackWorker::onFullBlackSwitchFinished(int fullBatchNo, bool activated)
{
    if (!m_fullSwitchInProgress || fullBatchNo != m_switchFullBatchNo)
        return;

    LOG_INFO().noquote() << "全量切换握手结束，批次:" << fullBatchNo << "是否激活:" << activated;
    m_fullSwitchInProgress = false;
    m_switchFullBatchNo = 0;

    if (!m_fullBlackReady)
        return;

    if (m_checkInProgress) {
        QTimer::singleShot(0, this, &DeltaBlackWorker::processNextDeltaBatch);
    } else {
        onCheckDeltaBlack();
    }
}

bool DeltaBlackWorker::tryCleanETCBlackCard(const QString &tableName, int &affected, QString &error)
{
    affected = 0;
    error.clear();

    if (!ensureDatabaseConnected()) {
        error = "增量数据库未连接";
        return false;
    }

    try {
        Transaction transaction(m_dao);
        const NonQueryResult result = transaction.deleteFrom(tableName).where("1=1");
        affected = result.numRowsAffected();
        if (!transaction.commit()) {
            error = transaction.lastError().text();
            return false;
        }

        return true;
    } catch (const DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery;
        error = e.lastError.text();
        return false;
    }
}

bool DeltaBlackWorker::ensureDatabaseConnected()
{
    if (m_dao.isValid() && m_dao.isOpen())
        return true;

    const QString dbPath = QDir(GM_INS->m_conf->getConfigSnap().deltaBlackPath).filePath("ETCBlackListDelta.db");
    if (!QFileInfo::exists(dbPath)) {
        LOG_ERROR().noquote() << "增量SQLite数据库文件不存在";
        setStatus(false, DeltaBlackDBUnavailable);
        return false;
    }

    if (!m_dao.isValid()) {
        m_dao = QSqlDatabase::addDatabase("QSQLITE", "deltaBlack");
        m_dao.setDatabaseName(dbPath);
    }

    if (!m_dao.open()) {
        LOG_ERROR().noquote() << "增量SQLite数据库初始化失败" << m_dao.lastError().text();
        setStatus(false, DeltaBlackDBUnavailable);
        return false;
    }

    QString error;
    if (!validateDatabase(error)) {
        LOG_ERROR().noquote() << "增量SQLite数据库结构无效" << error;
        m_dao.close();
        setStatus(false, DeltaBlackDBUnavailable);
        return false;
    }

    LOG_INFO().noquote() << "增量SQLite数据库连接成功:" << dbPath;
    return true;
}

bool DeltaBlackWorker::validateDatabase(QString &error)
{
    error.clear();

    const QStringList tables = m_dao.tables(QSql::Tables);
    const auto containsTable = [&tables](const QString &requiredTable) {
        for (const QString &table : tables) {
            if (table.compare(requiredTable, Qt::CaseInsensitive) == 0)
                return true;
        }
        return false;
    };

    const QStringList requiredTables = {"T_ETCBlackCardList_1", "T_ETCBlackCardList_2", "T_LaneBaseEnv"};
    for (const QString &table : requiredTables) {
        if (!containsTable(table)) {
            error = QString("缺少必需表%1").arg(table);
            return false;
        }
    }

    try {
        Database database(m_dao);
        const QString versionCountSql = R"(SELECT COUNT(*) FROM T_LaneBaseEnv WHERE envkey='BlackVer')";
        const int versionRows = database.scalar<int>(versionCountSql);
        if (versionRows != 1) {
            error = QString("BlackVer记录数量异常:%1").arg(versionRows);
            return false;
        }
    } catch (const DBException &e) {
        error = e.lastError.text();
        return false;
    }

    return true;
}

void DeltaBlackWorker::scheduleNextCheck()
{
    m_timer->start();
}

bool DeltaBlackWorker::requestNextDeltaData(QByteArray &responseData)
{
    responseData.clear();

    const QString fullVersion = GM_INS->m_env->getEnvSnap().fullBlackVersion;
    const QString deltaVersion = fetchDeltaVersion();

    LOG_INFO().noquote() << "当前全量版本号:" << fullVersion << "当前增量版本号:" << deltaVersion;

    // 读取SQLite是在恢复最后成功提交的版本；只有落库事务成功才会推进该值。
    if (!deltaVersion.isEmpty())
        m_version = deltaVersion;

    if (deltaVersion.isEmpty() && fullVersion.isEmpty()) {
        LOG_ERROR().noquote() << "当前全量版本号和增量版本号都为空，无法获取增量数据";
        setStatus(false, DeltaBlackBaselineUnavailable);
        return false;
    }

    QString baselineVersion = deltaVersion;
    if (baselineVersion.isEmpty()) {
        LOG_INFO().noquote() << "当前增量版本号为空，以全量版本号为基线开始获取增量数据";
        const QDateTime fullBaseline = QDateTime::fromString(fullVersion + "0000", "yyyyMMddhhmm");
        baselineVersion = fullBaseline.addSecs(-5 * 60).toString("yyyyMMddhhmm");
    } else if (!fullVersion.isEmpty() && baselineVersion.left(8) < fullVersion) {
        LOG_INFO().noquote() << "当前增量版本号不为空，但是小于全量版本号，以全量版本号为基线重新获取增量数据";
        const QDateTime fullBaseline = QDateTime::fromString(fullVersion + "0000", "yyyyMMddhhmm");
        baselineVersion = fullBaseline.addSecs(-5 * 60).toString("yyyyMMddhhmm");
    }

    const QDateTime nextVersionTime = QDateTime::fromString(baselineVersion, "yyyyMMddhhmm").addSecs(5 * 60);
    const QString requestedVersion = nextVersionTime.toString("yyyyMMddhhmm");

    QVariantMap reqMap;
    reqMap["version"] = requestedVersion;
    reqMap["queryType"] = "queryETCBlack";
    QByteArray reqData = DataDealUtils::mapToJson(reqMap);

    ST_ConfigSnap snap = GM_INS->m_conf->getConfigSnap();
    LOG_INFO().noquote() << "请求获取增量数据:" << snap.stationServiceURL << "版本:" << requestedVersion;

    QByteArray response;
    const bool netOk = m_http->postSync(response, QUrl(snap.stationServiceURL), reqData, "application/json");
    if (!netOk) {
        LOG_ERROR().noquote() << QString("获取增量数据失败. 原因 %1").arg(QString::fromUtf8(response));
        setStatus(false, DeltaBlackRequestFailed);
        return false;
    }
    LOG_INFO().noquote() << "增量数据获取成功: data size" << response.size();
    responseData = response;

    return true;
}

bool DeltaBlackWorker::processDeltaResponse(const QByteArray &responseData)
{
    if (responseData.isEmpty()) {
        LOG_ERROR().noquote() << "增量数据内容为空";
        setStatus(false, DeltaBlackResponseInvalid);
        return false;
    }

    bool jsonOk = false;
    QString jsonErr;
    const QVariantMap responseMap = DataDealUtils::jsonToMap(responseData, &jsonOk, &jsonErr);
    if (!jsonOk) {
        LOG_ERROR().noquote() << "增量数据解析失败 原因" << jsonErr;
        setStatus(false, DeltaBlackResponseInvalid);
        return false;
    }

    int queryResult = -1;
    QString version;
    int amount = 0;
    int operateTable = -1;
    QVariantList blackDetails;
    if (responseMap.contains("queryResult"))
        queryResult = responseMap["queryResult"].toInt();
    if (responseMap.contains("version"))
        version = responseMap["version"].toString();
    if (responseMap.contains("amount"))
        amount = responseMap["amount"].toInt();
    if (responseMap.contains("OperateTable"))
        operateTable = responseMap["OperateTable"].toInt();
    if (responseMap.contains("blackDetail"))
        blackDetails = responseMap["blackDetail"].toList();

    LOG_INFO().noquote() << "下载得到的增量数据: queryResult:" << queryResult << "version:" << version << "amount:" << amount
                         << "operateTable:" << operateTable << "blackDetails:" << blackDetails.size();

    // 增量已追平，等待下一版本
    if (queryResult == 2) {
        LOG_INFO().noquote() << "当前增量版本已为最新";
        setStatus(true, DeltaBlackReady);
        return false;
    }

    if (queryResult != 1) {
        LOG_ERROR().noquote() << "增量数据返回queryResult异常";
        setStatus(false, DeltaBlackResponseInvalid);
        return false;
    }

    if (version.isEmpty() || !QDateTime::fromString(version, "yyyyMMddhhmm").isValid()) {
        LOG_ERROR().noquote() << "增量数据返回version无效";
        setStatus(false, DeltaBlackResponseInvalid);
        return false;
    }
    if (amount != blackDetails.size()) {
        LOG_ERROR().noquote() << "增量数据返回amount和blackDetail数量不一致 amount" << amount << "blackDetail" << blackDetails.size();
        setStatus(false, DeltaBlackResponseInvalid);
        return false;
    }
    if (operateTable != 1 && operateTable != 2) {
        LOG_ERROR().noquote() << "增量数据返回OperateTable无效" << operateTable;
        setStatus(false, DeltaBlackResponseInvalid);
        return false;
    }

    setStatus(false, DeltaBlackApplying);
    if (!applyDeltaBatch(operateTable, blackDetails, version)) {
        setStatus(false, DeltaBlackApplyFailed);
        return false;
    }

    setStatus(false, DeltaBlackApplying);
    return true;
}

QString DeltaBlackWorker::fetchDeltaVersion()
{
    const QString sql = R"(SELECT envvalue FROM t_lanebaseenv WHERE envkey='BlackVer')";
    try {
        Database database(m_dao);
        return database.scalar<QString>(sql);
    } catch (const DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery;
        return "";
    }
}
