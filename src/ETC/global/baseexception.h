#pragma once

#include <exception>
#include <QString>

class BaseException : public std::exception
{
public:
    explicit BaseException(const QString &code, const QString &msg);
    ~BaseException() override;

    QString code() const; // 返回错误码
    QString msg() const;  // 返回错误信息

    const char *what() const noexcept override;

private:
    QString m_msg;
    QString m_code;
    QByteArray m_what;
};
