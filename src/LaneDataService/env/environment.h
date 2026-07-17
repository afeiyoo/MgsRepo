#pragma once

#include <QObject>

class Environment : public QObject
{
    Q_OBJECT
public:
    explicit Environment(QObject *parent = nullptr);

    QString growthBlackVersion() const;
    void setGrowthBlackVersion(const QString &newGrowthBlackVersion);

private:
    QString m_growthBlackVersion; // 增量版本
};
