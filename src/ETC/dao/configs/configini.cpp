#include "configini.h"

#include "utils/configutils.h"
#include "utils/fileutils.h"

using namespace Utils;

ConfigIni::ConfigIni(QObject *parent)
    : Config{parent}
{}

ConfigIni::~ConfigIni() {}

void ConfigIni::load(const FileName &confPath)
{
    QString str = FileUtils::canonicalPath(confPath).toString();
    m_confUtil->init(str, ConfigUtils::INI);

    // 数据库配置加载
    m_dbType = m_confUtil->getValue("LaneDB/dbtype", 1).toUInt();
    m_dbHost = m_confUtil->getValue("LaneDB/dbhost", "127.0.0.1").toString();
    m_dbPort = m_confUtil->getValue("LaneDB/dbport", 3309).toUInt();
    m_dbName = m_confUtil->getValue("LaneDB/dbname", "tolllanedb").toString();
    m_dbUser = m_confUtil->getValue("LaneDB/dbuser", "tlman").toString();
    m_dbPassword = m_confUtil->getValue("LaneDB/dbpasswd", "ds18fjeit").toString();

    // Redis配置加载
    m_redisHost = m_confUtil->getValue("RedisDB/dbhost", "").toString();
    m_redisPort = m_confUtil->getValue("RedisDB/dbport", 6379).toUInt();
    m_redisDBName = m_confUtil->getValue("RedisDB/dbname", "0").toString();
    m_redisUser = m_confUtil->getValue("RedisDB/dbuser", "test").toString();
    m_redisPassword = m_confUtil->getValue("RedisDB/dbpasswd", "Mgskj@202101").toString();

    // 车道基础配置加载
    m_stationName = m_confUtil->getValue("BaseInfo/stationname", "").toString();
    m_stationID = m_confUtil->getValue("BaseInfo/stationid", "").toString();
    m_laneID = m_confUtil->getValue("BaseInfo/laneid", "").toInt();
    m_laneType = m_confUtil->getValue("BaseInfo/lanetype", "").toInt();
    m_isTest = m_confUtil->getValue("BaseInfo/istest", "0").toInt();
}
