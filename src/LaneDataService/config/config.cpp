#include "config.h"

#include "env/defines.h"
#include "utils/configutils.h"
#include "utils/fileutils.h"
#include "utils/stdafx.h"

using namespace Utils;

Config::Config(QObject *parent)
    : QObject{parent}
{
    m_confUtil = new ConfigUtils();

    m_logFormat = "%{time} [%{type}] [%{threadid}] %{message}\n\n";
}

Config::~Config()
{
    SAFE_DELETE(m_confUtil);
}

void Config::loadConfig(const Utils::FileName &path)
{
    m_confUtil->init(FileUtils::canonicalPath(path).toString(), ConfigUtils::INI);

    // 日志配置
    m_logFormat = m_confUtil->getValue("Log/format", "%{time} [%{type}] [%{threadid}] %{message}\n\n").toString();
    m_logLimits = m_confUtil->getValue("Log/limit", 180).toUInt();

    // 数据库配置
    m_dbType = m_confUtil->getValue("DataBase/dbType", 1).toUInt();
    m_dbHost = m_confUtil->getValue("DataBase/dbHost", "127.0.0.1").toString();
    m_dbPort = m_confUtil->getValue("DataBase/dbPort", 3306).toUInt();
    m_dbName = m_confUtil->getValue("DataBase/dbName", "tolllanedb").toString();
    m_dbUser = m_confUtil->getValue("DataBase/dbUser", "tlman").toString();
    m_dbPassword = m_confUtil->getValue("DataBase/dbPassword", "ds18fjeit").toString();

    // sql文件路径
    if (m_dbType == 1) {
        m_sqlFiles = QStringList{":/static/sqls/common_mysql.xml", ":/static/sqls/mtcin_mysql.xml", ":/static/sqls/mtcout_mysql.xml",
                                 ":/static/sqls/etc_mysql.xml"};
    } else {
        m_sqlFiles = QStringList{":/static/sqls/common_dameng.xml", ":/static/sqls/etc_dameng.xml", ":/static/sqls/mtcin_dameng.xml",
                                 ":/static/sqls/mtcout_dameng.xml"};
    }

    // URL配置
    m_stationServiceURL = m_confUtil->getValue("URL/stationService", "").toString();

    // 状态名单配置
#ifdef Q_OS_WIN32
    m_fullBlackPath = m_confUtil->getValue("BlackList/fullBlackPath", "D://fjeit//DtpAgent32//BlackList").toString();
    m_deltaBlackPath = m_confUtil->getValue("BlackList/deltaBlackPath", "").toString();
    if (m_deltaBlackPath.isEmpty()) {
        m_deltaBlackPath = FileUtils::curApplicationDirPath() + "/deltaBlack";
    }
#else
    // TODO
#endif
    m_fullBatchNo = m_confUtil->getValue("BlackList/fullBatchNo", "").toString();
    m_deltaBatchNo = m_confUtil->getValue("BlackList/deltaBatchNo", "").toString();
}

ST_ConfigSnap Config::getConfigSnap() const
{
    QReadLocker locker(&m_lock);

    ST_ConfigSnap snap;
    snap.dbType = m_dbType;
    snap.dbHost = m_dbHost;
    snap.dbName = m_dbName;
    snap.dbUser = m_dbUser;
    snap.dbPassword = m_dbPassword;
    snap.dbPort = m_dbPort;

    snap.logFormat = m_logFormat;
    snap.logLimits = m_logLimits;

    snap.sqlFiles = m_sqlFiles;

    snap.fullBlackPath = m_fullBlackPath;
    snap.fullBatchNo = m_fullBatchNo;
    snap.deltaBlackPath = m_deltaBlackPath;
    snap.deltaBatchNo = m_deltaBatchNo;

    snap.stationServiceURL = m_stationServiceURL;

    return snap;
}

void Config::setFullBatchNo(QString batchNo)
{
    QWriteLocker locker(&m_lock);
    m_confUtil->setValue("BlackList/fullBatchNo", batchNo);
    m_fullBatchNo = batchNo;
}

void Config::setDeltaBatchNo(QString batchNo)
{
    QWriteLocker locker(&m_lock);
    m_confUtil->setValue("BlackList/deltaBatchNo", batchNo);
    m_deltaBatchNo = batchNo;
}
