#pragma once

#include <QObject>

class SignalManager : public QObject
{
    Q_OBJECT
public:
    explicit SignalManager(QObject *parent = nullptr);

signals:
    // 启动全量检查
    void sigCheckFullBlack();
};
