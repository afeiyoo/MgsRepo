#include "config.h"

#include <QApplication>
#include <QSettings>

#include "utils/fileutils.h"

Config::Config(QObject *parent)
    : QObject{parent}
{
    // 日志配置
    m_logConfig.format = "%{time} [%{type}] [%{threadid}] %{message}\n\n";
    m_logConfig.filesLimit = 180;
}

Config::~Config() {}

void Config::loadConfig(const Utils::FileName &configPath)
{
    QString str = Utils::FileUtils::canonicalPath(configPath).toString();
    m_ini = new Utils::IniUtils(str);

    // 数据库配置
    m_dbConfig.type = m_ini->value("DataBase", "type", "QODBC").toString();
    m_dbConfig.driver = m_ini->value("DataBase", "driver", "Oracle 12c ODBC driver").toString();
    m_dbConfig.host = m_ini->value("DataBase", "host", "tcfindb").toString();
    m_dbConfig.user = m_ini->value("DataBase", "user", "tcfin").toString();
    m_dbConfig.dbName = m_ini->value("DataBase", "dbName", "tcfindb").toString();
    m_dbConfig.password = m_ini->value("DataBase", "password", "tc23fin").toString();
    m_dbConfig.port = m_ini->value("DataBase", "port", 3306).toUInt();

    // URL配置
    m_baseConfig.mapUrl = m_ini->value("BaseEnv", "mapUrl", "http://10.35.2.220:8080/api/gateway").toString();
    m_baseConfig.plateOcrUrl = m_ini->value("BaseEnv", "plateOcrUrl", "http://10.35.2.220:19585/visual/plate/plateOCR").toString();
    m_baseConfig.lanePicUrl = m_ini->value("BaseEnv", "lanePicUrl", "http://10.35.2.153:8899/api/dtp/picture").toString();
    m_baseConfig.gantryPicUrl = m_ini->value("BaseEnv", "gantryPicUrl", "http://10.35.2.125:8099/opmn/api/common/gantrypicbase64").toString();
    m_baseConfig.blackStatusUrl = m_ini->value("BaseEnv", "blackStatusUrl", "http://10.35.2.175:8090/tas/receive/blackquery").toString();
    m_baseConfig.multiBulkUrl = m_ini->value("BaseEnv", "multiBulkUrl", "http://10.35.2.153:8080/api/mutibulkvehicle").toString();
    m_baseConfig.containerConfirmUrl = m_ini->value("BaseEnv", "containerConfirmUrl", "http://10.35.2.153:8080/api/containerconfirm").toString();
    m_baseConfig.arrearsUrl = m_ini->value("BaseEnv", "arrearsUrl", "https://10.35.2.175:443/tas/openapi/owefee/detail").toString();
    m_baseConfig.payBackUrl = m_ini->value("BaseEnv", "payBackUrl", "https://10.35.2.175:443/tas/openapi/payback/").toString();
    m_baseConfig.cloudPayUrl = m_ini->value("BaseEnv", "cloudPayUrl", "").toString(); // http://35.16.1.75:18080/SPTWebService/api/trade/pay
    m_baseConfig.cloudPayKey = m_ini->value("BaseEnv", "cloudPayKey", "51c56b886b5be869567dd389b3e5d1d6").toString();
    m_baseConfig.billQueryUrl = m_ini->value("BaseEnv", "billQueryUrl", "http://35.16.1.75:18080/SPTWebService/api/trade/pay/billquery").toString();
    m_baseConfig.refundUrl = m_ini->value("BaseEnv", "refundUrl", "http://35.16.1.75:18080/SPTWebService/api/trade/pay/Refund").toString();
    m_baseConfig.remoteAPIUrl = m_ini->value("BaseEnv", "remoteAPIUrl", "http://10.35.2.136:6099/api/common").toString();
}
