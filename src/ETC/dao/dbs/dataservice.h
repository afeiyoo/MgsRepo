#pragma once

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

protected:
    bool testConnection(const QString &connectionName, const QString &testSql) const;
    bool saveRecord(const QObject &obj) const;

    EasyQtSql::SqlFactory *m_dbFactory = nullptr;

private:
    QVariantMap qObject2QVaiantMap(const QObject *obj) const;
};
