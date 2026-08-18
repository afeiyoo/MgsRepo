#include "globalmanager.h"

#include "ConsoleAppender.h"
#include "Logger.h"
#include "RollingFileAppender.h"
#include "config/configmanager.h"
#include "core/dtpsender.h"
#include "dataservice/dataservice.h"
#include "utils/fileutils.h"
#include "utils/stdafx.h"

using namespace Utils;

Q_GLOBAL_STATIC(GlobalManager, ins);

GlobalManager::GlobalManager(QObject *parent)
    : QObject{parent}
{
    m_configMan = new ConfigManager(this);
    m_dtpSender = new DtpSender(this);

    m_confPath = QDir(FileUtils::curApplicationDirPath()).filePath("config/config.ini");

    m_pictureDir.setPath(QDir(FileUtils::curApplicationDirPath()).filePath("pictures"));
    m_cleanupTimer = new QTimer(this);

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

void GlobalManager::onCleanExpiredPictures()
{
    QString error;
    const bool ok = FileUtils::autoDeleteFiles(m_pictureDir.absolutePath(), ".jpg", 30 * 24, &error);
    if (!ok)
        LOG_WARNING().noquote() << "定期删除" << m_pictureDir.absolutePath() << "下过期文件失败:" << error;
}

void GlobalManager::onCleanExpiredCaches()
{
    QString error;
    const bool ok = FileUtils::autoDeleteFiles(m_configMan->m_baseConfig.cachePath, ".cache", 15 * 24, &error);
    if (!ok)
        LOG_WARNING().noquote() << "定期删除" << m_configMan->m_baseConfig.cachePath << "下过期文件失败:" << error;
}

int GlobalManager::init()
{
    // 日志初始化
    ConsoleAppender *consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat(m_configMan->m_logConfig.format);
    cuteLogger->registerAppender(consoleAppender);

    FileName logPath = FileName::fromString(FileUtils::curApplicationDirPath() + "/logs/PadService.log");
    FileUtils::makeSureDirExist(logPath.parentDir());
    RollingFileAppender *rollingFileAppender = new RollingFileAppender(FileUtils::canonicalPath(logPath).toString());
    rollingFileAppender->setFormat(m_configMan->m_logConfig.format);
    rollingFileAppender->setLogFilesLimit(m_configMan->m_logConfig.filesLimit);
    rollingFileAppender->setFlushOnWrite(true);
    rollingFileAppender->setDatePattern(RollingFileAppender::DatePattern::DailyRollover);
    cuteLogger->registerAppender(rollingFileAppender);

    // 配置加载
    if (!QFileInfo::exists(m_confPath)) {
        LOG_ERROR().noquote() << "程序初始化失败: 配置文件不存在" << m_confPath;
        return -100;
    }
    m_configMan->loadConfig(m_confPath);

    // 数据库连接初始化
    QString dbType = m_configMan->m_dbConfig.type;
    bool dbOk = m_ds->init(dbType, m_configMan->m_dbConfig.driver, m_configMan->m_dbConfig.user, m_configMan->m_dbConfig.password,
                           m_configMan->m_dbConfig.dbName);
    if (!dbOk) {
        LOG_ERROR().noquote() << "程序初始化失败: 数据库初始化异常";
        return -101;
    }

    // Dtp发送对象初始化
    const QString dtpLibPath = QDir(FileUtils::curApplicationDirPath()).filePath("libDtp-Client.so");
    bool dtpOk = m_dtpSender->initDtp(dtpLibPath);
    if (!dtpOk) {
        LOG_ERROR().noquote() << "程序初始化失败: DTP初始化失败";
        return -102;
    }

    // 云坐席台账接口URI初始化
    initRemoteURIs();

    // 系统环境初始化
    FileUtils::makeSureDirExist(FileName::fromString(m_pictureDir.absolutePath()));
    FileUtils::makeSureDirExist(FileName::fromString(m_configMan->m_baseConfig.cachePath));

    m_cleanupTimer->setInterval(30 * 60 * 1000);
    connect(m_cleanupTimer, &QTimer::timeout, this, &GlobalManager::onCleanExpiredPictures);
    connect(m_cleanupTimer, &QTimer::timeout, this, &GlobalManager::onCleanExpiredCaches);
    onCleanExpiredPictures();
    onCleanExpiredCaches();
    m_cleanupTimer->start();

#if QT_VERSION <= QT_VERSION_CHECK(5, 10, 0)
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime())); // 随机数种子初始化
#endif

    return 0;
}

void GlobalManager::initRemoteURIs()
{
    m_remoteURIs = {{11, "/adminlogin/login"},
                    {21, "/rmtLeaveAndReturn/pagn"},
                    {22, "/rmtLeaveAndReturn/save"},
                    {23, "/rmtLeaveAndReturn/getById/%1"},
                    {31, "/rmtLeader/pagn"},
                    {32, "/rmtLeader/save"},
                    {33, "/rmtLeader/getById/%1"},
                    {41, "/rmtNightPatrol/pagn"},
                    {42, "/rmtNightPatrol/save"},
                    {43, "/rmtNightPatrol/getById/%1"},
                    {51, "/rmtSpecialVehicle/pagn"},
                    {52, "/rmtSpecialVehicle/save"},
                    {53, "/rmtSpecialVehicle/getById/%1"},
                    {54, "/rmtSpecialVehicle/enPassRecord"},
                    {55, "/rmtSpecialVehicle/pictureSave"},
                    {56, "/rmtSpecialVehicle/pictureDownload"},
                    {61, "/rmtOverWeight/pagn"},
                    {62, "/rmtOverWeight/save"},
                    {63, "/rmtOverWeight/getById/%1"},
                    {64, "/rmtOverWeight/overWeightRecord"},
                    {71, "/rmtIllegalEntry/pagn"},
                    {72, "/rmtIllegalEntry/save"},
                    {73, "/rmtIllegalEntry/getById/%1"},
                    {81, "/rmtUnmannedLane/pagn"},
                    {82, "/rmtUnmannedLane/save"},
                    {83, "/rmtUnmannedLane/getById/%1"},
                    {91, "/rmtSystemUser/getAll"},
                    {101, "/rmtShiftRecord/pagn"},
                    {102, "/rmtShiftRecord/save"},
                    {103, "/rmtShiftRecord/getById/%1"}};
}
