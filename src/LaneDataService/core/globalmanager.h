#pragma once

#include <QObject>
#include <QTimer>

#define GM_INS GlobalManager::instance()

class Config;
class DataService;
class IMessageBus;
class SqlDealer;
class SignalManager;
class FullBlackChecker;

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
    // Sql语句管理对象
    SqlDealer *m_sqlDealer = nullptr;
    // 数据库操作对象(包括全量与增量)
    DataService *m_ds = nullptr;
    // 全局信号管理器
    SignalManager *m_sigMan = nullptr;

private:
    // 全量状态检查对象
    QTimer *m_fullBlackTimer = nullptr;
    FullBlackChecker *m_fullBlackChecker = nullptr;
};
