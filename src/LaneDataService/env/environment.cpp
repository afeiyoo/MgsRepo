#include "environment.h"

#include "core/globalmanager.h"
#include "env/defines.h"

Environment::Environment(QObject *parent)
    : QObject{parent}
{}

Environment::~Environment() {}

ST_EnvSnap Environment::getEnvSnap() const
{
    QReadLocker locker(&m_lock);

    ST_EnvSnap snap;
    snap.isGrowthBlackVersion = m_isGrowthBlackVersion;
    snap.growthBlackVersion = m_growthBlackVersion;
    snap.isFullBlackValid = m_isFullBlackValid;
    snap.fullBlackStatus = m_fullBlackStatus;
    snap.fullBlackVersion = m_fullBlackVersion;

    return snap;
}

void Environment::updateFullBlackEnvs(bool isValid, int status, const QString &version)
{
    QWriteLocker locker(&m_lock);
    m_isFullBlackValid = isValid;
    m_fullBlackStatus = status;
    m_fullBlackVersion = version;
}
