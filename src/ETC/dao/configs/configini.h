#pragma once

#include <QObject>

#include "config.h"

class ConfigIni : public Config
{
    Q_OBJECT
public:
    explicit ConfigIni(QObject *parent = nullptr);
    ~ConfigIni() override;

    void load(const Utils::FileName &confPath) override;
};
