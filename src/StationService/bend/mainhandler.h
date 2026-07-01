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

    QString dealQueryXZPass(const QVariantMap &aMap) const;

    QString dealQueryShift(const QVariantMap &aMap) const;

    QString dealQueryData(const QVariantMap &aMap) const;

    QString dealQueryETCBlack(const QVariantMap &aMap) const;

    QString checkETCBlackInfo(const QString &version) const;

    QString getCurBlackVersion() const;

private:
    QString m_peerIP; // 请求IP
};
