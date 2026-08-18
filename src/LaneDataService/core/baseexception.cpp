#include "baseexception.h"

BaseException::BaseException(int code, const QString &desc)
    : m_code{code}
    , m_desc{desc}
{}

BaseException::~BaseException() {}

int BaseException::code() const
{
    return m_code;
}

QString BaseException::desc() const
{
    return m_desc;
}

const char *BaseException::what() const noexcept
{
    m_buf = m_desc.toUtf8();
    return m_buf.constData();
}
