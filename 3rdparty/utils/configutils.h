#pragma once

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QObject>
#include <QPair>
#include <QReadWriteLock>
#include <QSettings>
#include <QVector>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace Utils {

class ConfigUtils : public QObject
{
    Q_OBJECT
public:
    // 配置文件格式，两种常用的都支持
    enum ConfigFormat { INI, JSON };

    explicit ConfigUtils(QObject *parent = nullptr);
    ~ConfigUtils() override;

    // 初始化：指定文件路径 + 格式
    // 程序启动时调用一次就行
    void init(const QString &filename, ConfigFormat format);

    // 读配置，带默认值，没有 key 也不会崩
    QVariant getValue(const QString &key, const QVariant &defaultValue = QVariant());
    // 写配置，自动加锁、原子写入、备份
    void setValue(const QString &key, const QVariant &value);

    void checkBackup();

    // 手动备份，一般不用自己调，setValue 里会自动调用
    void backup();

private:
    bool isFileCorrupted();
    // ==================== JSON 读写 ====================
    QVariant getJsonValue(const QString &key, const QVariant &def);
    void setJsonValue(const QString &key, const QVariant &val);

private:
    QString m_filename;
    ConfigFormat m_format;
    QReadWriteLock m_lock; // 读写锁，多线程安全核心
};

} // namespace Utils
