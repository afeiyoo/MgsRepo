#include "globalmanager.h"

#include "ConsoleAppender.h"
#include "RollingFileAppender.h"
#include "config/config.h"
#include "utils/fileutils.h"

using namespace Utils;

Q_GLOBAL_STATIC(GlobalManager, ins);

GlobalManager::GlobalManager(QObject *parent)
    : QObject{parent}
{
    m_conf = new Config(this);
}

GlobalManager::~GlobalManager() {}

GlobalManager *GlobalManager::instance()
{
    return ins();
}

int GlobalManager::init()
{
    // 配置加载
    FileName confPath = FileName::fromString(FileUtils::curApplicationDirPath() + "/config/StationServiceCfg.ini");
    if (!confPath.exists()) {
        return -1;
    }
    m_conf->loadConfig(confPath);

    // 日志初始化
    ConsoleAppender *consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat(m_conf->m_logFormat);
    cuteLogger->registerAppender(consoleAppender);

    FileName logPath = FileName::fromString(FileUtils::curApplicationDirPath() + "/logs/StationService.log");
    FileUtils::makeSureDirExist(logPath.parentDir());
    RollingFileAppender *rollingFileAppender = new RollingFileAppender(FileUtils::canonicalPath(logPath).toString());
    rollingFileAppender->setFormat(m_conf->m_logFormat);
    rollingFileAppender->setLogFilesLimit(m_conf->m_logLimits);
    rollingFileAppender->setFlushOnWrite(true);
    rollingFileAppender->setDatePattern(RollingFileAppender::DailyRollover);
    cuteLogger->registerAppender(rollingFileAppender);

    // 数据库连接初始化

    return 0;
}
