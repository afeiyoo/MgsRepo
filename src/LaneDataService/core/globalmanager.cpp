#include "globalmanager.h"

#include "CuteLogger/include/ConsoleAppender.h"
#include "CuteLogger/include/RollingFileAppender.h"
#include "Logger.h"
#include "config/config.h"
#include "dbs/dataservice.h"
#include "dbs/sqldealer.h"
#include "iservicehub.h"
#include "utils/fileutils.h"
#include "utils/stdafx.h"

using namespace Utils;

Q_GLOBAL_STATIC(GlobalManager, ins);

GlobalManager::GlobalManager(QObject *parent)
    : QObject{parent}
{
    m_conf = new Config(this);
    m_confPath = FileUtils::curApplicationDirPath() + "/config/config.ini";
    m_ds = new DataService();
}

GlobalManager::~GlobalManager()
{
    SAFE_DELETE(m_ds);
}

GlobalManager *GlobalManager::instance()
{
    return ins();
}

int GlobalManager::init(IServiceHub *hub)
{
    // 加载配置
    FileName confFile = FileName::fromString(m_confPath);
    if (!confFile.exists()) {
        return -100;
    }
    m_conf->loadConfig(confFile);

    // 日志初始化
    ConsoleAppender *consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat(m_conf->m_logFormat);
    cuteLogger->registerAppender(consoleAppender);

    FileName logPath = FileName::fromString(FileUtils::curApplicationDirPath() + "/logs/LaneDataService.log");
    FileUtils::makeSureDirExist(logPath.parentDir());
    RollingFileAppender *rollingFileAppender = new RollingFileAppender(FileUtils::canonicalPath(logPath).toString());
    rollingFileAppender->setFormat(m_conf->m_logFormat);
    rollingFileAppender->setLogFilesLimit(m_conf->m_logLimits);
    rollingFileAppender->setFlushOnWrite(true);
    rollingFileAppender->setDatePattern(RollingFileAppender::DailyRollover);
    cuteLogger->registerAppender(rollingFileAppender);

    // 通信类初始化
    m_hub = hub;

    // 加载sql文件
    bool sqlOk = m_sqlDealer->loadSqlFiles();
    if (!sqlOk)
        return -101;

    // 数据库操作对象初始化
    bool dbOk = m_ds->init(m_conf->m_dbType, m_conf->m_dbHost, m_conf->m_dbPort, m_conf->m_dbUser, m_conf->m_dbPassword, m_conf->m_dbName);
    if (!dbOk) {
        return -102;
    }

    return 0;
}
