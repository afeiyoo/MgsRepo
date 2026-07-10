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
    m_sqlFiles = QStringList{":/static/sqls/mtc_in.xml", ":/static/sqls/mtc_out.xml", ":/static/sqls/etc.xml"};
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

    // 日志配置
    m_logFormat = m_confUtil->getValue("Log/format", "%{time} [%{type}] [%{threadid}] %{message}\n\n").toString();
    m_logLimits = m_confUtil->getValue("Log/limit", 180).toUInt();
}
