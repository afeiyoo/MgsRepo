#pragma once

#include <QMessageBox>
#include <QObject>

class SignalCtrl : public QObject
{
    Q_OBJECT
public:
    explicit SignalCtrl(QObject *parent = nullptr);

signals:
    // 界面更新API
    void sigShowUpdateRequest(int uiType, const QJsonValue &values);

    // 窗口API
    void sigShowFormRequest(int formType, int api, const QJsonValue &values);
};
