#include "config.h"

#include "utils/configutils.h"
#include "utils/fileutils.h"
#include "utils/stdafx.h"

using namespace Utils;

Config::Config(QObject *parent)
    : QObject{parent}
{
    m_confUtil = new ConfigUtils();
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
    m_dbType = m_confUtil->getValue("DataBase/DBType", 1).toUInt(); // 默认sqlserver数据库 2:ODBC连接达梦
    m_dbHost = m_confUtil->getValue("DataBase/DBIP", "127.0.0.1").toString();
    m_dbPort = m_confUtil->getValue("DataBase/DBPort", 1433).toUInt();
    m_dbName = m_confUtil->getValue("DataBase/DBName", "tollstationdb").toString();
    m_dbUser = m_confUtil->getValue("DataBase/DBUser", "tsman").toString();
    m_dbPassword = m_confUtil->getValue("DataBase/DBPassword", "ds18fjeit").toString();

    // 日志配置
    m_logFormat = m_confUtil->getValue("Log/Format", "%{time} [%{type}] [%{threadid}] %{message}\n\n").toString();
    m_logLimits = m_confUtil->getValue("Log/Limit", 180).toUInt();

    // 基础配置
    m_queryAuthType = m_confUtil->getValue("BaseEnv/AuthType", 0).toInt();
    m_queryAuthIP = m_confUtil->getValue("BaseEnv/AuthIP", "").toString();
    m_incrementCheckUrl = m_confUtil->getValue("BaseEnv/BlackUrl", "").toString();
    m_saveDays = m_confUtil->getValue("BaseEnv/SaveDays", 31).toUInt();
    m_fullSavePath = m_confUtil->getValue("BaseEnv/FullSavePath", "").toString();
    m_fullCheckUrl = m_confUtil->getValue("BaseEnv/FullCheckUrl", "").toString();
}
