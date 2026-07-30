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

    m_pictureDir = QDir(FileUtils::curApplicationDirPath() + "/pictures");
    m_pictureCleanupTimer = new QTimer(this);

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
        LOG_INFO().noquote() << "定期删除" << m_pictureDir.absolutePath() << "下过期文件失败:" << error;
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

    LOG_INFO().noquote() << "开始进行程序初始化...";

    // 配置加载
    if (!QFileInfo::exists(m_confPath)) {
        LOG_ERROR().noquote() << "程序初始化失败: 配置文件不存在" << m_confPath;
        return -100;
    }
    m_configMan->loadConfig(m_confPath);

    // 系统环境初始化
    FileUtils::makeSureDirExist(FileName::fromString(m_pictureDir.absolutePath()));
    FileUtils::makeSureDirExist(FileName::fromString(GM_INSTANCE->m_configMan->m_baseConfig.cachePath));

    m_pictureCleanupTimer->setInterval(30 * 60 * 1000);
    connect(m_pictureCleanupTimer, &QTimer::timeout, this, &GlobalManager::onCleanExpiredPictures);
    onCleanExpiredPictures();
    m_pictureCleanupTimer->start();

#if QT_VERSION <= QT_VERSION_CHECK(5, 10, 0)
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime())); // 随机数种子初始化
#endif

    // 数据库连接初始化
    QString dbType = m_configMan->m_dbConfig.type;
    bool dbOk = m_ds->init(dbType, m_configMan->m_dbConfig.driver, m_configMan->m_dbConfig.user, m_configMan->m_dbConfig.password,
                           m_configMan->m_dbConfig.dbName);
    if (!dbOk) {
        LOG_ERROR().noquote() << "程序初始化失败: 数据库初始化异常";
        return -101;
    }

    // Dtp发送对象初始化
    bool dtpOk = m_dtpSender->initDtp("./libDtp-Client.so");
    if (!dtpOk) {
        LOG_ERROR().noquote() << "程序初始化失败: DTP初始化失败";
        return -102;
    }

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

    return 0;
}
