#pragma once

#include <QObject>
#include <QThread>

class Cron : public QThread
{
    Q_OBJECT
public:
    explicit Cron(QObject *parent = nullptr);
    ~Cron() override;

private:
    // 删除过期文件
    void checkJsonToDelete();
    int deletePathFile(const QString &path);

    // 增量
    int getIncrementFiles();
    bool checkVersionIsValid(const QString &version) const;

    // 全量
    int getFullFiles();
    int downloadFullFiles(const QVariantList &fileList, const QVariantList &md5Lit, const QVariantList &urlList);
    void removeDownloadedFiles(const QStringList &filePaths);
    bool saveFullXml(const QVariantMap &map);

protected:
    void run() override;

private:
    QString m_curDateStr;
    qint64 m_checkIncrementTime = 0;
    qint64 m_checkFullTime = 0;
    qint64 m_delJsonTime = 0;
};
