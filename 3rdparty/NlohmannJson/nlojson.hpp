#pragma once
#include "json.hpp"
#include <QByteArray>
#include <QIODevice>
#include <QReadWriteLock>
#include <QVariant>

using json = nlohmann::json;

class NloJson
{
public:
    NloJson() = default;
    ~NloJson() = default;

    // ---------------- Parser ----------------
    QVariant parse(const QByteArray &jsonData, bool *ok = nullptr)
    {
        try {
            json j = json::parse(jsonData.toStdString());
            if (ok)
                *ok = true;
            return jsonToVariant(j);
        } catch (const std::exception &e) {
            if (ok)
                *ok = false;
            // 写锁保护 m_lastError
            {
                QWriteLocker locker(&m_lock);
                m_lastError = e.what();
            }
            return QVariant();
        }
    }

    QVariant parse(QIODevice *io, bool *ok = nullptr)
    {
        if (!io->isOpen() && !io->open(QIODevice::ReadOnly)) {
            if (ok)
                *ok = false;
            QWriteLocker locker(&m_lock);
            m_lastError = "Failed to open QIODevice";
            return QVariant();
        }
        return parse(io->readAll(), ok);
    }

    QString errorString() const
    {
        QReadLocker locker(&m_lock);
        return m_lastError;
    }

    // ---------------- Serializer ----------------
    void serialize(const QVariant &variant, QIODevice *out, bool *ok)
    {
        QByteArray jsonStr = serialize(variant, ok);
        if (!ok || !*ok)
            return;

        QWriteLocker locker(&m_lock); // 串行化写设备
        if (!out->isOpen() && !out->open(QIODevice::WriteOnly)) {
            *ok = false;
            m_lastError = "Failed to open QIODevice for writing";
            return;
        }
        out->write(jsonStr);
    }

    QByteArray serialize(const QVariant &variant)
    {
        bool ok = false;
        return serialize(variant, &ok);
    }

    QByteArray serialize(const QVariant &variant, bool *ok)
    {
        try {
            json j = variantToJson(variant);
            if (ok)
                *ok = true;
            return QByteArray::fromStdString(j.dump(-1));
        } catch (const std::exception &e) {
            if (ok)
                *ok = false;
            QWriteLocker locker(&m_lock);
            m_lastError = e.what();
            return QByteArray();
        }
    }

private:
    mutable QReadWriteLock m_lock;
    QString m_lastError;

    // ---------------- JSON ↔ QVariant ----------------
    QVariant jsonToVariant(const json &j)
    {
        if (j.is_object()) {
            QVariantMap map;
            for (auto it = j.begin(); it != j.end(); ++it)
                map[QString::fromStdString(it.key())] = jsonToVariant(it.value());
            return map;
        } else if (j.is_array()) {
            QVariantList list;
            for (auto &v : j)
                list.append(jsonToVariant(v));
            return list;
        } else if (j.is_string()) {
            return QString::fromStdString(j.get<std::string>());
        } else if (j.is_boolean()) {
            return j.get<bool>();
        } else if (j.is_number_integer()) {
            return j.get<qint64>();
        } else if (j.is_number_float()) {
            return j.get<double>();
        } else if (j.is_null()) {
            return QVariant();
        }
        return QVariant();
    }

    json variantToJson(const QVariant &v)
    {
        switch (v.type()) {
        case QVariant::Map: {
            json j;
            QVariantMap map = v.toMap();
            for (auto it = map.begin(); it != map.end(); ++it)
                j[it.key().toStdString()] = variantToJson(it.value());
            return j;
        }
        case QVariant::List: {
            json j = json::array();
            for (auto item : v.toList())
                j.push_back(variantToJson(item));
            return j;
        }
        case QVariant::Bool:
            return v.toBool();
        case QVariant::Int:
        case QVariant::LongLong:
        case QVariant::UInt:
        case QVariant::ULongLong:
            return v.toLongLong();
        case QVariant::Double:
            return v.toDouble();
        case QVariant::String:
            return v.toString().toStdString();
        case QVariant::ByteArray: {
            QByteArray ba = v.toByteArray();
            return std::string(ba.constData(), ba.size());
        }
        default:
            return nullptr;
        }
    }
};
