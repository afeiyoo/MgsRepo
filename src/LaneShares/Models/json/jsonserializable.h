#pragma once

#include <QJsonObject>
#include <QJsonValue>

class JsonSerializable
{
public:
    explicit JsonSerializable() {}
    ~JsonSerializable() {}

protected:
    // 检验字段是否存在且类型正确
    bool validateField(const QJsonObject &json, const QString &key, QJsonValue::Type expectedType)
    {
        if (!json.contains(key) || json[key].type() != expectedType)
            return false;

        return true;
    }
};
