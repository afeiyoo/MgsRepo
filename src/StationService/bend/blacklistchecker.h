#pragma once

#include <QObject>
#include <QThread>

class BlackListChecker : public QThread
{
    Q_OBJECT
public:
    explicit BlackListChecker(QObject *parent = nullptr);
    ~BlackListChecker() override;

private:
    void checkJsonToDelete();
    int deletePathFile(const QString &path);

    int getBlackJsonMsg();
    bool checkVersionIsValid(const QString &version) const;

protected:
    void run() override;

private:
    QString m_curDateStr;
    qint64 m_checkJsonTime = 0;
    qint64 m_delJsonTime = 0;
};
