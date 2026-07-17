#include "globalmanager.h"

#include <QThread>
#include <QTimer>

#include "CuteLogger/include/ConsoleAppender.h"
#include "CuteLogger/include/RollingFileAppender.h"
#include "Logger.h"
#include "bend/fullblackmaster.h"
#include "config/config.h"
#include "dbs/dataservice.h"
#include "dbs/sqldealer.h"
#include "env/environment.h"
#include "signalmanager.h"
#include "utils/fileutils.h"
#include "utils/stdafx.h"

using namespace Utils;

Q_GLOBAL_STATIC(GlobalManager, ins);

GlobalManager::GlobalManager(QObject *parent)
    : QObject{parent}
{
    m_conf = new Config(this);
    m_confPath = FileUtils::curApplicationDirPath() + "/config/config.ini";
    m_ds = new DataService(this);
    m_sigMan = new SignalManager(this);
    m_fbMaster = new FullBlackMaster(this);

    m_sqlDealer = new SqlDealer();
    m_env = new Environment(this);
}

GlobalManager::~GlobalManager()
{
    SAFE_DELETE(m_sqlDealer);
}

GlobalManager *GlobalManager::instance()
{
    return ins();
}

int GlobalManager::init()
{
    // 加载配置
    FileName confFile = FileName::fromString(m_confPath);
    if (!confFile.exists())
        return -100;

    m_conf->loadConfig(confFile);

    // 日志初始化
    ConsoleAppender *consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat(m_conf->m_logFormat);
    cuteLogger->registerAppender(consoleAppender);

    FileName mainLogPath = FileName::fromString(FileUtils::curApplicationDirPath() + "/logs/LaneDataService.log");
    FileUtils::makeSureDirExist(mainLogPath.parentDir());
    RollingFileAppender *mainRollingFileAppender = new RollingFileAppender(FileUtils::canonicalPath(mainLogPath).toString());
    mainRollingFileAppender->setFormat(m_conf->m_logFormat);
    mainRollingFileAppender->setLogFilesLimit(m_conf->m_logLimits);
    mainRollingFileAppender->setFlushOnWrite(true);
    mainRollingFileAppender->setDatePattern(RollingFileAppender::DailyRollover);
    cuteLogger->registerAppender(mainRollingFileAppender);

    // 加载sql文件
    bool sqlOk = m_sqlDealer->loadSqlFiles();
    if (!sqlOk)
        return -101;

    // 数据库操作对象初始化
    bool dbOk = m_ds->init(m_conf->m_dbType, m_conf->m_dbHost, m_conf->m_dbPort, m_conf->m_dbUser, m_conf->m_dbPassword, m_conf->m_dbName);
    if (!dbOk)
        return -102;

    // 启动全量检查
    m_fbMaster->init();

    return 0;
}
