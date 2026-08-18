#include "globalmanager.h"

#include "ConsoleAppender.h"
#include "RollingFileAppender.h"
#include "config/config.h"
#include "core/readerctrl.h"
#include "utils/fileutils.h"
#include "utils/stdafx.h"

using namespace Utils;

Q_GLOBAL_STATIC(GlobalManager, ins)

GlobalManager::GlobalManager(QObject *parent)
    : QObject{parent}
{
    m_confPath = FileUtils::curApplicationDirPath() + "/config.ini";
    m_config = new Config();
    m_reader = new ReaderCtrl(this);
}

GlobalManager::~GlobalManager()
{
    SAFE_DELETE(m_config);
}

GlobalManager *GlobalManager::instance()
{
    return ins();
}

int GlobalManager::init()
{
    // 日志初始化
    ConsoleAppender *consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat("%{time} [%{type}] [%{threadid}] %{message}\n\n");
    cuteLogger->registerAppender(consoleAppender);

    FileName logPath = FileName::fromString(FileUtils::curApplicationDirPath() + "/logs/CardService.log");
    FileUtils::makeSureDirExist(logPath.parentDir());
    RollingFileAppender *rollingFileAppender = new RollingFileAppender(FileUtils::canonicalPath(logPath).toString());
    rollingFileAppender->setFormat("%{time} [%{type}] [%{threadid}] %{message}\n\n");
    rollingFileAppender->setLogFilesLimit(180);
    rollingFileAppender->setFlushOnWrite(true);
    rollingFileAppender->setDatePattern(RollingFileAppender::DailyRollover);
    cuteLogger->registerAppender(rollingFileAppender);

    LOG_INFO().noquote() << "程序正在初始化...";
    LOG_INFO().noquote() << "读取配置文件:" << m_confPath;

    // 配置加载
    FileName confPath = FileName::fromString(m_confPath);
    if (!confPath.exists()) {
        LOG_ERROR().noquote() << "程序初始化失败: 配置文件不存在!";
        return -100;
    }
    m_config->loadConfig(confPath.toString());

    // 读卡器初始化
    m_reader->initReader();

    return 0;
}
