#pragma once

#include <QMessageBox>
#include <QObject>

#include "etc_global.h"

class IEtcPageController;
class ETC_EXPORT ETC : public QObject
{
    Q_OBJECT
public:
    explicit ETC(QObject *parent = nullptr);
    ~ETC() override;

    int init(IEtcPageController *ui);

public slots:
    void onKeyPress(int key);                             // 前端按键响应
    void onDialogResp(int api, const QJsonValue &values); //  前端弹窗响应
};
