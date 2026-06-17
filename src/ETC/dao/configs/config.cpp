#include "config.h"

#include "utils/configutils.h"
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
