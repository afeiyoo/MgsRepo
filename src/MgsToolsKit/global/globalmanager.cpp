#include "globalmanager.h"

#include "ConsoleAppender.h"
#include "Logger.h"
#include "RollingFileAppender.h"
#include "global/constant.h"
#include "global/signalmanager.h"
#include "utils/fileutils.h"

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
    consoleAppender->setFormat(Constant::Log::FORMAT);
    cuteLogger->registerAppender(consoleAppender);
    // 轮转日志 主程序日志
    FileName mainLogPath = logDir + QString("/main.log");
    RollingFileAppender *mainAppender = new RollingFileAppender(FileUtils::canonicalPath(mainLogPath).toString());
    // 轮转日志 情报板日志
    FileName infoboardLogPath = logDir + QString("/infoboard.log");
    RollingFileAppender *infoboardAppender = new RollingFileAppender(FileUtils::canonicalPath(infoboardLogPath).toString());
    // 轮转日志 智能车控器日志
    FileName smartControllerLogPath = logDir + QString("/smartController.log");
    RollingFileAppender *smartControllerAppender = new RollingFileAppender(FileUtils::canonicalPath(smartControllerLogPath).toString());
    // 轮转日志 发卡机日志
    FileName cardRobotLogPath = logDir + QString("/cardRobot.log");
    RollingFileAppender *cardRobotAppender = new RollingFileAppender(FileUtils::canonicalPath(cardRobotLogPath).toString());

    QList<RollingFileAppender *> appenders = {mainAppender, infoboardAppender, smartControllerAppender, cardRobotAppender};
    for (auto appender : appenders) {
        appender->setFormat(Constant::Log::FORMAT);
        appender->setLogFilesLimit(90);
        appender->setFlushOnWrite(true);
        appender->setDatePattern(RollingFileAppender::DatePattern::DailyRollover);
    }
    cuteLogger->registerAppender(mainAppender);
    cuteLogger->registerCategoryAppender("infoboard", infoboardAppender);
    cuteLogger->registerCategoryAppender("smartctrl", smartControllerAppender);
    cuteLogger->registerCategoryAppender("cardrobot", cardRobotAppender);

    LOG_INFO().noquote() << "开始程序初始化...";
}
