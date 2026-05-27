#pragma once

#include <QObject>

#define GM_INSTANCE GlobalManager::instance()
#define GM_SIG GM_INSTANCE->m_signalMan

class SignalManager;
class GlobalManager : public QObject
{
    Q_OBJECT
public:
    explicit GlobalManager(QObject *parent = nullptr);
    ~GlobalManager() override;

    static GlobalManager *instance();

    void init();

public:
    // 信号管理中心
    SignalManager *m_signalMan = nullptr;
};
