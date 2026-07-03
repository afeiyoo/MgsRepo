#pragma once

#include <QObject>

class BizHandler : public QObject
{
    Q_OBJECT
public:
    explicit BizHandler(QObject *parent = nullptr);
    ~BizHandler() override;

    // 返回当前是否上班
    bool isShiftedStarted();

    // 系统初始化
    void systemInit();

    // 系统退出API
    void quitSystemRequest();
    void quitSystemQuery();

    // 交接班API
    void exchangeShiftRequest();
    void endShift();

public slots:

private:
private:
};
