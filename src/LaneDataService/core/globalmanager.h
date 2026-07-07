#pragma once

#include <QObject>

#define GM_INS GlobalManager::instance()

class Config;
class DataService;
class GlobalManager : public QObject
{
    Q_OBJECT
public:
    explicit GlobalManager(QObject *parent = nullptr);
    ~GlobalManager() override;

    static GlobalManager *instance();

    int init();

public:
    // 配置文件路径
    QString m_confPath;
    // 配置管理对象
    Config *m_conf = nullptr;
    // 数据库操作对象
    DataService *m_ds = nullptr;
};
