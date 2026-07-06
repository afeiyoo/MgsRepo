#pragma once

#include <exception>
#include <QString>

class BaseException : public std::exception
{
public:
    // 异常类型枚举
    enum EM_ExceptionType { QUERY_REPEAT, SAVE_DATA, QUERY_XZPASS, QUERY_SHIFT, QUERY_DATA, QUERY_ETC_BLACK, NORMAL };

    explicit BaseException(EM_ExceptionType type, int errCode, const QString &errDesc);
    ~BaseException() override;

    EM_ExceptionType type() const;
    int errCode() const;
    QString errDesc() const;

    const char *what() const noexcept override;

private:
    // 异常类型
    EM_ExceptionType m_type;
    // 异常值
    int m_errCode;
    // 异常描述信息
    QString m_errDesc;
    mutable QByteArray m_buf;
};
