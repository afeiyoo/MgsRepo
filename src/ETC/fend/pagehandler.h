#pragma once

#include <QObject>

class IEtcPageController;
class PageHandler : public QObject
{
    Q_OBJECT
public:
    explicit PageHandler(QObject *parent = nullptr);
    ~PageHandler() override;

    void init(IEtcPageController *newUi);

public slots:
    void onUpdateTradeHint(const QString &info, bool isWarn) const;
    void onUpdateHelpHint(const QString &info) const;
    void onShowInfoDialog(int api, const QString &title, const QStringList &strs, bool switchLine);

private:
    IEtcPageController *m_ui = nullptr;
};
