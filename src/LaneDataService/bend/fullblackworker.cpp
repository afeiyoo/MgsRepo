#include "fullblackworker.h"

#include <algorithm>

#include <QMetaObject>
#include <QRegularExpression>
#include <QSqlError>

#include "EasyQtSql.h"
#include "HttpClient/src/http.h"
#include "Logger.h"
#include "config/config.h"
#include "core/globalmanager.h"
#include "core/signalmanager.h"
#include "dbs/dataservice.h"
#include "env/environment.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"
#include "utils/stdafx.h"

using namespace EasyQtSql;
using namespace Utils;

FullBlackWorker::FullBlackWorker(QObject *parent)
    : QObject{parent}
{
    m_timer = new QTimer(this);
    m_http = new Http();
}

FullBlackWorker::~FullBlackWorker()
{
    QString connectionNames[2];
    for (int i = 0; i < 2; ++i) {
        connectionNames[i] = m_dao[i].connectionName();
        if (m_dao[i].isOpen())
            m_dao[i].close();
    }

    // 先释放所有QSqlDatabase句柄
    m_dao[0] = QSqlDatabase();
    m_dao[1] = QSqlDatabase();
    for (const QString &name : connectionNames) {
        if (!name.isEmpty())
            QSqlDatabase::removeDatabase(name);
    }
    m_timer->stop();

    SAFE_DELETE(m_http);
}

void FullBlackWorker::onCheckFullBlack()
{
    LOG_INFO().noquote() << "开始检查全量...";

    if (m_updateRunning) {
        LOG_INFO().noquote() << "全量更新流程正在进行，本次检查跳过";
        return;
    }

    setStatus(m_isValid, FullBlackChecking);

    ST_FullManifest localManifest;
    ST_FullManifest remoteManifest;
    QString localError;
    QString remoteError;
    const bool localOk = readLocalManifest(&localManifest, &localError);
    const bool remoteOk = fetchRemoteManifest(&remoteManifest, &remoteError);

    if (localOk) {
        LOG_INFO().noquote() << "本地全量清单有效，批次:" << localManifest.batchNo;
    } else {
        LOG_WARNING().noquote() << "本地全量清单无效:" << localError;
    }
    if (remoteOk) {
        LOG_INFO().noquote() << "远程全量清单有效，批次:" << remoteManifest.batchNo;
    } else {
        LOG_WARNING().noquote() << "远程全量清单无效:" << remoteError;
    }

    // 本地和远程清单都不可用，没有可确定的全量批次
    if (!localOk && !remoteOk) {
        setStatus(m_isFirst ? false : m_isValid, m_isFirst ? FullBlackUnavailable : FullBlackCheckFailed);
        return;
    }

    // 本地清单无效，直接下载远程权威批次
    if (!localOk) {
        prepareFullDownload(remoteManifest);
        return;
    }

    // 本地批次落后，不加载旧全量，直接下载远程权威批次
    if (remoteOk && localManifest.batchNo < remoteManifest.batchNo) {
        prepareFullDownload(remoteManifest);
        return;
    }

    // 远程服务是批次权威来源。本地批次超前视为本地清单被异常修改，禁止加载和回退使用。
    if (remoteOk && localManifest.batchNo > remoteManifest.batchNo) {
        LOG_ERROR().noquote() << "本地全量批次高于远程权威批次，按远程批次重新下载修复。本地批次:" << localManifest.batchNo
                              << "远程批次:" << remoteManifest.batchNo;
        prepareFullDownload(remoteManifest, FullBlackLocalBatchAhead);
        return;
    }

    // 走到这里说明本地清单有效，且远程不可用或两端批次相等
    const int localBatchNo = localManifest.batchNo;
    if (!m_isFirst && m_isValid && localBatchNo == m_activeBatchNo) {
        LOG_INFO().noquote() << "当前活动全量已是本地清单批次，无需重复加载，批次:" << localBatchNo;
        setStatus(true, FullBlackReady);
        return;
    }

    const ST_ConfigSnap snap = GM_INS->m_conf->getConfigSnap();
    const QString localFilePath = snap.fullBlackPath + QString("/ETCBlackCard_%1.db").arg(localBatchNo);
    if (!loadFullBlack(localBatchNo, localFilePath)) {
        LOG_ERROR().noquote() << "本地全量加载失败，批次:" << localBatchNo;

        // 只要远程清单可用，就尝试通过下载恢复，包括本地和远程同批次的情况
        if (remoteOk) {
            prepareFullDownload(remoteManifest);
            return;
        }

        setStatus(m_isFirst ? false : m_isValid, FullBlackLocalLoadFailed);
        return;
    }

    LOG_INFO().noquote() << "本地全量加载成功，批次:" << localBatchNo;
    setStatus(true, FullBlackReady);
    pruneOldFiles(localBatchNo);
    finishFirstCheck();
}

void FullBlackWorker::prepareFullDownload(const ST_FullManifest &manifest, EM_FullBlackStatus status)
{
    m_pendingManifest = manifest;
    m_updateRunning = true;

    LOG_INFO().noquote() << "需要下载远程全量，批次:" << manifest.batchNo << "切片数量:" << manifest.slices.size();
    setStatus(false, status);
}

void FullBlackWorker::finishFirstCheck()
{
    if (!m_isFirst)
        return;

    m_isFirst = false;
    emit GM_INS->m_sigMan->sigFullBlackFirstCheckFinished();
}

void FullBlackWorker::onInit()
{
    connect(GM_INS->m_sigMan, &SignalManager::sigCleanETCBlackCardFinished, this, &FullBlackWorker::onCleanETCBlackCardFinished);

    // 全量数据库连接初始化
    for (int i = 0; i < 2; ++i) {
        const QString connName = QString("fullBlack_%1").arg(i, 2, 10, QChar('0'));
        m_dao[i] = QSqlDatabase::addDatabase("QSQLITE", connName);
    }

    // 每隔10分钟检查一次全量
    m_timer->setInterval(10 * 60 * 1000);
    connect(m_timer, &QTimer::timeout, this, &FullBlackWorker::onCheckFullBlack);

    // 程序加载时，立即进行全量检查
    onCheckFullBlack();

    m_timer->start();
}

void FullBlackWorker::onCleanETCBlackCardFinished(int affected)
{
    LOG_INFO().noquote() << "清理增量表完成: 影响行数" << affected;
}

Utils::optional<int> FullBlackWorker::getMaxBatchNoFromFiles(const QString &path) const
{
    const FileName dirPath = FileName::fromString(path);
    const FileNameList fileList = FileUtils::getFilesWithSuffix(dirPath, ".db");

    if (fileList.isEmpty())
        return nullopt;

    LOG_INFO().noquote() << "在" << dirPath.toString() << "下，找到" << fileList.size() << "个全量文件";
    const QRegularExpression fileNameRegex("^ETCBlackCard_(\\d+)\\.db$");
    int maxBatchNo = 0;
    for (const auto &file : fileList) {
        const QString fileName = file.fileName();
        const QRegularExpressionMatch match = fileNameRegex.match(fileName);
        if (!match.hasMatch())
            continue;

        bool ok = false;
        const int batchNo = match.captured(1).toInt(&ok);
        if (!ok || batchNo <= 0) {
            LOG_INFO().noquote() << "忽略格式不正确的数据库文件:" << fileName;
            continue;
        }

        if (batchNo > maxBatchNo)
            maxBatchNo = batchNo;
    }

    return maxBatchNo > 0 ? make_optional(maxBatchNo) : nullopt;
}

void FullBlackWorker::pruneOldFiles(int batchNo)
{
    const FileName dirPath = FileName::fromString(GM_INS->m_conf->getConfigSnap().fullBlackPath);
    if (!dirPath.exists())
        return;

    const FileNameList fileList = FileUtils::getFilesWithSuffix(dirPath, ".db");
    if (fileList.isEmpty())
        return;

    const QRegularExpression fileNameRegex("^ETCBlackCard_(\\d+)\\.db$");
    for (const auto &file : fileList) {
        const QString fileName = file.fileName();
        const QRegularExpressionMatch match = fileNameRegex.match(fileName);
        if (!match.hasMatch())
            continue;

        bool ok = false;
        const int tempBatchNo = match.captured(1).toInt(&ok);
        if (!ok || tempBatchNo <= 0)
            continue;

        if (tempBatchNo < batchNo) {
            if (!FileUtils::removeRecursively(file)) {
                LOG_INFO().noquote() << "删除旧批次全量文件失败:" << file.fileName();
                continue;
            }
            LOG_INFO().noquote() << "删除旧批次全量文件成功:" << file.fileName();
        }
    }
}

void FullBlackWorker::setStatus(bool isValid, EM_FullBlackStatus status)
{
    m_isValid = isValid;
    m_curStatus = status;
    GM_INS->m_env->updateFullBlackEnvs(m_isValid, m_curStatus, m_version);
}

bool FullBlackWorker::loadFullBlack(int batchNo, const QString &path)
{
    LOG_INFO().noquote() << "加载全量文件:" << path << "批次:" << batchNo;

    m_dao[1].setDatabaseName(path);

    if (!m_dao[1].open()) {
        LOG_ERROR().noquote() << "全量文件打开失败:" << m_dao[1].lastError().text();
        m_dao[1].close();
        return false;
    }

    // 核验全量文件
    LOG_INFO().noquote() << "开始校核全量文件...";
    QString candidateVersion;
    QString candidateCleanTable;
    if (!validateFullBlack(m_dao[1], batchNo, &candidateVersion, &candidateCleanTable)) {
        LOG_ERROR().noquote() << "全量文件校核失败!";
        m_dao[1].close();
        return false;
    }
    LOG_INFO().noquote() << "全量文件校核成功";

    // 全量核验通过，发布连接
    std::swap(m_dao[0], m_dao[1]);
    if (m_dao[1].isOpen())
        m_dao[1].close();

    m_version = candidateVersion;
    m_activeBatchNo = batchNo;
    m_cleanTable = candidateCleanTable;

    return true;
}

bool FullBlackWorker::validateFullBlack(const QSqlDatabase &db, int batchNo, QString *version, QString *cleanTable)
{
    const QString sql = "SELECT version, cleantable FROM t_operatectrl WHERE paramtype = ? AND batchno = ?";

    Transaction t(db);
    try {
        PreparedQuery query = t.prepare(sql);
        QueryResult res = query.exec(515, batchNo);

        LOG_INFO().noquote() << "执行SQL语句:" << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next()) {
            LOG_ERROR().noquote() << "未查询到全量元数据, 批次" << batchNo;
            return false;
        }

        const QString candidateVersion = res.value("version").toString();
        const QString candidateCleanTable = res.value("cleantable").toString();
        if (candidateVersion.isEmpty() || candidateCleanTable.isEmpty()) {
            LOG_ERROR().noquote() << "全量version或cleantable为空";
            return false;
        }

        *version = candidateVersion;
        *cleanTable = candidateCleanTable;
        return true;
    } catch (const DBException &e) {
        LOG_ERROR().noquote() << e.lastError.text() << "\t" << e.lastQuery;
        return false;
    }
}

bool FullBlackWorker::parseFullManifest(const QByteArray &data, const QUrl &manifestUrl, ST_FullManifest *manifest, QString *error) const
{
    *manifest = ST_FullManifest();
    error->clear();

    bool xmlOk = false;
    QString xmlError;
    const QVariantMap root = DataDealUtils::xmlToMap(data, &xmlOk, &xmlError);
    if (!xmlOk) {
        *error = QString("XML解析失败: %1").arg(xmlError);
        return false;
    }

    bool batchOk = false;
    const QString batchText = root.value("batchno").toString().trimmed();
    const int batchNo = batchText.toInt(&batchOk);
    if (!batchOk || batchNo <= 0) {
        *error = QString("batchno无效: %1").arg(batchText);
        return false;
    }

    // 获取完整zip文件的MD5
    const QVariantMap totalMd5Node = root.value("totalmd5").toMap();
    const QString totalMd5 = totalMd5Node.value("md5").toString().trimmed().toUpper();
    if (totalMd5.isEmpty()) {
        *error = "totalmd5不合法";
        return false;
    }

    QList<ST_FullSliceInfo> slices;
    QVariantList parameterNodes;
    const QVariant parameterValue = root.value("parameter");
    if (parameterValue.type() == QVariant::List) {
        parameterNodes = parameterValue.toList();
    } else if (parameterValue.type() == QVariant::Map) {
        parameterNodes.append(parameterValue);
    }
    const QRegularExpression sliceRegex(QString("^ETCBlackCard_%1\\.zip\\.(\\d{2})$").arg(batchText));
    for (const QVariant &node : parameterNodes) {
        const QVariantMap one = node.toMap();
        const QString fileName = one.value("zipfile").toString().trimmed();
        const QString md5 = one.value("md5").toString().trimmed().toUpper();

        const QRegularExpressionMatch match = sliceRegex.match(fileName);
        if (!match.hasMatch()) {
            *error = QString("全量切片文件名无效: %1").arg(fileName);
            return false;
        }

        bool indexOk = false;
        const int index = match.captured(1).toInt(&indexOk);
        if (!indexOk || index <= 0) {
            *error = QString("全量切片序号无效: %1").arg(fileName);
            return false;
        }
        ST_FullSliceInfo slice;
        slice.index = index;
        slice.fileName = fileName;
        slice.md5 = md5;
        if (!manifestUrl.isEmpty())
            slice.url = manifestUrl.resolved(QUrl(fileName));
        slices.append(slice);
    }
    if (slices.isEmpty()) {
        *error = "全量切片文件为空";
        return false;
    }

    std::sort(slices.begin(), slices.end(), [](const ST_FullSliceInfo &left, const ST_FullSliceInfo &right) { return left.index < right.index; });
    for (int i = 0; i < slices.size(); ++i) {
        const int expectedIndex = i + 1;
        if (slices.at(i).index != expectedIndex) {
            *error = QString("全量切片序号不连续，期望%1，实际%2").arg(expectedIndex, 2, 10, QChar('0')).arg(slices.at(i).index, 2, 10, QChar('0'));
            return false;
        }
    }

    manifest->batchNo = batchNo;
    manifest->totalMd5 = totalMd5;
    manifest->slices = slices;
    manifest->rawXml = data;
    return true;
}

bool FullBlackWorker::readLocalManifest(ST_FullManifest *manifest, QString *error) const
{
    const ST_ConfigSnap snap = GM_INS->m_conf->getConfigSnap();

    const FileName localFile = FileName::fromString(snap.fullBlackPath + "/BlackUpdate.xml");
    if (!localFile.exists()) {
        *error = "本地BlackUpdate.xml不存在";
        return false;
    }

    FileReader reader;
    if (!reader.fetch(localFile.toString())) {
        *error = QString("读取本地BlackUpdate.xml失败: %1").arg(reader.errorString());
        return false;
    }

    return parseFullManifest(reader.data(), QUrl(), manifest, error);
}

bool FullBlackWorker::fetchRemoteManifest(ST_FullManifest *manifest, QString *error) const
{
    const ST_ConfigSnap snap = GM_INS->m_conf->getConfigSnap();
    QString baseUrl = snap.stationServiceURL.trimmed();
    while (baseUrl.endsWith('/'))
        baseUrl.chop(1);
    const QUrl manifestUrl(baseUrl + "/BlackUpdate/BlackUpdate.xml");
    if (!manifestUrl.isValid() || manifestUrl.isEmpty()) {
        *error = QString("远程BlackUpdate.xml地址无效: %1").arg(manifestUrl.toString());
        return false;
    }

    QByteArray data;
    if (!m_http->getSync(data, manifestUrl)) {
        *error = QString("远程BlackUpdate.xml获取失败: %1").arg(QString::fromUtf8(data));
        return false;
    }
    if (data.isEmpty()) {
        *error = "远程BlackUpdate.xml内容为空";
        return false;
    }

    return parseFullManifest(data, manifestUrl, manifest, error);
}

int FullBlackWorker::getLocalBatchNo()
{
    ST_FullManifest manifest;
    QString error;
    if (!readLocalManifest(&manifest, &error)) {
        LOG_WARNING().noquote() << error;
        return 0;
    }

    return manifest.batchNo;
}

int FullBlackWorker::getRemoteBatchNo()
{
    ST_FullManifest manifest;
    QString error;
    if (!fetchRemoteManifest(&manifest, &error)) {
        LOG_WARNING().noquote() << error;
        return 0;
    }

    return manifest.batchNo;
}
