#include "dataservice.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "EasyQtSql.h"
#include "Logger.h"
#include "T_CollectLog.h"
#include "T_FF_CapRecord.h"
#include "T_LaneCapRecord.h"
#include "bend/environment.h"
#include "dao/configs/config.h"
#include "global/globalmanager.h"
#include "utils/bizutils.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"

using namespace EasyQtSql;
using namespace Utils;

DataService::DataService(QObject *parent)
    : QObject{parent}
{}

DataService::~DataService() {}

bool DataService::testConnection(const QString &connectionName, const QString &testSql) const
{
    if (!m_dbFactory) {
        LOG_CERROR("db").noquote() << "数据库连接初始化失败: SqlFactory为空";
        return false;
    }

    QSqlDatabase sdb = m_dbFactory->getDatabase(connectionName);
    if (!sdb.isValid()) {
        LOG_CERROR("db").noquote() << "数据库连接初始化失败: 无效的数据库连接";
        return false;
    }

    if (!sdb.isOpen()) {
        LOG_CERROR("db").noquote() << "数据库连接初始化失败:" << sdb.lastError().text();
        return false;
    }

    QSqlQuery query(sdb);
    if (!query.exec(testSql)) {
        LOG_CERROR("db").noquote() << "数据库连接探测失败:" << query.lastError().text() << "\t" << testSql;
        return false;
    }

    LOG_CINFO("db").noquote() << "数据库连接初始化成功:" << testSql;
    return true;
}

bool DataService::saveRecord(const QObject &obj) const
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("biz");

    QString sql = DataDealUtils::getInsertSql(const_cast<QObject *>(&obj));

    Transaction t(sdb);
    try {
        QVariantMap kvs = qObject2QVaiantMap(&obj);
        NonQueryResult res = t.insertInto(obj.metaObject()->className()).values(kvs).exec();

        LOG_CINFO("db").noquote() << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        return t.commit();
    } catch (const DBException &e) {
        t.rollback();
        LOG_CERROR("db").noquote() << e.lastError.text() << "\t" << e.lastQuery;
        return false;
    }
}

QVariantMap DataService::qObject2QVaiantMap(const QObject *obj) const
{
    //判空
    QVariantMap result;
    if (obj == nullptr)
        return result;

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QStringList autoIncCols = obj->property("auto_inc").toString().split(",", Qt::SkipEmptyParts);
#else
    QStringList autoIncCols = obj->property("auto_inc").toString().split(",", QString::SkipEmptyParts);
#endif
    for (QString &col : autoIncCols) {
        col = col.trimmed();
    }

    const QMetaObject *metaobject = obj->metaObject();
    int count = metaobject->propertyCount();
    int offset = metaobject->propertyOffset();
    for (int i = offset; i < count; ++i) {
        QMetaProperty metaproperty = metaobject->property(i);
        const char *name = metaproperty.name();

        if (!metaproperty.isReadable() || QLatin1String(name) == "objectName" || QLatin1String(name) == "tbl_pk" || QLatin1String(name) == "auto_inc"
            || autoIncCols.contains(QLatin1String(name)))
            continue;

        QVariant value = obj->property(name);
        result[QLatin1String(name)] = value;
    }
    return result;
}

int DataService::getUniqueSeq()
{
    uint today = DataDealUtils::curDateStr("yyyyMMdd").toUInt();

    if (m_curDate != today) {
        m_curDate = today;
        m_uniqueSeq = 0;
    }

    m_uniqueSeq = (m_uniqueSeq % 9999) + 1;

    return m_uniqueSeq;
}

QString DataService::getStationIP(const QString &stationID) const
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("biz");

    QString sql = QString(R"(SELECT IPADDR FROM T_STATION WHERE STATIONID = ? AND IPADDR NOT IN ('0000', '0.0.0.0');)");

    Transaction t(sdb);
    try {
        PreparedQuery query = t.prepare(sql);
        QueryResult res = query.exec(stationID);

        LOG_CINFO("db").noquote() << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return QStringLiteral("");

        QString data = res.value("IPADDR").toString();
        return data;
    } catch (const DBException &e) {
        LOG_CERROR("db").noquote() << e.lastError.text() << "\t" << e.lastQuery;
        return QStringLiteral("");
    }
}

bool DataService::isHolidayFreeVehClass(uint vehClass, bool checkVehClass) const
{
    QSqlDatabase sdb = m_dbFactory->getDatabase("biz");

    QString sql = QString(R"(SELECT COUNT(*) AS CNT FROM T_HOLIDAYFREE WHERE NOW() BETWEEN STARTTIME AND STOPTIME;)");
    if (checkVehClass) {
        sql += QString(" AND VEHCLASS = %1").arg(vehClass);
    }

    Transaction t(sdb);
    try {
        PreparedQuery query = t.prepare(sql);
        QueryResult res = query.exec();

        LOG_CINFO("db").noquote() << DataDealUtils::fullExecutedQuery(res.unwrappedQuery());

        if (!res.next())
            return false;

        int data = res.value("CNT").toInt();
        return data > 0;
    } catch (const DBException &e) {
        LOG_CERROR("db").noquote() << e.lastError.text() << "\t" << e.lastQuery;
        return false;
    }
}

void DataService::saveLaneCapRecord(const QString &vehPlate, const QString &capTradeID)
{
    T_LaneCapRecord record;

    record.StationID = GM_INSTANCE->m_conf->m_stationID;
    record.LaneID = GM_INSTANCE->m_conf->m_laneID;
    record.LaneType = GM_INSTANCE->m_conf->m_laneType;
    record.ShiftDate = GM_INSTANCE->m_env->m_shiftDate;
    record.ShiftID = GM_INSTANCE->m_env->m_shiftID;
    record.DataTime = DataDealUtils::curDateTime();
    record.TradeID = capTradeID;
    record.VehPlate = vehPlate;
    record.VehColor = BizUtils::getColorFromPlate(vehPlate);
    record.VehClass = 0;
    record.VehSpeed = 0;
    record.VehFeatureCode = "";
    record.FaceFeatureCode = "";
    record.CreateTime = DataDealUtils::curDateTime();
    record.Reserve = "";

    bool dbOk = saveRecord(record);
    if (!dbOk) {
        LOG_CERROR("db").noquote() << "保存T_LaneCapRecord失败 TradeID:" << capTradeID;
    }

    // 生成xml文件
    QString dtpContent = BizUtils::makeDtpContentFromObj(record);
    QString fromNode = GM_INSTANCE->m_conf->m_stationID.rightJustified(4, QChar('0'))
                       + QString::number(GM_INSTANCE->m_conf->m_laneID).rightJustified(2, QChar('0'));
    QString toNode = GM_INSTANCE->m_conf->m_stationID.rightJustified(4, QChar('0'));
    int recordCount = 1;
    QString dtpXml = BizUtils::makeDtpXml(dtpContent, "214", fromNode, toNode, recordCount);

    FileSaver saver(FileUtils::curApplicationDirPath() + "/upload/TradeQ_" + DataDealUtils::curDateTimeStr("yyyyMMddhhmmsszzz") + ".xml");
    bool fileOk = saver.finalize();
    if (!fileOk) {
        LOG_CERROR("db").noquote() << "生成" << saver.fileName() << "报文失败: " << saver.errorString();
    }
}

void DataService::saveFFCapRecord(const QString &captureID, const QString &vehPlate)
{
    T_FF_CapRecord record;

    record.CaptureID = captureID;
    record.FlagType = 0;
    record.FlagID = GM_INSTANCE->m_conf->m_psdFlagID;
    record.FlagName = GM_INSTANCE->m_conf->m_psdFlagName;
    record.FlagIndex = GM_INSTANCE->m_conf->m_laneID;
    record.CapTime = DataDealUtils::curDateTime();
    record.Shiftdate = GM_INSTANCE->m_env->m_shiftDate;
    record.ShiftID = DataDealUtils::curDateTimeStr("hh").toInt();
    record.CapType = 0;
    record.CapDevice = "统一高清";
    record.Channel = GM_INSTANCE->m_conf->m_laneID;
    record.VehPlate = vehPlate;
    record.PictureID = captureID;
    record.TradeID = "";
    record.VehClass = 0;
    record.VehBrand = "";
    record.Reserve1 = "";
    record.Reserve2 = "";

    bool dbOk = saveRecord(record);
    if (!dbOk) {
        LOG_CERROR("db").noquote() << "保存T_FF_CapRecord失败 CaptureID:" << captureID;
    }

    // TODO 生成json文件
}

void DataService::saveFFShiftStat() {}

bool DataService::checkOperator(const QString &ID, const QString &stationID) const
{
    if (ID.isEmpty())
        return false;

    QSqlDatabase sdb = m_dbFactory->getDatabase("biz");

    QString sql = QString(R"(SELECT COUNT(*) AS CNT FROM T_USER WHERE USERID=? AND ORGID=?;)");

    Transaction t(sdb);
    try {
        // TODO
        return true;
    } catch (const DBException &e) {
        return false;
    }
}

bool DataService::saveCollectLog(const QString &tradeID, const QString &log, int index)
{
    QString finalTradeID = tradeID;
    if (finalTradeID.isEmpty()) {
        finalTradeID = QString("NT%1%2").arg(GM_INSTANCE->m_conf->m_stationID).arg(DataDealUtils::curDateTimeStr("yyyyMMddhhmmsszzz"));
    }

    T_CollectLog record;

    record.TradeID = tradeID;
    record.LogIdx = index;
    record.LaneID = GM_INSTANCE->m_conf->m_laneID;
    record.ShiftDate = DataDealUtils::curDate();
    record.ShiftNum = GM_INSTANCE->m_env->m_shiftID;
    record.ShiftUser = GM_INSTANCE->m_env->m_operatorID.isEmpty() ? "999" : GM_INSTANCE->m_env->m_operatorID;
    record.LogTime = DataDealUtils::curDateTime();
    record.LogContent = log;

    bool dbOk = saveRecord(record);
    if (!dbOk) {
        LOG_CERROR("db").noquote() << "保存T_CollectLog失败 TradeID:" << tradeID;
    }

    // 生成xml文件
    QString dtpContent = BizUtils::makeDtpContentFromObj(record);
    QString fromNode = GM_INSTANCE->m_conf->m_stationID.rightJustified(4, QChar('0'))
                       + QString::number(GM_INSTANCE->m_conf->m_laneID).rightJustified(2, QChar('0'));
    QString toNode = GM_INSTANCE->m_conf->m_stationID.rightJustified(4, QChar('0'));
    int recordCount = 1;
    QString dtpXml = BizUtils::makeDtpXml(dtpContent, "210", fromNode, toNode, recordCount);

    FileSaver saver(FileUtils::curApplicationDirPath() + "/upload/MonitorQ_" + DataDealUtils::curDateTimeStr("MMddhhmmsszzz")
                    + QString("%1").arg(getUniqueSeq(), 4, 10, QChar('0')) + ".xml");
    bool fileOk = saver.finalize();
    if (!fileOk) {
        LOG_CERROR("db").noquote() << "生成" << saver.fileName() << "报文失败: " << saver.errorString();
    }

    return dbOk && fileOk;
}
