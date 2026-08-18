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
    snap.isDeltaBlackValid = m_isDeltaBlackValid;
    snap.deltaBlackStatus = m_deltaBlackStatus;
    snap.deltaBlackVersion = m_deltaBlackVersion;

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

void Environment::updateDeltaBlackEnvs(bool isValid, int status, const QString &version)
{
    QWriteLocker locker(&m_lock);
    m_isDeltaBlackValid = isValid;
    m_deltaBlackStatus = status;
    m_deltaBlackVersion = version;
}
