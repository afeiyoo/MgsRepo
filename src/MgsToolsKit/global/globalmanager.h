#pragma once

#include <QObject>

#define GM_INSTANCE GlobalManager::instance()

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
    SignalManager *m_signalMan = nullptr;
};
