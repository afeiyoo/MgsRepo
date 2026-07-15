#include "fullblackworker.h"

#include <QRegularExpression>
#include <QThread>

#include "Logger.h"
#include "config/config.h"
#include "core/globalmanager.h"
#include "core/signalmanager.h"
#include "utils/fileutils.h"

using namespace Utils;

FullBlackWorker::FullBlackWorker(QObject *parent)
    : QObject{parent}
{
    connect(GM_INS->m_sigMan, &SignalManager::sigCheckFullBlack, this, &FullBlackWorker::onCheckFullBlack);
    connect(GM_INS->m_sigMan, &SignalManager::sigLoadFullBlackRes, this, &FullBlackWorker::onLoadFullBlackRes);
}

FullBlackWorker::~FullBlackWorker() {}

void FullBlackWorker::onCheckFullBlack(bool isFirst, bool curFullBlackOk)
{
    if (m_loading) {
        LOG_INFO().noquote() << "全量文件正在加载，跳过本次新批次检查，待加载批次:" << m_pendingBatchNo;
        return;
    }

    LOG_INFO().noquote() << "开始检查并加载全量文件...";
    auto result = getMaxBatchNoFromFiles(GM_INS->m_conf->m_fullBlackPath);
    if (!result) {
        LOG_WARNING().noquote() << "在" << GM_INS->m_conf->m_fullBlackPath << "下，未找到全量文件";
        emit GM_INS->m_sigMan->sigUpdateFullBlackStatus(false, "全量文件未找到!");
        return;
    }

    const int batchNo = result.value();
    const QString filePath = GM_INS->m_conf->m_fullBlackPath + QString("/ETCBlackCard_%1.db").arg(batchNo);

    // 批次检查
    if (batchNo < GM_INS->m_conf->m_fullBatchNo) {
        LOG_WARNING().noquote() << "全量文件批次低于当前批次，拒绝回退: 当前批次" << GM_INS->m_conf->m_fullBatchNo << "全量文件最大批次" << batchNo;
        emit GM_INS->m_sigMan->sigUpdateFullBlackStatus(false, "非最新批次全量文件!");
        return;
    }

    if (!isFirst && batchNo == GM_INS->m_conf->m_fullBatchNo) {
        if (!curFullBlackOk) {
            LOG_INFO().noquote() << "尝试重新加载全量文件:" << batchNo;
            m_loading = true;
            m_pendingBatchNo = batchNo;
            emit GM_INS->m_sigMan->sigLoadFullBlack(filePath, batchNo);
        } else {
            LOG_WARNING().noquote() << "未发现新批次全量文件: 当前批次" << GM_INS->m_conf->m_fullBatchNo << "全量文件最大批次" << batchNo;
        }
        return;
    }

    m_loading = true;
    m_pendingBatchNo = batchNo;
    emit GM_INS->m_sigMan->sigLoadFullBlack(filePath, batchNo);
}

void FullBlackWorker::onLoadFullBlackRes(bool ok, int batchNo, const QString &version)
{
    if (!m_loading || batchNo != m_pendingBatchNo) {
        LOG_ERROR().noquote() << "忽略不匹配的全量加载结果，返回批次:" << batchNo << "待加载批次:" << m_pendingBatchNo;
        return;
    }

    m_loading = false;
    m_pendingBatchNo = 0;

    if (!ok) {
        LOG_ERROR().noquote() << QString("%1批次全量文件加载失败").arg(batchNo);
        emit GM_INS->m_sigMan->sigUpdateFullBlackStatus(false, QString("%1批次全量文件加载失败").arg(batchNo));
        return;
    }

    // 发布全量新版本
    LOG_INFO().noquote() << QString("%1批次全量文件加载成功").arg(batchNo);
    emit GM_INS->m_sigMan->sigUpdateFullBlackStatus(true, QString("%1批次全量文件加载成功").arg(batchNo));
    emit GM_INS->m_sigMan->sigUpdateFullBlackVersion(version);

    // 全量加载成功后，删除旧版本全量文件
    pruneOldFiles(batchNo);
}

Utils::optional<int> FullBlackWorker::getMaxBatchNoFromFiles(const QString &fullBlackPath) const
{
    const FileName dirPath = FileName::fromString(fullBlackPath);
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
    const FileName dirPath = FileName::fromString(GM_INS->m_conf->m_fullBlackPath);
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
