#pragma once

#include <QObject>

#define GM_INS GlobalManager::instance()

class Config;
class ReaderCtrl;
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
    // 配置管理类
    Config *m_config = nullptr;
    // 读卡器控制类
    ReaderCtrl *m_reader = nullptr;
};
