#pragma once

#include <exception>
#include <QString>

class BaseException : public std::exception
{
public:
    explicit BaseException(int status, const QString &desc);
    ~BaseException();

    int status() const;
    QString desc() const;

    const char *what() const noexcept override;

private:
    // 异常值
    int m_status;
    // 异常描述信息
    QString m_desc;
    mutable QByteArray m_buf;
};
