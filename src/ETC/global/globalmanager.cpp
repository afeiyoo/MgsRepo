#include "globalmanager.h"

#include "ConsoleAppender.h"
#include "RollingFileAppender.h"
#include "dao/configs/config.h"
#include "dao/configs/configini.h"
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

    FileName logPath = FileName::fromString(FileUtils::curApplicationDirPath() + "/logs/ETC.log");
    FileUtils::makeSureDirExist(logPath.parentDir());
    RollingFileAppender *rollingAppender = new RollingFileAppender(FileUtils::canonicalPath(logPath).toString());
    rollingAppender->setFormat(m_conf->m_logFormat);
    rollingAppender->setLogFilesLimit(m_conf->m_logMaxSaveDays);
    rollingAppender->setFlushOnWrite(true);
    rollingAppender->setDatePattern(RollingFileAppender::DatePattern::DailyRollover);
    cuteLogger->registerAppender(rollingAppender);

    // 配置初始化
    LOG_INFO().noquote() << "ETC模块开始加载配置......";
    FileName confPath = FileName::fromString(FileUtils::curApplicationDirPath() + "/config/LaneConf.ini");
    if (!confPath.exists()) {
        LOG_ERROR().noquote() << "ETC模块配置加载失败: 请检查配置文件是否存在";
        return -1;
    }
    m_conf->load(confPath);
    LOG_INFO().noquote() << "ETC模块配置加载成功: " << m_conf->dump();
    return 0;
}
