#include "globalmanager.h"

#include "ConsoleAppender.h"
#include "Logger.h"
#include "RollingFileAppender.h"
#include "config/config.h"
#include "core/dtpsender.h"
#include "utils/fileutils.h"

Q_GLOBAL_STATIC(GlobalManager, ins);

GlobalManager::GlobalManager(QObject *parent)
    : QObject{parent}
{
    m_config = new Config(this);
    m_dtpSender = new DtpSender(this);
    m_confPath = Utils::FileUtils::curApplicationDirPath() + "/config/config.ini";
}

GlobalManager::~GlobalManager() {}

GlobalManager *GlobalManager::instance()
{
    return ins();
}

void GlobalManager::init()
{
    // 日志初始化
    ConsoleAppender *consoleAppender = new ConsoleAppender;
    consoleAppender->setFormat(m_config->m_logConfig.format);
    cuteLogger->registerAppender(consoleAppender);

    Utils::FileName logPath = Utils::FileName::fromString(Utils::FileUtils::curApplicationDirPath() + "/logs/PadService.log");
    Utils::FileUtils::makeSureDirExist(logPath.parentDir());
    RollingFileAppender *rollingFileAppender = new RollingFileAppender(Utils::FileUtils::canonicalPath(logPath).toString());
    rollingFileAppender->setFormat(m_config->m_logConfig.format);
    rollingFileAppender->setLogFilesLimit(m_config->m_logConfig.filesLimit);
    rollingFileAppender->setFlushOnWrite(true);
    rollingFileAppender->setDatePattern(RollingFileAppender::DatePattern::DailyRollover);
    cuteLogger->registerAppender(rollingFileAppender);

    // 配置加载
    Utils::FileName configPath = Utils::FileName::fromString(m_confPath);
    m_config->loadConfig(configPath);

    // 系统环境初始化
    m_pictureDir = Utils::FileName::fromString(Utils::FileUtils::curApplicationDirPath() + "/pictures");
    Utils::FileUtils::makeSureDirExist(m_pictureDir);
    QString error;
    Utils::FileUtils::autoDeleteFiles(m_pictureDir.toString(), ".jpg", 30 * 24, &error);
    if (!error.isEmpty())
        LOG_INFO().noquote() << error;

#if QT_VERSION <= QT_VERSION_CHECK(5, 10, 0)
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime())); // 随机数种子初始化
#endif

    // 数据库连接初始化
    QString dbType = m_config->m_dbConfig.type;
    EasyQtSql::SqlFactory::DBSetting setting;
    if (dbType == "QODBC") {
        QString connectionString = QString("Driver={%1};DBQ=%2;UID=%3;PWD=%4")
                                       .arg(m_config->m_dbConfig.driver, m_config->m_dbConfig.dbName, m_config->m_dbConfig.user,
                                            m_config->m_dbConfig.password);
        setting = EasyQtSql::SqlFactory::DBSetting(dbType, connectionString);
    } else {
        setting = EasyQtSql::SqlFactory::DBSetting(dbType, m_config->m_dbConfig.host, m_config->m_dbConfig.port, m_config->m_dbConfig.user,
                                                   m_config->m_dbConfig.password, m_config->m_dbConfig.dbName);
    }
    m_dbFactory = EasyQtSql::SqlFactory::getInstance()->config(setting, "oracle");

    // Dtp发送对象初始化
    m_dtpSender->initDtp("./libDtp-Client.so");

    // 云坐席台账接口URI初始化
    m_remoteURIs.insert(11, "/adminlogin/login");
    m_remoteURIs.insert(21, "/rmtLeaveAndReturn/pagn");
    m_remoteURIs.insert(22, "/rmtLeaveAndReturn/save");
    m_remoteURIs.insert(23, "/rmtLeaveAndReturn/getById/%1");
    m_remoteURIs.insert(31, "/rmtLeader/pagn");
    m_remoteURIs.insert(32, "/rmtLeader/save");
    m_remoteURIs.insert(33, "/rmtLeader/getById/%1");
    m_remoteURIs.insert(41, "/rmtNightPatrol/pagn");
    m_remoteURIs.insert(42, "/rmtNightPatrol/save");
    m_remoteURIs.insert(43, "/rmtNightPatrol/getById/%1");
    m_remoteURIs.insert(51, "/rmtSpecialVehicle/pagn");
    m_remoteURIs.insert(52, "/rmtSpecialVehicle/save");
    m_remoteURIs.insert(53, "/rmtSpecialVehicle/getById/%1");
    m_remoteURIs.insert(54, "/rmtSpecialVehicle/enPassRecord");
    m_remoteURIs.insert(55, "/rmtSpecialVehicle/pictureSave");
    m_remoteURIs.insert(56, "/rmtSpecialVehicle/pictureDownload");
    m_remoteURIs.insert(61, "/rmtOverWeight/pagn");
    m_remoteURIs.insert(62, "/rmtOverWeight/save");
    m_remoteURIs.insert(63, "/rmtOverWeight/getById/%1");
    m_remoteURIs.insert(64, "/rmtOverWeight/overWeightRecord");
    m_remoteURIs.insert(71, "/rmtIllegalEntry/pagn");
    m_remoteURIs.insert(72, "/rmtIllegalEntry/save");
    m_remoteURIs.insert(73, "/rmtIllegalEntry/getById/%1");
    m_remoteURIs.insert(81, "/rmtUnmannedLane/pagn");
    m_remoteURIs.insert(82, "/rmtUnmannedLane/save");
    m_remoteURIs.insert(83, "/rmtUnmannedLane/getById/%1");
    m_remoteURIs.insert(91, "/rmtSystemUser/getAll");
    m_remoteURIs.insert(101, "/rmtShiftRecord/pagn");
    m_remoteURIs.insert(102, "/rmtShiftRecord/save");
    m_remoteURIs.insert(103, "/rmtShiftRecord/getById/%1");
}
