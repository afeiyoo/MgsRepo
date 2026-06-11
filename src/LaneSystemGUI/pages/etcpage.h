#pragma once

#include "pages/basepage.h"

class EtcPage : public BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit EtcPage(QWidget *parent = nullptr);
    ~EtcPage() override;

public:
    // 工班信息显示区域API
    void setTotalVehCnt(int cnt);          // 总过车数
    void setNormalVehCnt(int cnt);         // 普通车数
    void setFreeVehCnt(int cnt);           // 免征车数
    void setTotalToll(qreal fee);          // 总收费
    void setCreditCardCnt(int cnt);        // 记账卡数
    void setPrePayCardCnt(int cnt);        // 储值卡数
    void setHolidayFreeVehCnt(int cnt);    // 动免车数
    void setPeccanyVehCnt(int cnt);        // 冲关车数
    void setLastShiftTotalVehCnt(int cnt); // 上个班次总车次

    // 当前车辆与卡内信息显示区域API
    void setCardType(const QString &cardType);           // 卡类型
    void setProvince(const QString &province);           // 省份
    void setBalance(const QString &balance);             // 卡内余额
    void setTradeTime(const QString &time);              // 交易时间
    void setEnStationName(const QString &enStationName); // 入口站名
    void setToll(const QString &toll);                   // 收费

protected:
    void initLeftUi() override;
    void initRightUi() override;

    QWidget *initLaneStatusArea() override;
    PageArea *initShiftInfoArea() override;
    PageArea *initVehInfoArea() override;
    RecentTradePanel *initRecentTradeArea() override;

private:
    // 交易信息显示区域
    QLabel *m_totalVehCnt = nullptr;          // 总过车数
    QLabel *m_normalVehCnt = nullptr;         // 普通车数
    QLabel *m_freeVehCnt = nullptr;           // 免征车数
    QLabel *m_totalToll = nullptr;            // 金额
    QLabel *m_creditCardCnt = nullptr;        // 记账卡数
    QLabel *m_prePayCardCnt = nullptr;        // 储值卡数
    QLabel *m_holidayFreeVehCnt = nullptr;    // 动免车数
    QLabel *m_peccanyVehCnt = nullptr;        // 冲关车数
    QLabel *m_lastShiftTotalVehCnt = nullptr; // 上个班次总车次

    // 当前车辆与卡内信息显示区域
    QLabel *m_cardType = nullptr;      // 卡类型
    QLabel *m_province = nullptr;      // 省份
    QLabel *m_balance = nullptr;       // 卡内余额
    QLabel *m_tradeTime = nullptr;     // 交易时间
    QLabel *m_enStationName = nullptr; // 入口站名
    QLabel *m_toll = nullptr;          // 收费
};
