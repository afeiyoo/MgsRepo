#pragma once

#include <QObject>
#include <QThread>
#include <QVariantList>
#include <QVariantMap>

class Http;

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
    void downloadNextFullFile();
    void finishFullDownload(bool success);
    void removeDownloadedFiles(const QStringList &filePaths);
    bool saveFullXml(const QVariantMap &map);
    void removeOldFullFiles(const QString &ver);

protected:
    void run() override;

private:
    QString m_curDateStr;
    qint64 m_checkIncrementTime = 0;
    qint64 m_checkFullTime = 0;
    qint64 m_delJsonTime = 0;

    bool m_fullDownloading = false;
    int m_fullDownloadIndex = 0;
    // 当前下载上下文
    QVariantList m_fullFileList;
    QVariantList m_fullMd5List;
    QVariantList m_fullUrlList;

    QVariantMap m_fullResMap; // 用于保存BlackUpdate.xml
    QString m_fullRemoteVer;
    QStringList m_fullDownloadedFiles;
    Http *m_fullHttpClient = nullptr;
};
