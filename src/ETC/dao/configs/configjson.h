#pragma once

#include <QObject>

#include "config.h"

class ConfigJson : public Config
{
    Q_OBJECT
public:
    explicit ConfigJson(QObject *parent = nullptr);
    ~ConfigJson() override;

    void load(const Utils::FileName &confPath) override;
};
