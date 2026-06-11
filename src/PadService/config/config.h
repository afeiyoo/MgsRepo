#pragma once

#include "global/defs.h"
#include "utils/configutils.h"
#include "utils/fileutils.h"
#include <QObject>

class FileName;
class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = nullptr);
    ~Config() override;

    void loadConfig(const Utils::FileName &configPath);

public:
    ST_BaseConfig m_baseConfig;
    ST_DBConfig m_dbConfig;
    ST_LogConfig m_logConfig;

private:
    Utils::ConfigUtils *m_confUtil = nullptr;
};
