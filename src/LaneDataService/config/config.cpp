#include "config.h"

#include "utils/configutils.h"
#include "utils/fileutils.h"
#include "utils/stdafx.h"

using namespace Utils;

Config::Config(QObject *parent)
    : QObject{parent}
{
    m_confUtil = new ConfigUtils();

    m_logFormat = "%{time} [%{type}] [%{threadid}] %{message}\n\n";
    m_logLimits = 180;
}

Config::~Config()
{
    SAFE_DELETE(m_confUtil);
}

void Config::loadConfig(const Utils::FileName &path)
{
    QString str = FileUtils::canonicalPath(path).toString();
    m_confUtil->init(str, ConfigUtils::INI);

    // 数据库配置
    m_dbType = m_confUtil->getValue("DataBase/dbType", 1).toUInt();
    m_dbHost = m_confUtil->getValue("DataBase/dbHost", "127.0.0.1").toString();
    m_dbPort = m_confUtil->getValue("DataBase/dbPort", 1433).toUInt();
    m_dbName = m_confUtil->getValue("DataBase/dbName", "tollstationdb").toString();
    m_dbUser = m_confUtil->getValue("DataBase/dbUser", "tsman").toString();
    m_dbPassword = m_confUtil->getValue("DataBase/dbPassword", "ds18fjeit").toString();

    // sql文件路径
    if (m_dbType == 1) {
        m_sqlFiles = QStringList{":/static/sqls/common_mysql.xml", ":/static/sqls/mtcin_mysql.xml", ":/static/sqls/mtcout_mysql.xml",
                                 ":/static/sqls/etc_mysql.xml"};
    } else {
        m_sqlFiles = QStringList{":/static/sqls/common_dameng.xml", ":/static/sqls/etc_dameng.xml", ":/static/sqls/mtcin_dameng.xml",
                                 ":/static/sqls/mtcout_dameng.xml"};
    }

    // 日志配置
    m_logFormat = m_confUtil->getValue("Log/format", "%{time} [%{type}] [%{threadid}] %{message}\n\n").toString();
    m_logLimits = m_confUtil->getValue("Log/limit", 180).toUInt();

    // URL配置
    m_stationServiceURL = m_confUtil->getValue("URL/stationService", "").toString();

    // 全量状态配置
#ifdef Q_OS_WIN32
    m_fullBlackPath = m_confUtil->getValue("FullBlack/blackListPath", "D://fjeit//DtpAgent32//BlackList").toString();
#else
    // TODO
#endif
    int batchNo = m_confUtil->getValue("FullBlack/batchNo", 0).toInt();
    {
        QWriteLocker locker(&m_lock);
        m_fullBatchNo = batchNo;
    }
}

void Config::setFullBatchNo(int batchNo)
{
    QWriteLocker locker(&m_lock);
    m_confUtil->setValue("FullBlack/batchNo", batchNo);
    m_fullBatchNo = batchNo;
}

int Config::fullBatchNo() const
{
    QReadLocker locker(&m_lock);
    return m_fullBatchNo;
}
