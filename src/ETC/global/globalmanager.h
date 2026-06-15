#pragma once

#include <QObject>

#define GM_INSTANCE GlobalManager::instance()

class Config;
class DataService;
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
    Config *m_conf = nullptr;    // 全局配置
    DataService *m_ds = nullptr; // 数据库操作类
};
