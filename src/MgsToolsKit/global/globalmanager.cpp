#include "globalmanager.h"

#include "ConsoleAppender.h"
#include "Logger.h"
#include "RollingFileAppender.h"
#include "global/constant.h"
#include "global/signalmanager.h"
#include "utils/fileutils.h"

#include <QDir>

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
    QString mainLogPath = QDir(logDir.toString()).filePath("main.log");
    RollingFileAppender *mainAppender = new RollingFileAppender(mainLogPath);
    // 轮转日志 情报板日志
    QString infoboardLogPath = QDir(logDir.toString()).filePath("infoboard.log");
    RollingFileAppender *infoboardAppender = new RollingFileAppender(infoboardLogPath);
    // 轮转日志 智能车控器日志
    QString smartControllerLogPath = QDir(logDir.toString()).filePath("smartController.log");
    RollingFileAppender *smartControllerAppender = new RollingFileAppender(smartControllerLogPath);
    // 轮转日志 发卡机日志
    QString cardRobotLogPath = QDir(logDir.toString()).filePath("cardRobot.log");
    RollingFileAppender *cardRobotAppender = new RollingFileAppender(cardRobotLogPath);
    // 轮转日志 手机+自助交易终端日志
    QString mobileTerminalLogPath = QDir(logDir.toString()).filePath("mobilePlusTerminal.log");
    RollingFileAppender *mobileTerminalAppender = new RollingFileAppender(mobileTerminalLogPath);
    // 轮转日志 车型识别器日志
    QString vehRecognizerLogPath = QDir(logDir.toString()).filePath("vehRecognizer.log");
    RollingFileAppender *vehRecognizerAppender = new RollingFileAppender(vehRecognizerLogPath);

    QList<RollingFileAppender *> appenders = {mainAppender,      infoboardAppender,      smartControllerAppender,
                                              cardRobotAppender, mobileTerminalAppender, vehRecognizerAppender};
    for (auto appender : appenders) {
        appender->setFormat(Constant::Log::FORMAT);
        appender->setLogFilesLimit(90);
        appender->setFlushOnWrite(true);
        appender->setDatePattern(RollingFileAppender::DatePattern::DailyRollover);
    }
    cuteLogger->registerAppender(mainAppender);
    cuteLogger->registerCategoryAppender("infoboard", infoboardAppender);
    cuteLogger->registerCategoryAppender("SmartLaneController", smartControllerAppender);
    cuteLogger->registerCategoryAppender("cardrobot", cardRobotAppender);
    cuteLogger->registerCategoryAppender("MobilePlusTerminal", mobileTerminalAppender);
    cuteLogger->registerCategoryAppender("VehRecognizer", vehRecognizerAppender);

    LOG_INFO().noquote() << "开始程序初始化...";
}
