#pragma once

#include <exception>
#include <QString>

class BaseException : public std::exception
{
public:
    explicit BaseException(int code, const QString &desc);
    ~BaseException();

    int code() const;
    QString desc() const;

    const char *what() const noexcept override;

private:
    // 异常值
    int m_code;
    // 异常描述信息
    QString m_desc;
    mutable QByteArray m_buf;
};
