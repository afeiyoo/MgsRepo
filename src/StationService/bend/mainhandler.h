#pragma once

#include <QObject>

class MainHandler : public QObject
{
    Q_OBJECT
public:
    explicit MainHandler(const QString &peerIP, QObject *parent = nullptr);
    ~MainHandler() override;

    QString doMainDeal(const QByteArray &reqBody) const;

private:
    QString dealQueryRepeat(const QVariantMap &aMap) const;

    QString dealSaveData(const QVariantMap &aMap) const;

private:
    QString m_peerIP; // 请求IP
};
