#include "config.h"

#include "utils/configutils.h"
#include "utils/stdafx.h"

using namespace Utils;

Config::Config()
{
    m_confUtil = new ConfigUtils();
}

Config::~Config()
{
    SAFE_DELETE(m_confUtil);
}

void Config::loadConfig(const QString &path)
{
    m_confUtil->init(path, ConfigUtils::INI);

    m_apiType = m_confUtil->getValue("reader/apiType", 0).toInt();
    m_apiName = m_confUtil->getValue("reader/apiName", "API_JKM115").toString();
    m_laneType = m_confUtil->getValue("reader/laneType", 1).toInt();
    m_comPort = m_confUtil->getValue("reader/comPort", "").toString();
    m_slotNo = m_confUtil->getValue("reader/slotNo", -1).toInt();
}

ST_ConfigSnap Config::getSnap() const
{
    QReadLocker locker(&m_lock);

    ST_ConfigSnap snap;
    snap.apiName = m_apiName;
    snap.apiType = m_apiType;
    snap.laneType = m_laneType;
    snap.comPort = m_comPort;
    snap.slotNo = m_slotNo;

    return snap;
}

void Config::updateConfig(int apiType, const QString &apiName, const QString &comPort, int slotNo)
{
    QWriteLocker locker(&m_lock);

    m_confUtil->setValue("reader/apiType", apiType);
    m_confUtil->setValue("reader/apiName", apiName);
    m_confUtil->setValue("reader/comPort", comPort);
    m_confUtil->setValue("reader/slotNo", slotNo);
    m_apiType = apiType;
    m_apiName = apiName;
    m_comPort = comPort;
    m_slotNo = slotNo;
}
