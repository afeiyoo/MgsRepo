#pragma once

#include <QObject>
#include <QTimer>

#define GM_INS GlobalManager::instance()

class Config;
class DataService;
class IMessageBus;
class SignalManager;
class FullBlackMaster;
class Environment;
class SqlDealer;

class GlobalManager : public QObject
{
    Q_OBJECT
public:
    explicit GlobalManager(QObject *parent = nullptr);
    ~GlobalManager() override;

    static GlobalManager *instance();

    // -100:配置文件不存在
    // -101:SQL文件加载失败
    // -102:数据库连接初始化失败
    int init();

public:
    // 配置文件路径
    QString m_confPath;
    // 配置管理对象
    Config *m_conf = nullptr;
    // 数据库操作对象(包括全量与增量)
    DataService *m_ds = nullptr;
    // 全局信号管理器
    SignalManager *m_sigMan = nullptr;
    // 全量检查对象
    FullBlackMaster *m_fbMaster = nullptr;
    // 全局环境变量
    Environment *m_env = nullptr;
    // SQL语句获取器（线程安全）
    SqlDealer *m_sql = nullptr;
};
