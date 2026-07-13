#include "fullblackchecker.h"

#include <QRegularExpression>
#include <QThread>

#include "Logger.h"
#include "config/config.h"
#include "core/globalmanager.h"
#include "core/signalmanager.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"
#include "utils/stdafx.h"

using namespace Utils;

FullBlackChecker::FullBlackChecker(QObject *parent)
    : QObject{parent}
{
    m_td = new QThread();
    this->moveToThread(m_td);
    m_td->start();
}

FullBlackChecker::~FullBlackChecker()
{
    if (m_td->isRunning()) {
        m_td->quit();
        if (!m_td->wait(1000)) {
            m_td->terminate();
            m_td->wait(1000);
        }
    }

    SAFE_DELETE(m_td);
}

void FullBlackChecker::init()
{
    connect(GM_INS->m_sigMan, &SignalManager::sigCheckFullBlack, this, &FullBlackChecker::onCheckFullBlack);
}

void FullBlackChecker::onCheckFullBlack()
{
    LOG_CINFO("cron").noquote() << "开始检查是否有新版本全量文件下达:" << DataDealUtils::curDateTimeStr();
    auto res = getMaxBatchNoFromFiles();
    if (!res)
        return;

    int batchNo = res.value();
    if (batchNo <= GM_INS->m_conf->m_fullBatchNo) {
        LOG_CINFO("cron").noquote() << "未发现新版本全量文件: 当前版本" << GM_INS->m_conf->m_fullBatchNo;
        return;
    }

    LOG_CINFO("cron").noquote() << "发现新版本全量文件: 老版本" << GM_INS->m_conf->m_fullBatchNo << "新版本" << batchNo;
    // 加载新全量
    QString fullBlackPath = GM_INS->m_conf->m_fullBlackPath + QString("/ETCBlackCard_%1.db").arg(batchNo);
    if (!loadNewFile(fullBlackPath)) {
        LOG_CERROR("cron").noquote() << "新版本全量文件加载失败:" << DataDealUtils::curDateTimeStr();
        return;
    }
    LOG_CINFO("cron").noquote() << "新版本全量文件加载成功:" << DataDealUtils::curDateTimeStr();
}

Utils::optional<int> FullBlackChecker::getMaxBatchNoFromFiles()
{
    FileName dirPath = FileName::fromString(GM_INS->m_conf->m_fullBlackPath);
    FileNameList fileList = FileUtils::getFilesWithSuffix(dirPath, ".db");

    if (fileList.isEmpty()) {
        LOG_CINFO("cron").noquote() << "在" << dirPath.toString() << "下，未找到全量文件";
        return nullopt;
    }

    LOG_CINFO("cron").noquote() << "在" << dirPath.toString() << "下，找到" << fileList.size() << "个全量文件";
    const QRegularExpression fileNameRegex("^ETCBlackCard_(\\d+)\\.db$");
    int maxBatchNo = 0;
    for (const auto &file : fileList) {
        QString fileName = file.fileName();
        QRegularExpressionMatch match = fileNameRegex.match(fileName);

        if (!match.hasMatch())
            continue;

        bool ok = false;
        int batchNo = match.captured(1).toInt(&ok);
        if (!ok || batchNo <= 0) {
            LOG_CINFO("cron").noquote() << "忽略格式不正确的数据库文件:" << fileName;
            continue;
        }

        if (batchNo > maxBatchNo)
            maxBatchNo = batchNo;
    }

    return maxBatchNo;
}

bool FullBlackChecker::loadNewFile(const QString &filePath)
{
    FileName newFile = FileName::fromString(filePath);
    if (!newFile.exists()) {
        LOG_CERROR("cron").noquote() << "全量文件" << newFile.fileName() << "不存在";
        return false;
    }

    return true;
}

void FullBlackChecker::pruneFiles(const QString &excludeFile)
{
    FileName dirPath = FileName::fromString(GM_INS->m_conf->m_fullBlackPath);
    FileNameList fileList = FileUtils::getFilesWithSuffix(dirPath, ".db");
    if (fileList.isEmpty() || excludeFile.isEmpty())
        return;

    int cnt = 0;
    for (const auto &file : fileList) {
        if (file.fileName() == excludeFile)
            continue;

        QString errDesc;
        if (!FileUtils::removeRecursively(file, &errDesc)) {
            LOG_CERROR("cron").noquote() << "文件" << file.fileName() << "删除失败:" << errDesc;
        } else {
            ++cnt;
        }
    }

    LOG_CINFO("cron").noquote() << "成功清理" << cnt << "个过期全量文件";
}
