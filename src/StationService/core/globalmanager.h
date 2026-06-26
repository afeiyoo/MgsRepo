#pragma once

#include <QObject>

#define GM_INS GlobalManager::instance()

class Config;
class GlobalManager : public QObject
{
    Q_OBJECT
public:
    explicit GlobalManager(QObject *parent = nullptr);
    ~GlobalManager() override;

    static GlobalManager *instance();

    int init();

public:
    Config *m_conf = nullptr; // 配置管理类
};
