#include "configutils.h"

namespace Utils {

ConfigUtils::ConfigUtils(QObject *parent)
    : QObject{parent}
{}

ConfigUtils::~ConfigUtils()
{
    backup(); // 程序析构时，也备份一下
}

void ConfigUtils::init(const QString &filename, ConfigFormat format)
{
    m_filename = filename;
    m_format = format;
    checkBackup(); // 一启动先检查配置坏没坏
}

QVariant ConfigUtils::getValue(const QString &key, const QVariant &defaultValue)
{
    QReadLocker locker(&m_lock); // 读加锁，允许多线程同时读
    if (m_format == ConfigFormat::INI) {
        QSettings set(m_filename, QSettings::IniFormat);
        return set.value(key, defaultValue);
    } else if (m_format == ConfigFormat::JSON) {
        return getJsonValue(key, defaultValue);
    }
    return defaultValue;
}

void ConfigUtils::setValue(const QString &key, const QVariant &value)
{
    QWriteLocker locker(&m_lock); // 写加锁，同一时间只能一个线程写
    if (m_format == ConfigFormat::INI) {
        QSettings set(m_filename, QSettings::IniFormat);
        set.setValue(key, value);
        set.sync();
        backup(); // 写完顺手备个份
    } else if (m_format == ConfigFormat::JSON) {
        setJsonValue(key, value);
    }
}

void ConfigUtils::checkBackup()
{
    QFile f(m_filename);
    // 文件不存在或者解析失败，都视为损坏
    if (!f.exists() || isFileCorrupted()) {
        QFile fbak(m_filename + ".bak");
        if (fbak.exists()) {
            // 配置文件损坏，已自动从备份恢复
            fbak.copy(m_filename);
        } else {
            // 配置文件不存在，创建一个新的空文件
            f.open(QIODevice::WriteOnly);
            f.close();
        }
    }
}

void ConfigUtils::backup()
{
    QFile::remove(m_filename + ".bak");
    QFile::copy(m_filename, m_filename + ".bak");
}

bool ConfigUtils::isFileCorrupted()
{
    QFile f(m_filename);
    if (!f.open(QIODevice::ReadOnly)) {
        return true;
    }
    bool corrupted = false;
    if (m_format == ConfigFormat::JSON) {
        QJsonParseError e;
        QJsonDocument::fromJson(f.readAll(), &e);
        corrupted = (e.error != QJsonParseError::NoError);
    }
    f.close();
    return corrupted;
}

QVariant ConfigUtils::getJsonValue(const QString &key, const QVariant &def)
{
    QFile f(m_filename);
    if (!f.open(QIODevice::ReadOnly)) {
        return def;
    }
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    // 支持斜杠路径解析，例如 "Serial/Port"
    QStringList keys = key.split("/");
    QJsonObject obj = doc.object();
    for (int i = 0; i < keys.size() - 1; ++i) {
        obj = obj.value(keys[i]).toObject();
    }

    QVariant res = obj.value(keys.last()).toVariant();
    return res.isNull() ? def : res;
}

void ConfigUtils::setJsonValue(const QString &key, const QVariant &val)
{
    QJsonObject rootObj;
    if (QFile(m_filename).exists()) {
        QFile f(m_filename);
        if (f.open(QIODevice::ReadOnly)) {
            rootObj = QJsonDocument::fromJson(f.readAll()).object();
            f.close();
        }
    }

    QStringList keys = key.split("/");
    // 用一个列表按顺序记录路径上所有的 QJsonObject
    QVector<QJsonObject> objects;
    objects.append(rootObj);

    // 逐层深入，获取或创建中间的 QJsonObject
    for (int i = 0; i < keys.size() - 1; ++i) {
        QJsonObject currentObj = objects.last().value(keys[i]).toObject();
        objects.append(currentObj);
    }

    objects.last().insert(keys.last(), QJsonValue::fromVariant(val));

    // 从底层往上倒推，把更新后的子对象重新组装回父对象中
    for (int i = keys.size() - 2; i >= 0; --i) {
        objects[i].insert(keys[i], objects[i + 1]);
    }
    rootObj = objects.first();

    //关键点：原子写入临时文件，再安全替换（保持你原有的优秀设计）
    QString tmpName = m_filename + ".tmp";
    QFile ftmp(tmpName);
    if (ftmp.open(QIODevice::WriteOnly)) {
        ftmp.write(QJsonDocument(rootObj).toJson());
        ftmp.close();
        QFile::remove(m_filename);
        QFile::rename(tmpName, m_filename);
    }

    backup();
}

} // namespace Utils
