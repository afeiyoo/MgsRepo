#pragma once

#include "global/defs.h"
#include "utils/configutils.h"
#include <QObject>

class ConfigManager : public QObject
{
    Q_OBJECT
public:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager() override;

    void loadConfig(const QString &configPath);

public:
    ST_BaseConfig m_baseConfig;
    ST_DBConfig m_dbConfig;
    ST_LogConfig m_logConfig;

private:
    Utils::ConfigUtils *m_confUtil = nullptr;
};
