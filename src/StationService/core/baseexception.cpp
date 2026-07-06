#include "baseexception.h"

BaseException::BaseException(EM_ExceptionType type, int errCode, const QString &errDesc)
    : m_type{type}
    , m_errCode{errCode}
    , m_errDesc{errDesc}
{}

BaseException::~BaseException() {}

BaseException::EM_ExceptionType BaseException::type() const
{
    return m_type;
}

int BaseException::errCode() const
{
    return m_errCode;
}

QString BaseException::errDesc() const
{
    return m_errDesc;
}

const char *BaseException::what() const noexcept
{
    m_buf = m_errDesc.toUtf8();
    return m_buf.constData();
}
