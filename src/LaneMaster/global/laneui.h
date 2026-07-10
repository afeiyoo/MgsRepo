#pragma once

#include <QObject>

class BasePage;
class LaneUI : public QObject
{
    Q_OBJECT
public:
    explicit LaneUI(BasePage *page, QObject *parent = nullptr);
    ~LaneUI() override;

    // 顶部信息栏API
    Q_INVOKABLE QByteArray setStationInfo(const QByteArray &json);

    // 工班信息区域显示API
    Q_INVOKABLE QByteArray setTotalVehCnt(const QByteArray &json);
    Q_INVOKABLE QByteArray setLastShiftTotalVehCnt(const QByteArray &json);

private:
    BasePage *m_page = nullptr;
};
