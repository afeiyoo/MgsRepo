#include "fullblackworker.h"

#include <QMetaObject>
#include <QRegularExpression>
#include <QSqlError>

#include "EasyQtSql.h"
#include "Logger.h"
#include "config/config.h"
#include "core/globalmanager.h"
#include "dbs/dataservice.h"
#include "env/defines.h"
#include "env/environment.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"

using namespace EasyQtSql;
using namespace Utils;

FullBlackWorker::FullBlackWorker(QObject *parent)
    : QObject{parent}
{
    m_timer = new QTimer(this);
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
}

void FullBlackWorker::onCheckFullBlack()
{
    ST_ConfigSnap snap = GM_INS->m_conf->getConfigSnap();

    LOG_INFO().noquote() << "开始检查全量...";
    auto result = getMaxBatchNoFromFiles(snap.fullBlackPath);
    if (!result) {
        if (m_isFirst) {
            LOG_ERROR().noquote() << "程序启动，未找到全量文件 => 全量异常";
            setStatus(false, -1);
            m_isFirst = false;
            return;
        }

        if (m_isValid) {
            LOG_INFO().noquote() << "程序运行中，未找到全量文件。上次检查全量正常 => 全量正常";
        } else {
            LOG_ERROR().noquote() << "程序运行中，未找到全量文件。上次检查全量异常 => 全量异常";
        }
        setStatus(m_isValid, -2);
        return;
    }

    const int fileBatchNo = result.value();
    const int curBatchNo = snap.fullBatchNo.toInt();

    const QString filePath = snap.fullBlackPath + QString("/ETCBlackCard_%1.db").arg(fileBatchNo);

    // 全量批次检查
    if (fileBatchNo < curBatchNo) {
        if (m_isFirst) {
            LOG_ERROR().noquote() << "程序启动，未找到当前批次全量文件: 当前批次" << curBatchNo << "文件最大批次" << fileBatchNo << "=> 全量异常";
            setStatus(false, -3);
            m_isFirst = false;
            return;
        }

        if (m_isValid) {
            LOG_ERROR().noquote() << "程序运行中，未找到当前批次全量文件: 当前批次" << curBatchNo << "文件最大批次" << fileBatchNo
                                  << "上次检查全量正常 => 全量正常";
        } else {
            LOG_ERROR().noquote() << "程序运行中，未找到当前批次全量文件: 当前批次" << curBatchNo << "文件最大批次" << fileBatchNo
                                  << "上次检查全量异常 => 全量异常";
        }
        setStatus(m_isValid, -4);
        return;
    }

    if (!m_isFirst && fileBatchNo == curBatchNo) {
        if (m_isValid) {
            LOG_INFO().noquote() << "未发现新批次全量文件: 当前批次" << curBatchNo << "全量文件最大批次" << fileBatchNo
                                 << "上次检查全量正常 => 全量正常";
            return;
        }
        LOG_INFO().noquote() << "上次全量加载失败，尝试重新加载全量文件，批次:" << fileBatchNo;
    }

    // 加载全量
    if (!loadFullBlack(fileBatchNo, filePath)) {
        if (m_isFirst) {
            LOG_ERROR().noquote() << "程序启动，全量加载失败: 批次" << fileBatchNo << "=> 全量异常";
            setStatus(false, -5);
            m_isFirst = false;
            return;
        }

        if (m_isValid) {
            LOG_ERROR().noquote() << "程序运行中，全量加载失败: 批次" << fileBatchNo << "上次检查全量正常 => 全量正常";
        } else {
            LOG_ERROR().noquote() << "程序运行中，全量加载失败: 批次" << fileBatchNo << "上次检查全量异常 => 全量异常";
        }
        setStatus(m_isValid, -6);
        return;
    }

    m_isFirst = false;
    LOG_INFO().noquote() << "全量加载成功: 批次" << fileBatchNo;
    setStatus(true, 0);
    pruneOldFiles(fileBatchNo);
}

void FullBlackWorker::onInit()
{
    // 全量数据库连接初始化
    for (int i = 0; i < 2; ++i) {
        const QString connName = QString("fb_%1").arg(i, 2, 10, QChar('0'));
        m_dao[i] = QSqlDatabase::addDatabase("QSQLITE", connName);
    }

    // 每隔10分钟检查一次全量
    m_timer->setInterval(10 * 60 * 1000);
    connect(m_timer, &QTimer::timeout, this, &FullBlackWorker::onCheckFullBlack);

    // 程序加载时，立即进行全量检查
    onCheckFullBlack();

    m_timer->start();
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

void FullBlackWorker::setStatus(bool isValid, int status)
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

    // 清理ETCBlackCard表
    const int curBatchNo = GM_INS->m_conf->getConfigSnap().fullBatchNo.toInt();
    bool cleanOk = false;
    if (batchNo > curBatchNo && !candidateCleanTable.isEmpty()) {
        const bool invoked = QMetaObject::invokeMethod(GM_INS->m_ds, "cleanETCBlackCard", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, cleanOk),
                                                       Q_ARG(QString, candidateCleanTable));
        if (!invoked || !cleanOk) {
            LOG_ERROR().noquote() << "清理表" << candidateCleanTable << "失败";
            m_dao[1].close();
            return false;
        }
    }

    // 全量核验通过，发布连接
    std::swap(m_dao[0], m_dao[1]);
    if (m_dao[1].isOpen())
        m_dao[1].close();

    m_version = candidateVersion;
    m_cleanTable = candidateCleanTable;

    GM_INS->m_conf->setFullBatchNo(batchNo);
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
