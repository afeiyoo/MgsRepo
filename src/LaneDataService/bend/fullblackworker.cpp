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
    connect(GM_INS->m_sigMan, &SignalManager::sigCheckNewFullBlack, this, &FullBlackWorker::onCheckNewFullBlack);
    connect(GM_INS->m_sigMan, &SignalManager::sigCheckCurFullBlack, this, &FullBlackWorker::onCheckCurFullBlack);
    connect(GM_INS->m_sigMan, &SignalManager::sigLoadFullBlackRes, this, &FullBlackWorker::onLoadFullBlackRes);
}

FullBlackWorker::~FullBlackWorker() {}

void FullBlackWorker::onCheckNewFullBlack()
{
    if (m_loading) {
        LOG_INFO().noquote() << "全量文件正在加载，跳过本次新版本检查，待加载版本:" << m_pendingBatchNo;
        return;
    }

    LOG_INFO().noquote() << "开始检查是否有新版本全量文件下达...";
    auto result = getMaxBatchNoFromFiles(GM_INS->m_conf->m_fullBlackPath);
    if (!result) {
        LOG_WARNING().noquote() << "在" << GM_INS->m_conf->m_fullBlackPath << "下未找到全量文件";
        return;
    }

    const int batchNo = result.value();
    if (batchNo <= GM_INS->m_conf->m_fullBatchNo) {
        LOG_WARNING().noquote() << "未发现新版本全量文件: 当前版本" << GM_INS->m_conf->m_fullBatchNo << "全量目录下最大版本" << batchNo;
        return;
    }

    const QString filePath = GM_INS->m_conf->m_fullBlackPath + QString("/ETCBlackCard_%1.db").arg(batchNo);
    m_loading = true;
    m_pendingBatchNo = batchNo;
    emit GM_INS->m_sigMan->sigLoadFullBlack(filePath, batchNo);
}

void FullBlackWorker::onCheckCurFullBlack()
{
    if (m_loading) {
        LOG_INFO().noquote() << "全量文件正在加载，跳过本次当前版本加载，待加载版本:" << m_pendingBatchNo;
        return;
    }

    int curBatchNo = GM_INS->m_conf->m_fullBatchNo;
    LOG_INFO().noquote() << "准备加载当前版本全量文件:" << curBatchNo;
    const FileName filePath = FileName::fromString(GM_INS->m_conf->m_fullBlackPath + QString("/ETCBlackCard_%1.db").arg(curBatchNo));
    if (!filePath.exists()) {
        LOG_ERROR().noquote() << "加载当前版本全量文件失败:" << filePath.fileName() << "不存在";
        onCheckNewFullBlack();
        return;
    }
    LOG_INFO().noquote() << "当前版本全量文件已找到，开始加载...";
    m_loading = true;
    m_pendingBatchNo = curBatchNo;
    emit GM_INS->m_sigMan->sigLoadFullBlack(filePath.toString(), curBatchNo);
}

void FullBlackWorker::onLoadFullBlackRes(bool ok, int batchNo)
{
    if (!m_loading || batchNo != m_pendingBatchNo) {
        LOG_ERROR().noquote() << "忽略不匹配的全量加载结果，返回版本:" << batchNo << "待加载版本:" << m_pendingBatchNo;
        return;
    }

    m_loading = false;
    m_pendingBatchNo = 0;

    if (!ok) {
        LOG_ERROR().noquote() << "全量文件加载失败: 版本号" << batchNo;
        return;
    }

    // 发布全量新版本
    LOG_INFO().noquote() << "全量文件加载成功: 版本号" << batchNo;
    GM_INS->m_conf->setFullBatchNo(batchNo);
    emit GM_INS->m_sigMan->sigUpdateFullBlackStatus(0);

    // TODO 全量加载成功后，删除旧版本全量文件
}

Utils::optional<int> FullBlackWorker::getMaxBatchNoFromFiles(const QString &fullBlackPath) const
{
    const FileName dirPath = FileName::fromString(fullBlackPath);
    const FileNameList fileList = FileUtils::getFilesWithSuffix(dirPath, ".db");

    if (fileList.isEmpty()) {
        LOG_INFO().noquote() << "在" << dirPath.toString() << "下，未找到全量文件";
        return nullopt;
    }

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
