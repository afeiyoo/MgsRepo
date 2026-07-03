#pragma once

#include <QObject>
#include <QSet>

#include "EasyQtSql/EasyQtSql.h"
#include "utils/fileutils.h"

#define GM_INSTANCE GlobalManager::instance()

class DtpSender;
class Config;
class GlobalManager : public QObject
{
    Q_OBJECT
public:
    explicit GlobalManager(QObject *parent = nullptr);
    ~GlobalManager() override;

    static GlobalManager *instance();

    void init();

public:
    // 配置文件路径
    QString m_confPath;
    // 配置对象
    Config *m_config = nullptr;
    // 数据库连接池
    EasyQtSql::SqlFactory *m_dbFactory = nullptr;
    // 图片保存目录
    Utils::FileName m_pictureDir;
    // DTP传输对象
    DtpSender *m_dtpSender = nullptr;
    // 云坐席台账接口URI
    QMap<int, QString> m_remoteURIs;
};
