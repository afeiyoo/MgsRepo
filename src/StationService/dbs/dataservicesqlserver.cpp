#include "dataservicesqlserver.h"

#include "EasyQtSql.h"

using namespace EasyQtSql;

DataServiceSqlServer::DataServiceSqlServer(QObject *parent)
    : DataService{parent}
{}

DataServiceSqlServer::~DataServiceSqlServer() {}

bool DataServiceSqlServer::init(const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName)
{
    SqlFactory::DBSetting setting("QODBC", host, port, userName, passWord, dbName);

    m_dbFactory = SqlFactory::getInstance()->config(setting);

    return testConnection();
}

QString DataServiceSqlServer::getTestSql() const
{
    return QString("SELECT 1;");
}

QString DataServiceSqlServer::getSuccessedTradesSql(int vehicleIdentifyType, QString vehPlate, QString cardID, int dataType, int judgeTime) const
{
    const bool isEntry = (dataType == 0);

    const QString timeField = isEntry ? "EnTime" : "ExTime";
    const QString shiftField = isEntry ? "EnShiftDate" : "ExShiftDate";
    const QString laneField = isEntry ? "EnLane" : "ExLane";
    const QString etcTable = isEntry ? "t_etc_in" : "t_etc_out";
    const QString mtcTable = isEntry ? "t_mtc_in" : "t_mtc_out";
    const QString etcPlateField = "OBUPlate";
    const QString mtcPlateField = isEntry ? "EnVehPlate" : "ExVehPlate";
    const QString feeExpr = isEntry ? "0" : "rebatedpay * 100";

    QString etcWhere;
    QString mtcWhere;

    if (vehicleIdentifyType == 1) {
        // 按卡号
        etcWhere = QString("CardID = '%1'").arg(cardID);
        mtcWhere = QString("CardID = '%1'").arg(cardID);
    } else if (vehicleIdentifyType == 2) {
        // 按车牌
        if (isEntry) {
            etcWhere = QString("OBUPlate LIKE '%%1'").arg(vehPlate);
            mtcWhere = QString("EnVehPlate LIKE '%%1'").arg(vehPlate);
        } else {
            etcWhere = QString("(OBUPlate LIKE '%%1' OR ExVehPlate LIKE '%%1' OR CardPlate LIKE '%%1')").arg(vehPlate);
            mtcWhere = QString("(ExVehPlate LIKE '%%1' OR CardPlate LIKE '%%1')").arg(vehPlate);
        }
    } else {
        // 按车牌 + 卡号
        if (isEntry) {
            etcWhere = QString("OBUPlate LIKE '%%1' AND CardID = '%2'").arg(vehPlate, cardID);
            mtcWhere = QString("EnVehPlate LIKE '%%1' AND CardID = '%2'").arg(vehPlate, cardID);
        } else {
            etcWhere = QString("(OBUPlate LIKE '%%1' OR ExVehPlate LIKE '%%1' OR CardPlate LIKE '%%1') AND CardID = '%2'").arg(vehPlate, cardID);
            mtcWhere = QString("(ExVehPlate LIKE '%%1' OR CardPlate LIKE '%%1') AND CardID = '%2'").arg(vehPlate, cardID);
        }
    }

    const QString commonWhere = QString("%1 > DATEADD(SECOND, -%2, GETDATE()) AND IsValid = 1 AND (%3 = CONVERT(varchar(10), GETDATE(), 121) "
                                        "OR %3 = CONVERT(varchar(10), DATEADD(DAY, -1, GETDATE()), 121))")
                                    .arg(timeField)
                                    .arg(judgeTime)
                                    .arg(shiftField);

    QString sql = QString("SELECT TradeID, %1 AS LaneID, %2 AS TradeTime, %3 AS VehPlate, CardID, %4 AS Fee "
                          "FROM %5 WHERE %6 AND %7 UNION ALL SELECT TradeID, %1 AS LaneID, %2 AS TradeTime, "
                          "%8 AS VehPlate, CardID, %4 AS Fee FROM %9 WHERE %10 AND %7;")
                      .arg(laneField, timeField, etcPlateField, feeExpr, etcTable, etcWhere, commonWhere, mtcPlateField, mtcTable, mtcWhere);

    return sql;
}
