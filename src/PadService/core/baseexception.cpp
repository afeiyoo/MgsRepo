#include "baseexception.h"

BaseException::BaseException(int status, const QString &desc)
    : m_errCode{status}
    , m_desc{desc}
{}

BaseException::~BaseException() {}

int BaseException::errCode() const
{
    return m_errCode;
}

QString BaseException::errDesc() const
{
    return m_desc;
}

const char *BaseException::what() const noexcept
{
    m_buf = m_desc.toUtf8();
    return m_buf.constData();
}
