#include "globalmanager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include "ConsoleAppender.h"
#include "RollingFileAppender.h"
#include "bend/bizhandler.h"
#include "dao/configs/config.h"
#include "dao/configs/configini.h"
#include "dao/dbs/dataservicedameng.h"
#include "dao/dbs/dataservicemysql.h"
#include "fend/pagehandler.h"
#include "global/const.h"
#include "middle/gateway.h"
#include "middle/signalctrl.h"
#include "utils/fileutils.h"

using namespace Utils;

Q_GLOBAL_STATIC(GlobalManager, ins)

namespace {
QVariantMap fetchKeyboard(const QString &path)
{
    FileReader reader;
    QByteArray keyboardJson = reader.fetchQrc(path);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(keyboardJson, &error);

    if (error.error != QJsonParseError::NoError || !doc.isObject())
        return QVariantMap();

    QJsonObject root = doc.object();

    return root.toVariantMap();
}
} // namespace

GlobalManager::GlobalManager(QObject *parent)
    : QObject{parent}
{
    m_conf = new ConfigIni(this);
    m_sigCtrl = new SignalCtrl(this);
    m_gate = new GateWay(this);
    m_pageHandler = new PageHandler(this);
    m_bizHandler = new BizHandler(this);
}

GlobalManager::~GlobalManager() {}

GlobalManager *GlobalManager::instance()
{
    return ins();
}

int GlobalManager::init(IEtcPageController *ui)
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

    m_pageHandler->init(ui);

    // 配置初始化
    LOG_INFO().noquote() << "ETC模块开始加载配置......";
    FileName confPath = FileName::fromString(FileUtils::curApplicationDirPath() + "/config/LaneConf.ini");
    if (!confPath.exists()) {
        LOG_ERROR().noquote() << "ETC模块配置加载失败: 请检查配置文件是否存在";
        return -1;
    }
    m_conf->load(confPath);
    LOG_INFO().noquote() << "ETC模块配置加载成功: " << m_conf->dump();

    // 键盘表加载
    LOG_INFO().noquote() << "ETC模块开始加载键盘表......";
    m_keyBoard = fetchKeyboard(Constant::PATH::KEYBOARD_PATH);
    if (m_keyBoard.isEmpty()) {
        LOG_ERROR().noquote() << "键盘表加载失败";
        return -1;
    }
    LOG_INFO().noquote() << "ETC模块键盘表加载成功";

    // 初始化数据库服务
    LOG_INFO().noquote() << "ETC模块开始初始化数据库连接......";
    if (m_conf->m_dbType == 1) {
        m_ds = new DataServiceMysql(this);
    } else {
        m_ds = new DataServiceDameng(this);
    }
    if (!m_ds->init(m_conf->m_dbHost, m_conf->m_dbPort, m_conf->m_dbUser, m_conf->m_dbPassword, m_conf->m_dbName)) {
        LOG_ERROR().noquote() << "ETC模块数据库初始化失败";
        return -1;
    }
    LOG_INFO().noquote() << "ETC模块初始化数据库连接成功";

    return 0;
}
