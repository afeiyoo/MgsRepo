#include "baseexception.h"

BaseException::BaseException(const QString &code, const QString &msg)
    : m_msg{msg}
    , m_code{code}
    , m_what{msg.toUtf8()}
{}

BaseException::~BaseException() {}

QString BaseException::code() const
{
    return m_code;
}

QString BaseException::msg() const
{
    QString info = QString::fromLocal8Bit("错误码(%1)").arg(m_code);
    return info;
}

const char *BaseException::what() const noexcept
{
    return m_what.constData();
}
