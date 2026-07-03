#pragma once

#include <QObject>
#include <QVariantMap>

#define GM_INSTANCE GlobalManager::instance()

class Config;
class DataService;
class GateWay;
class Environment;
class BizHandler;
class SignalCtrl;
class PageHandler;
class Environment;
class IEtcPageController;
class GlobalManager : public QObject
{
    Q_OBJECT
public:
    explicit GlobalManager(QObject *parent = nullptr);
    ~GlobalManager() override;

    static GlobalManager *instance();

    // 全局初始化
    int init(IEtcPageController *ui);

public:
    Config *m_conf = nullptr;             // 全局配置
    DataService *m_ds = nullptr;          // 数据库操作类
    GateWay *m_gate = nullptr;            // 软件网关
    QVariantMap m_keyBoard;               // 车道键盘表
    BizHandler *m_bizHandler = nullptr;   // 业务处理类
    SignalCtrl *m_sigCtrl = nullptr;      // 消息中心
    PageHandler *m_pageHandler = nullptr; // 界面控制
    Environment *m_env = nullptr;         // 全局业务环境变量
};
