#include "iniutils.h"

#include "qtcassert.h"

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRect>
#include <QSize>
#include <QTemporaryFile>
#include <QTextStream>

namespace Utils {

IniUtils::IniUtils(const QString &filepath, QIODevice::OpenMode mode, QObject *parent)
    : QObject(parent)
    , m_iniPath(QDir::fromNativeSeparators(filepath))
    , m_iniMode(mode)
{
    load();
}

IniUtils::~IniUtils()
{
    save();
}

QString IniUtils::dumpInfo() const
{
    QString helpInfo;
    QTextStream stream(&helpInfo);

    stream << "start loading " << m_iniPath.split('/').last() << " ==> ";

    for (const auto &agroup : m_iniData.datas) {
        if (!agroup.isHead)
            stream << agroup.group << " {";

        bool first = true;
        for (const auto &arow : agroup.rows) {
            if (arow.isValid) {
                if (!first)
                    stream << ", ";
                stream << arow.key << ":" << arow.value.toString();
                first = false;
            }
        }
        if (!agroup.isHead)
            stream << "}; ";
    }

    return helpInfo;
}

QStringList IniUtils::groups() const
{
    auto result = m_iniData.allgroups;
    result.removeAll(QString());
    return result;
}

QStringList IniUtils::keys(const QString &group) const
{
    QStringList keyList;
    int groupIndex = m_iniData.allgroups.indexOf(group);
    if (group.isEmpty() || groupIndex < 0)
        return keyList;
    auto result = m_iniData.datas.at(groupIndex).allkeys;
    result.removeAll(QString());
    return result;
}

QVariant IniUtils::value(const QString &group, const QString &key, const QVariant &def, Qt::CaseSensitivity cs) const
{
    int groupIndex = getStrListIndex(m_iniData.allgroups, group, cs);
    if (group.isEmpty() || key.isEmpty() || groupIndex < 0)
        return def.toString();

    int keyIndex = getStrListIndex(m_iniData.datas.at(groupIndex).allkeys, key, cs);
    if (keyIndex < 0)
        return def.toString();

    return m_iniData.datas.at(groupIndex).rows.at(keyIndex).value;
}

void IniUtils::setValue(const QString &group, const QString &key, const QVariant &value, Qt::CaseSensitivity cs)
{
    if (group.isEmpty() || key.isEmpty())
        return;
    // 如果是不存在的分组则创建
    int groupIndex = getStrListIndex(m_iniData.allgroups, group, cs);
    if (groupIndex < 0) {
        IniGroup iniGroup;
        iniGroup.isHead = false;
        iniGroup.group = group;
        m_iniData.datas.push_back(iniGroup);
        m_iniData.allgroups.push_back(group);
        groupIndex = m_iniData.allgroups.count() - 1;
    }
    // 如果不存在key则创建
    int keyIndex = getStrListIndex(m_iniData.datas.at(groupIndex).allkeys, key, cs);
    if (keyIndex < 0) {
        IniRow iniRow;
        iniRow.isValid = true;
        iniRow.key = key;
        // ini_row.value = value;
        m_iniData.datas[groupIndex].rows.push_back(iniRow);
        m_iniData.datas[groupIndex].allkeys.push_back(key);
        keyIndex = m_iniData.datas.at(groupIndex).allkeys.count() - 1;
    }
    // 插入这里有点疑问，就是
    m_iniData.datas[groupIndex].rows[keyIndex].value = value;
    // 标记为已修改状态，在save时判断
    m_iniData.change = true;
}

bool IniUtils::save()
{
    // 只读
    QTC_ASSERT(!(QIODevice::ReadOnly == m_iniMode), return false);

    if (m_iniData.change && isWritable()) {
        QFile file(m_iniPath);
        // QIODevice::Text on windows endl=\r\n
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            QTextStream ts(&file);
            ts.setCodec("utf-8");

            for (const auto &agroup : m_iniData.datas) {
                if (!agroup.isHead) {
                    ts << '[' << agroup.group << ']' << endl;
                }

                for (const auto &arow : agroup.rows) {
                    if (arow.isValid) {
                        ts << arow.key << '=' << variantToString(arow.value) << endl;
                    } else {
                        ts << arow.key << endl;
                    }
                }
            }

            file.close();
            return true;
        }
    }
    return false;
}

void IniUtils::load(const QString &filepath)
{
    QTC_ASSERT(!(m_iniMode & ~(QIODevice::WriteOnly | QIODevice::ReadOnly | QIODevice::ReadWrite)), return);

    // 保存当前的
    save();

    m_iniData = IniData{};
    const QString path = filepath.isEmpty() ? m_iniPath : filepath;
    m_iniPath = QDir::fromNativeSeparators(path);

    QFile file(path);
    if ((QIODevice::WriteOnly == m_iniMode) || !file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    IniData iniData;
    IniGroup iniGroup;
    IniRow iniRow;
    QTextStream ts(&file);
    ts.setCodec("utf-8");

    // 默认分组
    iniGroup.isHead = true;
    while (!ts.atEnd()) {
        // 去掉首尾空格
        QString lineData = ts.readLine().trimmed();

        if (lineData.count() < 3 || lineData[0] == ';' || lineData[0] == '#') {
            // 长度小于三无效: a=0 or [a] 注释暂不考虑
            iniRow.isValid = false;
            iniRow.key = lineData;
            iniRow.value.clear();

            iniGroup.rows.push_back(iniRow);
            iniGroup.allkeys.push_back(QString());
            // LOG_INFO().noquote() << "invalid" << lineData;
        } else if (lineData[0] == '[' && lineData[lineData.length() - 1] == ']') {
            // 分组
            iniData.datas.push_back(iniGroup);
            iniData.allgroups.push_back(iniGroup.group);

            iniGroup.isHead = false;
            iniGroup.group = lineData.mid(1, lineData.count() - 2);

            iniGroup.rows.clear();
            iniGroup.allkeys.clear();
            // LOG_INFO().noquote() << "group" << iniGroup.group;
        } else {
            iniRow.isValid = false;
            int split_index = lineData.indexOf('=');
            if (split_index > 0 && split_index < lineData.length() - 1) {
                QString key = lineData.mid(0, split_index).trimmed();
                QString value = lineData.mid(split_index + 1, lineData.length() - split_index - 1).trimmed();
                if (!key.isEmpty() && !value.isEmpty()) {
                    iniRow.isValid = true;
                    iniRow.key = key;
                    iniRow.value = stringToVariant(value);

                    iniGroup.rows.push_back(iniRow);
                    iniGroup.allkeys.push_back(key);
                    // LOG_INFO().noquote() << "key-value" << key << value;
                }
            }
            if (!iniRow.isValid) {
                iniRow.key = lineData;
                iniRow.value.clear();

                iniGroup.rows.push_back(iniRow);
                iniGroup.allkeys.push_back(QString());
                // LOG_INFO().noquote() << "invalid" << lineData;
            }
        }
    }
    file.close();

    // 最后一个分组的数据
    if (iniGroup.isHead || (iniData.allgroups.count() > 0 && iniGroup.isHead != true)) {
        iniData.datas.push_back(iniGroup);
        iniData.allgroups.push_back(iniGroup.group);
    }

    m_iniData = iniData;
}

bool IniUtils::isWritable() const
{
    bool result = false;
    QFileInfo fileinfo(m_iniPath);
    if (fileinfo.exists()) {
        QFile file(m_iniPath);
        result = file.open(QFile::ReadWrite);
        file.close();
    } else {
        QDir dir(fileinfo.absolutePath());
        if (dir.exists() || dir.mkpath(dir.absolutePath())) {
            // 临时文件
            QTemporaryFile file(m_iniPath);
            result = file.open();
            file.close();
        }
    }
    return result;
}

int IniUtils::getStrListIndex(const QList<QString> &strList, const QString &key, Qt::CaseSensitivity cs) const
{
    int index = -1;
    //大小写不敏感
    for (int i = 0; i < strList.size(); i++) {
        if (strList[i].compare(key, cs) == 0) {
            index = i;
            break;
        }
    }
    return index;
}

QString IniUtils::variantToString(const QVariant &v)
{
    QString result;

    switch (v.type()) {
    case QVariant::Invalid:
        result = QLatin1String("@Invalid()");
        break;

    case QVariant::ByteArray: {
        QByteArray a = v.toByteArray();
        result = QLatin1String("@ByteArray(") + QLatin1String(a.constData(), a.size()) + QLatin1Char(')');
        break;
    }

    case QVariant::String:
    case QVariant::LongLong:
    case QVariant::ULongLong:
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::Bool:
    case QVariant::Double:
    case QVariant::KeySequence: {
        result = v.toString();
        if (result.contains(QChar::Null))
            result = QLatin1String("@String(") + result + QLatin1Char(')');
        else if (result.startsWith(QLatin1Char('@')))
            result.prepend(QLatin1Char('@'));
        break;
    }
#ifndef QT_NO_GEOM_VARIANT
    case QVariant::Rect: {
        QRect r = qvariant_cast<QRect>(v);
        result = QString::asprintf("@Rect(%d %d %d %d)", r.x(), r.y(), r.width(), r.height());
        break;
    }
    case QVariant::Size: {
        QSize s = qvariant_cast<QSize>(v);
        result = QString::asprintf("@Size(%d %d)", s.width(), s.height());
        break;
    }
    case QVariant::Point: {
        QPoint p = qvariant_cast<QPoint>(v);
        result = QString::asprintf("@Point(%d %d)", p.x(), p.y());
        break;
    }
#endif // !QT_NO_GEOM_VARIANT

    default: {
#ifndef QT_NO_DATASTREAM
        QDataStream::Version version;
        const char *typeSpec;
        if (v.type() == QVariant::DateTime) {
            version = QDataStream::Qt_5_6;
            typeSpec = "@DateTime(";
        } else {
            version = QDataStream::Qt_4_0;
            typeSpec = "@Variant(";
        }
        QByteArray a;
        {
            QDataStream s(&a, QIODevice::WriteOnly);
            s.setVersion(version);
            s << v;
        }

        result = QLatin1String(typeSpec) + QLatin1String(a.constData(), a.size()) + QLatin1Char(')');
#else
        Q_ASSERT(!"stringToVariant: Cannot save custom types without QDataStream support");
#endif
        break;
    }
    }

    return result;
}

QVariant IniUtils::stringToVariant(const QString &s)
{
    if (s.startsWith(QLatin1Char('@'))) {
        if (s.endsWith(QLatin1Char(')'))) {
            if (s.startsWith(QLatin1String("@ByteArray("))) {
                return QVariant(s.midRef(11, s.size() - 12).toLatin1());
            } else if (s.startsWith(QLatin1String("@String("))) {
                return QVariant(s.midRef(8, s.size() - 9).toString());
            } else if (s.startsWith(QLatin1String("@Variant(")) || s.startsWith(QLatin1String("@DateTime("))) {
#ifndef QT_NO_DATASTREAM
                QDataStream::Version version;
                int offset;
                if (s.at(1) == QLatin1Char('D')) {
                    version = QDataStream::Qt_5_6;
                    offset = 10;
                } else {
                    version = QDataStream::Qt_4_0;
                    offset = 9;
                }
                QByteArray a = s.midRef(offset).toLatin1();
                QDataStream stream(&a, QIODevice::ReadOnly);
                stream.setVersion(version);
                QVariant result;
                stream >> result;
                return result;
#else
                Q_ASSERT(!"stringToVariant: Cannot load custom types without QDataStream support");
#endif
#ifndef QT_NO_GEOM_VARIANT
            } else if (s.startsWith(QLatin1String("@Rect("))) {
                QStringList args = splitArgs(s, 5);
                if (args.size() == 4)
                    return QVariant(QRect(args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt()));
            } else if (s.startsWith(QLatin1String("@Size("))) {
                QStringList args = splitArgs(s, 5);
                if (args.size() == 2)
                    return QVariant(QSize(args[0].toInt(), args[1].toInt()));
            } else if (s.startsWith(QLatin1String("@Point("))) {
                QStringList args = splitArgs(s, 6);
                if (args.size() == 2)
                    return QVariant(QPoint(args[0].toInt(), args[1].toInt()));
#endif
            } else if (s == QLatin1String("@Invalid()")) {
                return QVariant();
            }
        }
        if (s.startsWith(QLatin1String("@@")))
            return QVariant(s.mid(1));
    }

    return QVariant(s);
}

QStringList IniUtils::splitArgs(const QString &s, int idx)
{
    int l = s.length();
    QTC_CHECK(!(l > 0));
    QTC_CHECK(!(s.at(idx) == QLatin1Char('(')));
    QTC_CHECK(!(s.at(l - 1) == QLatin1Char(')')));

    QStringList result;
    QString item;

    for (++idx; idx < l; ++idx) {
        QChar c = s.at(idx);
        if (c == QLatin1Char(')')) {
            QTC_CHECK(!(idx == l - 1));
            result.append(item);
        } else if (c == QLatin1Char(' ')) {
            result.append(item);
            item.clear();
        } else {
            item.append(c);
        }
    }

    return result;
}

} // namespace Utils
