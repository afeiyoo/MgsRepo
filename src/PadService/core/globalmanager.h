#pragma once

#include <QDir>
#include <QMap>
#include <QObject>
#include <QTimer>

#define GM_INSTANCE GlobalManager::instance()

class DtpSender;
class ConfigManager;
class DataService;
class GlobalManager : public QObject
{
    Q_OBJECT
public:
    explicit GlobalManager(QObject *parent = nullptr);
    ~GlobalManager() override;

    static GlobalManager *instance();

    int init();

private:
    void initRemoteURIs();

private slots:
    void onCleanExpiredPictures();
    void onCleanExpiredCaches();

public:
    // 配置文件路径
    QString m_confPath;
    // 配置对象
    ConfigManager *m_configMan = nullptr;
    // 数据库操作对象
    DataService *m_ds = nullptr;
    // 图片保存目录
    QDir m_pictureDir;
    // DTP传输对象
    DtpSender *m_dtpSender = nullptr;
    // 云坐席台账接口URI
    QMap<int, QString> m_remoteURIs;

private:
    QTimer *m_cleanupTimer = nullptr;
};
