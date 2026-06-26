#include "dataservice.h"
#include "utils/stdafx.h"

#include "EasyQtSql.h"

DataService::DataService(QObject *parent)
    : QObject{parent}
{}

DataService::~DataService()
{
    SAFE_DELETE(m_dbFactory);
}
