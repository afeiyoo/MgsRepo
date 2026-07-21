#include "globalmanager.h"

#include "CuteLogger/include/ConsoleAppender.h"
#include "CuteLogger/include/RollingFileAppender.h"
#include "Logger.h"
#include "bend/deltablackmaster.h"
#include "bend/fullblackmaster.h"
#include "config/config.h"
#include "dbs/dataservice.h"
#include "dbs/sqldealer.h"
#include "env/defines.h"
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
    m_dbMaster = new DeltaBlackMaster(this);
    m_env = new Environment(this);
    m_sql = new SqlDealer();
}

GlobalManager::~GlobalManager()
{
    SAFE_DELETE(m_sql);
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
    ST_ConfigSnap snap = m_conf->getConfigSnap();

    // 日志初始化
    ConsoleAppender *consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat(snap.logFormat);
    cuteLogger->registerAppender(consoleAppender);

    FileName mainLogPath = FileName::fromString(FileUtils::curApplicationDirPath() + "/logs/LaneDataService.log");
    FileUtils::makeSureDirExist(mainLogPath.parentDir());
    RollingFileAppender *mainRollingFileAppender = new RollingFileAppender(FileUtils::canonicalPath(mainLogPath).toString());
    mainRollingFileAppender->setFormat(snap.logFormat);
    mainRollingFileAppender->setLogFilesLimit(snap.logLimits);
    mainRollingFileAppender->setFlushOnWrite(true);
    mainRollingFileAppender->setDatePattern(RollingFileAppender::DailyRollover);
    cuteLogger->registerAppender(mainRollingFileAppender);

    // Sql语句加载
    if (!m_sql->loadSqlFiles(snap.sqlFiles))
        return -101;

    // 数据库操作对象初始化
    bool dbOk = m_ds->init(snap.dbType, snap.dbHost, snap.dbPort, snap.dbUser, snap.dbPassword, snap.dbName);
    if (!dbOk)
        return -102;

    // 启动全量检查
    m_fbMaster->init();
    // 启动增量检查
    m_dbMaster->init();

    return 0;
}
