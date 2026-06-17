#pragma once

#include "jsonserializable.h"

#include <QJsonArray>

class InfoDialogRequest : public JsonSerializable
{
    Q_GADGET
    Q_PROPERTY(QString title MEMBER title)
    Q_PROPERTY(QStringList strs MEMBER strs)
    Q_PROPERTY(bool switchLine MEMBER switchLine)
public:
    InfoDialogRequest() = default;
    ~InfoDialogRequest() = default;

    static InfoDialogRequest fromJson(const QJsonObject &obj)
    {
        InfoDialogRequest params;
        if (params.validateField(obj, "title", QJsonValue::String))
            params.title = obj["title"].toString();
        if (params.validateField(obj, "strs", QJsonValue::Array)) {
            QJsonArray arr = obj["strs"].toArray();
            for (const QJsonValue &v : arr) {
                params.strs << v.toString();
            }
        }
        if (params.validateField(obj, "switchLine", QJsonValue::Bool))
            params.switchLine = obj["switchLine"].toBool();

        return params;
    }

    QJsonObject toJson() const
    {
        QJsonObject obj;

        obj["title"] = title;
        obj["strs"] = QJsonArray::fromStringList(strs);
        obj["switchLine"] = switchLine;

        return obj;
    }

public:
    QString title;
    QStringList strs;
    bool switchLine = false;
};

class InfoDialogResponse : public JsonSerializable
{
    Q_GADGET
    Q_PROPERTY(bool yes MEMBER yes)
public:
    InfoDialogResponse() = default;
    ~InfoDialogResponse() = default;

    static InfoDialogResponse fromJson(const QJsonObject &obj)
    {
        InfoDialogResponse params;
        if (params.validateField(obj, "yes", QJsonValue::Bool))
            params.yes = obj["yes"].toBool();

        return params;
    }

    QJsonObject toJson() const
    {
        QJsonObject obj;

        obj["yes"] = yes;
        return obj;
    }

public:
    bool yes = false;
};
