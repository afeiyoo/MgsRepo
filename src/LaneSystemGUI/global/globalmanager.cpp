#include "globalmanager.h"

#include "ConsoleAppender.h"
#include "RollingFileAppender.h"
#include "global/signalmanager.h"
#include "global/uiconst.h"
#include "utils/fileutils.h"

#include <QFont>

using namespace Utils;

Q_GLOBAL_STATIC(GlobalManager, ins)

GlobalManager::GlobalManager(QObject *parent)
    : QObject{parent}
{
    m_signalMan = new SignalManager(this);
}

GlobalManager::~GlobalManager() {}

GlobalManager *GlobalManager::instance()
{
    return ins();
}

void GlobalManager::init()
{
    // 日志初始化
    FileName logDir = FileName::fromString(FileUtils::curApplicationDirPath() + "/logs");
    FileUtils::makeSureDirExist(logDir);
    // 控制台日志
    ConsoleAppender *consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat(Log::FORMAT);
    cuteLogger->registerAppender(consoleAppender);
    // 轮转文件日志
    FileName sysLogPath = logDir + QString("/front.log");
    RollingFileAppender *sysFileAppender = new RollingFileAppender(FileUtils::canonicalPath(sysLogPath).toString());
    sysFileAppender->setLogFilesLimit(Log::MAX_SAVE_DAY);
    sysFileAppender->setFormat(Log::FORMAT);
    sysFileAppender->setFlushOnWrite(true);
    sysFileAppender->setDatePattern(RollingFileAppender::DatePattern::DailyRollover);
    cuteLogger->registerAppender(sysFileAppender);

    LOG_INFO().noquote() << "开始进行系统初始化......";
}
