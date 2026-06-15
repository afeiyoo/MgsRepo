#pragma once

#include <QObject>

class Environment;
class BizHandler : public QObject
{
    Q_OBJECT
public:
    explicit BizHandler(QObject *parent = nullptr);
    ~BizHandler() override;

    // 返回当前是否上班
    bool isInShifted();

    void quitSystem();

private:
    Environment *m_env = nullptr;
};
