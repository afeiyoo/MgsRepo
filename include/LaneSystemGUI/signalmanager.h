#pragma once

#include <QObject>
#include <QString>

#include "lanesystemgui_global.h"

class LANESYSTEMGUI_EXPORT SignalManager : public QObject
{
    Q_OBJECT
public:
    explicit SignalManager(QObject *parent = nullptr);
    ~SignalManager() override;

signals:
    // 称重信息区域信号
    void sigShowUpdateCurWeightInfo(const QString &info, const QString &color = "#000000"); // 当前称重信息刷新
    void sigShowUpdateWeightCount(uint count);                                              // 称重缓冲区数量刷新

    // 按键信号
    void sigKeyPress(int key);

    // 对话框返回
    void sigDialogResp(int api, const QJsonValue &values);
};
