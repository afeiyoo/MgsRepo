#pragma once

#include <QObject>

#define GM_INSTANCE GlobalManager::instance()

class Config;
class GlobalManager : public QObject
{
    Q_OBJECT
public:
    explicit GlobalManager(QObject *parent = nullptr);
    ~GlobalManager() override;

    static GlobalManager *instance();

    // 全局初始化
    int init(int argc, char *argv[]);

public:
    Config *m_conf = nullptr; // 全局配置
};
