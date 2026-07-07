#include "dataservice.h"

#include "EasyQtSql.h"
#include "Logger.h"
#include "config/config.h"
#include "core/globalmanager.h"
#include "sqldealer.h"
#include "utils/fileutils.h"
#include "utils/stdafx.h"

using namespace EasyQtSql;
using namespace Utils;

DataService::DataService()
{
    m_sqlDealer = new SqlDealer();
}

DataService::~DataService()
{
    SAFE_DELETE(m_sqlDealer);
}

bool DataService::init(uint type, const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName)
{
    // 加载sql文件
    FileUtils::makeSureDirExist(FileName::fromString(GM_INS->m_conf->m_sqlFilesDir));
    bool sqlOk = m_sqlDealer->loadSqlFiles(GM_INS->m_conf->m_sqlFilesDir);
    if (!sqlOk)
        return false;

    // 建立连接
    SqlFactory::DBSetting setting;
    if (type == 1) {
        setting = SqlFactory::DBSetting("QMYSQL", host, port, userName, passWord, dbName);
    } else {
        setting = SqlFactory::DBSetting("QODBC", host, port, userName, passWord, dbName);
    }
    m_dbFactory = SqlFactory::getInstance()->config(setting);
    return testConnection();
}

bool DataService::testConnection()
{
    if (!m_dbFactory) {
        LOG_ERROR().noquote() << "数据库连接初始化失败: SqlFactory为空";
        return false;
    }

    QSqlDatabase sdb = m_dbFactory->getDatabase();
    if (!sdb.isValid()) {
        LOG_ERROR().noquote() << "数据库连接初始化失败: 无效的数据库连接";
        return false;
    }

    if (!sdb.isOpen()) {
        LOG_ERROR().noquote() << "数据库连接初始化失败:" << sdb.lastError().text();
        return false;
    }

    QSqlQuery query(sdb);
    QString testSql = getTestSql();
    if (!query.exec(testSql)) {
        LOG_ERROR().noquote() << "数据库连接初始化失败:" << query.lastError().text() << "\t" << testSql;
        return false;
    }

    LOG_INFO().noquote() << "数据库连接初始化成功";
    return true;
}

QString DataService::getTestSql() const
{
    return QString("SELECT 1;");
}
