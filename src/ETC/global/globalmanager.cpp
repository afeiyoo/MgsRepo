#include "globalmanager.h"

#include "ConsoleAppender.h"
#include "RollingFileAppender.h"
#include "dao/configs/config.h"
#include "dao/configs/configini.h"
#include "dao/dbs/dataservice.h"
#include "dao/dbs/dataservicedameng.h"
#include "dao/dbs/dataservicemysql.h"
#include "utils/fileutils.h"

using namespace Utils;

Q_GLOBAL_STATIC(GlobalManager, ins)

GlobalManager::GlobalManager(QObject *parent)
    : QObject{parent}
{
    m_conf = new ConfigIni(this);
}

GlobalManager::~GlobalManager() {}

GlobalManager *GlobalManager::instance()
{
    return ins();
}

int GlobalManager::init(int argc, char *argv[])
{
    // 日志初始化
    ConsoleAppender *consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat(m_conf->m_logFormat);
    cuteLogger->registerAppender(consoleAppender);

    FileName bizLogPath = FileName::fromString(FileUtils::curApplicationDirPath() + "/logs/ETC.log");
    FileUtils::makeSureDirExist(bizLogPath.parentDir());
    RollingFileAppender *bizAppender = new RollingFileAppender(FileUtils::canonicalPath(bizLogPath).toString());
    bizAppender->setFormat(m_conf->m_logFormat);
    bizAppender->setLogFilesLimit(m_conf->m_logMaxSaveDays);
    bizAppender->setFlushOnWrite(true);
    bizAppender->setDatePattern(RollingFileAppender::DatePattern::DailyRollover);
    cuteLogger->registerAppender(bizAppender);

    FileName dbLogPath = FileName::fromString(FileUtils::curApplicationDirPath() + "/logs/db.log");
    FileUtils::makeSureDirExist(dbLogPath.parentDir());
    RollingFileAppender *dbAppender = new RollingFileAppender(FileUtils::canonicalPath(dbLogPath).toString());
    dbAppender->setFormat(m_conf->m_logFormat);
    dbAppender->setLogFilesLimit(m_conf->m_logMaxSaveDays);
    dbAppender->setFlushOnWrite(true);
    dbAppender->setDatePattern(RollingFileAppender::DatePattern::DailyRollover);
    cuteLogger->registerCategoryAppender("db", dbAppender);

    // 配置初始化
    LOG_INFO().noquote() << "ETC模块开始加载配置......";
    FileName confPath = FileName::fromString(FileUtils::curApplicationDirPath() + "/config/LaneConf.ini");
    if (!confPath.exists()) {
        LOG_ERROR().noquote() << "ETC模块配置加载失败: 请检查配置文件是否存在";
        return -1;
    }
    m_conf->load(confPath);
    LOG_INFO().noquote() << "ETC模块配置加载成功: " << m_conf->dump();

    // 初始化数据库服务
    if (m_conf->m_dbType == 1) {
        m_ds = new DataServiceMysql(this);
    } else {
        m_ds = new DataServiceDameng(this);
    }

    return 0;
}
