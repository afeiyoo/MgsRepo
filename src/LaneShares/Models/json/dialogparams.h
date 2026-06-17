#pragma once

#include <QJsonObject>
#include <QJsonParseError>
#include <QString>

template<typename T>
class DialogParams
{
public:
    int api; // API编码
    T data;  // 要传输的参数

    static DialogParams<T> fromJson(const QByteArray &rawData)
    {
        DialogParams<T> response;
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(rawData, &error);

        if (error.error != QJsonParseError::NoError || !doc.isObject()) {
            response.api = -1;
            return response;
        }

        QJsonObject root = doc.object();
        response.api = root["api"].toInt();

        // 将data节点交给具体类型的 fromJson 处理
        if (root.contains("data")) {
            response.data = T::fromJson(root["data"].toObject());
        }
        return response;
    }

    static DialogParams<T> fromJson(const QJsonValue &value)
    {
        DialogParams<T> response;

        if (!value.isObject()) {
            response.api = -1;
            return response;
        }

        QJsonObject root = value.toObject();
        response.api = root["api"].toInt();

        // 将data节点交给具体类型的 fromJson 处理
        if (root.contains("data")) {
            response.data = T::fromJson(root["data"].toObject());
        }
        return response;
    }

    QJsonObject toJsonObj() const
    {
        QJsonObject root;

        root["api"] = api;

        // 将data节点交给具体类型的 toJson 处理
        root["data"] = data.toJson();

        return root;
    }

    QByteArray toJson(QJsonDocument::JsonFormat format = QJsonDocument::Compact) const { return QJsonDocument(toJsonObj()).toJson(format); }
};
