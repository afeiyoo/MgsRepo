#include "environment.h"

Environment::Environment(QObject *parent)
    : QObject{parent}
{}

QString Environment::growthBlackVersion() const
{
    return m_growthBlackVersion;
}

void Environment::setGrowthBlackVersion(const QString &newGrowthBlackVersion)
{
    m_growthBlackVersion = newGrowthBlackVersion;
}
