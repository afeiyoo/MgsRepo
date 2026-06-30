#include "dataservicedameng.h"

#include "EasyQtSql.h"

using namespace EasyQtSql;

DataServiceDameng::DataServiceDameng(QObject *parent)
    : DataService{parent}
{}

DataServiceDameng::~DataServiceDameng() {}

bool DataServiceDameng::init(const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName)
{
    SqlFactory::DBSetting setting("QODBC", host, port, userName, passWord, dbName);

    m_dbFactory = SqlFactory::getInstance()->config(setting);

    return testConnection();
}

QString DataServiceDameng::getTestSql() const
{
    return QString("SELECT 1 FROM DUAL;");
}

QString DataServiceDameng::getSuccessedTradesSql(int vehicleIdentifyType, QString vehPlate, QString cardID, int dataType, int judgeTime) const
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
        // 车牌 + 卡号
        if (isEntry) {
            etcWhere = QString("OBUPlate LIKE '%%1' AND CardID = '%2'").arg(vehPlate, cardID);
            mtcWhere = QString("EnVehPlate LIKE '%%1' AND CardID = '%2'").arg(vehPlate, cardID);
        } else {
            etcWhere = QString("(OBUPlate LIKE '%%1' OR ExVehPlate LIKE '%%1' OR CardPlate LIKE '%%1') AND CardID = '%2'").arg(vehPlate, cardID);
            mtcWhere = QString("(ExVehPlate LIKE '%%1' OR CardPlate LIKE '%%1')  AND CardID = '%2'").arg(vehPlate, cardID);
        }
    }

    const QString commonWhere = QString("%1 > SYSDATE - INTERVAL '%2' SECOND AND IsValid = 1 AND (%3 = TO_CHAR(SYSDATE, 'YYYY-MM-DD') OR %3 = "
                                        "TO_CHAR(SYSDATE - 1, 'YYYY-MM-DD'))")
                                    .arg(timeField)
                                    .arg(judgeTime)
                                    .arg(shiftField);

    QString sql = QString("SELECT TradeID, %1 AS LaneID, %2 AS TradeTime, %3 AS VehPlate, CardID, %4 AS Fee FROM %5 WHERE %6 AND %7 UNION ALL "
                          "SELECT TradeID, %1 AS LaneID, %2 AS TradeTime, %8 AS VehPlate, CardID, %4 AS Fee FROM %9 WHERE %10 AND %7;")
                      .arg(laneField, timeField, etcPlateField, feeExpr, etcTable, etcWhere, commonWhere, mtcPlateField, mtcTable, mtcWhere);

    return sql;
}

QString DataServiceDameng::getShiftCntSql(const QString &shiftDate, int shiftID, int laneID, int flag) const
{
    QString sql;
    if (flag == 1) {
        sql = QString("SELECT SUM(total) FROM (SELECT COUNT(*) AS total FROM t_mtc_in WHERE EnShiftDate = '%1' AND EnShiftID = %2 AND EnLane = %3 "
                      "AND IsValid = 1 UNION ALL SELECT COUNT(*) AS total FROM t_etc_in WHERE EnShiftDate = '%1' AND EnShiftID = %2 AND EnLane = %3 "
                      "AND IsValid = 1)")
                  .arg(shiftDate)
                  .arg(shiftID)
                  .arg(laneID);
    } else {
        sql = QString("SELECT SUM(total) FROM (SELECT COUNT(*) AS total FROM t_mtc_out WHERE ExShiftDate = '%1' AND ExShiftID = %2 AND ExLane = %3 "
                      "AND IsValid = 1 UNION ALL SELECT COUNT(*) AS total FROM t_etc_out WHERE ExShiftDate = '%1' AND ExShiftID = %2 AND ExLane = %3 "
                      "AND IsValid = 1)")
                  .arg(shiftDate)
                  .arg(shiftID)
                  .arg(laneID);
    }
    return sql;
}
