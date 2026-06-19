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
    void sigUpdateTradeHint(const QString &info, bool isWarn = false);
    void sigUpdateHelpHint(const QString &info);

    // 窗口API
    void sigShowInfoDialog(int api, const QString &title, const QStringList &strs, bool switchLine = true);
};
