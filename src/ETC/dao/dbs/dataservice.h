#pragma once

#include <QDate>
#include <QObject>

namespace EasyQtSql {
class SqlFactory;
}

class DataService : public QObject
{
    Q_OBJECT
public:
    explicit DataService(QObject *parent = nullptr);
    ~DataService() override;

    virtual bool init(const QString &host, int port, const QString &userName, const QString &passWord, const QString &dbName) = 0;

    // 获取收费站IP
    QString getStationIP(const QString &stationID) const;

    // 检查是否节假日免费车型
    bool isHolidayFreeVehClass(uint vehClass, bool checkVehClass = false) const;

    // 保存车道抓拍记录
    void saveLaneCapRecord(const QString &vehPlate, const QString &capTradeID);

    // 保存承载门架抓拍记录
    void saveFFCapRecord(const QString &captureID, const QString &vehPlate);

    // 保存承载门架工班记录
    void saveFFShiftStat();

    // 检查是否本站工号
    bool checkOperator(const QString &ID, const QString &stationID) const;

    // 保存collectLog
    bool saveCollectLog(const QString &tradeID, const QString &log, int index);

protected:
    bool testConnection(const QString &connectionName, const QString &testSql) const;
    bool saveRecord(const QObject &obj) const;

protected:
    EasyQtSql::SqlFactory *m_dbFactory = nullptr;

private:
    // 数据对象转QVariantMap
    QVariantMap qObject2QVaiantMap(const QObject *obj) const;

    // 获取0-9999循环序列号
    int getUniqueSeq();

private:
    uint m_uniqueSeq = 0; // 序列号
    uint m_curDate = 0;
};
