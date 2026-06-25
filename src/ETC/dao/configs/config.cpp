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
    m_logMaxSaveDays = 180;
}

Config::~Config()
{
    SAFE_DELETE(m_confUtil);
}

QString Config::dump() const
{
    QString content;

    content += QStringLiteral("[BaseInfo] stationName: %1, stationID: %2, laneID: %3, laneType: %4\n")
                   .arg(m_stationName, m_stationID)
                   .arg(m_laneID)
                   .arg(m_laneType);

    content += QStringLiteral("[LaneDB] dbType: %1, dbHost: %2, dbPort: %3, dbName: %4, dbUser: %5, dbPassword: %6\n")
                   .arg(m_dbType)
                   .arg(m_dbHost)
                   .arg(m_dbPort)
                   .arg(m_dbName)
                   .arg(m_dbUser)
                   .arg(m_dbPassword);

    content += QStringLiteral("[RedisDB] redisHost: %1, redisPort: %2, redisDBName: %3, redisUser: %4, redisPassword: %5\n")
                   .arg(m_redisHost)
                   .arg(m_redisPort)
                   .arg(m_redisDBName)
                   .arg(m_redisUser)
                   .arg(m_redisPassword);

    content += QStringLiteral("[Test] isTest: %1, plate: %2, imagePath: %3").arg(m_isTest).arg(m_testPlate).arg(m_testImagePath);

    return content;
}

void Config::readConfig()
{
    if (!m_confUtil)
        return;

    // 数据库配置加载
    m_dbType = m_confUtil->getValue("LaneDB/dbtype", 1).toUInt();
    m_dbHost = m_confUtil->getValue("LaneDB/dbhost", "127.0.0.1").toString();
    m_dbPort = m_confUtil->getValue("LaneDB/dbport", 3306).toUInt();
    m_dbName = m_confUtil->getValue("LaneDB/dbname", "tolllanedb").toString();
    m_dbUser = m_confUtil->getValue("LaneDB/dbuser", "tlman").toString();
    m_dbPassword = m_confUtil->getValue("LaneDB/dbpasswd", "ds18fjeit").toString();

    // Redis配置加载
    m_redisHost = m_confUtil->getValue("RedisDB/dbhost", "").toString();
    m_redisPort = m_confUtil->getValue("RedisDB/dbport", 6379).toUInt();
    m_redisDBName = m_confUtil->getValue("RedisDB/dbname", "0").toString();
    m_redisUser = m_confUtil->getValue("RedisDB/dbuser", "test").toString();
    m_redisPassword = m_confUtil->getValue("RedisDB/dbpasswd", "Mgskj@202101").toString();

    // 测试配置加载
    m_isTest = m_confUtil->getValue("Test/istest", false).toBool();
    m_testPlate = m_confUtil->getValue("Test/plate", "").toString();
    m_testImagePath = m_confUtil->getValue("Test/imgpath", "").toString();

    // 车道基础配置加载
    m_stationName = m_confUtil->getValue("BaseInfo/stationname", "").toString();
    m_stationID = m_confUtil->getValue("BaseInfo/stationid", "").toString();
    m_laneID = m_confUtil->getValue("BaseInfo/laneid", 0).toUInt();
    m_laneType = m_confUtil->getValue("BaseInfo/lanetype", 1).toUInt();
    m_psdFlagID = m_confUtil->getValue("BaseInfo/psdflagid", "").toString();
    m_psdFlagName = m_confUtil->getValue("BaseInfo/psdflagname", "").toString();

    // 车道个性化配置
    m_isEnableCompareVehplate = m_confUtil->getValue("SpecialConf/isenablecomparevehplate", false).toBool();
    m_savePicPath = m_confUtil->getValue("SpecialConf/savepicpath", FileUtils::curApplicationDirPath() + "/upload").toString();
    m_showDevsList = m_confUtil->getValue("SpecialConf/showdevslist", "").toString();
}
