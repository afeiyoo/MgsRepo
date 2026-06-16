#pragma once

#include <QMessageBox>
#include <QObject>

#include "etc_global.h"

class ETC_EXPORT ETC : public QObject
{
    Q_OBJECT
public:
    explicit ETC(QObject *parent = nullptr);
    ~ETC() override;

    int init(int argc, char *argv[]);

public slots:
    void onKeyPress(int key);                               // 前端按键响应
    void onShowFormResp(int api, const QJsonValue &values); // 前端弹窗响应

signals:
    // 窗口API
    void sigShowFormRequest(int formType, int api, const QJsonValue &values);
};
