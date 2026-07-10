#include "baseexception.h"

BaseException::BaseException(int status, const QString &desc)
    : m_status{status}
    , m_desc{desc}
{}

BaseException::~BaseException() {}

int BaseException::status() const
{
    return m_status;
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
