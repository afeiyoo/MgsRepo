#pragma once

#include "pages/basepage.h"

#include <QLabel>
#include <QPushButton>

class PageArea;
class MtcInPage : public BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit MtcInPage(QWidget *parent = nullptr);
    ~MtcInPage() override;

    // 工班信息显示区域API
    void setTotalVehCnt(uint cnt);       // 总过车数
    void setTotalCardCnt(uint cnt);      // 总发卡数
    void setNoFlagCardCnt(uint cnt);     // 错标卡数
    void setPeccanyVehCnt(uint cnt);     // 冲关车数
    void setFleetVehCnt(uint cnt);       // 车队车数
    void setCpcCardCnt(uint cnt);        // 通行卡数
    void setEtcCardCnt(uint cnt);        // 闽通卡数
    void setBadCardCnt(uint cnt);        // 坏卡数
    void setPaperCardCnt(uint cnt);      // 发纸券数
    void setHolidayFreeVehCnt(uint cnt); // 动免车数
    void setRestTickCnt(uint cnt);       // 剩余券数
    void setStartTicketNum(uint num);    // 起始券号
    void setEndTicketNum(uint num);      // 结束券号
    void setCurTicketNum(uint num);      // 当前券号

    // 当前车辆与卡内信息显示区域API
    void setCardType(const QString &cardType);           // 卡类型
    void setCardNum(const QString &cardNum);             // 卡号
    void setBalance(const QString &balance);             // 卡内余额
    void setEnTime(const QString &enTime);               // 入口时间
    void setEnStationName(const QString &enStationName); // 入口站名
    void setCardStatus(const QString &cardStatus);       // 卡状态

    // 卡机状态区域API
    void setRobotStatus1(bool isOk, ushort restCard);
    void setRobotStatus2(bool isOk, ushort restCard);
    void setRobotStatus3(bool isOk, ushort restCard);
    void setRobotStatus4(bool isOk, ushort restCard);

protected:
    void initLeftUi() override;
    void initRightUi() override;

    PageArea *initShiftInfoArea() override;
    PageArea *initVehInfoArea() override;
    RecentTradePanel *initRecentTradeArea() override;
    QWidget *initRobotStatusArea();

private:
    // 工班信息显示区域
    QLabel *m_totalVehCnt = nullptr;       // 总过车数
    QLabel *m_totalCardCnt = nullptr;      // 总发卡数
    QLabel *m_noFlagCardCnt = nullptr;     // 错标卡数
    QLabel *m_peccanyVehCnt = nullptr;     // 冲关车数
    QLabel *m_fleetVehCnt = nullptr;       // 车队车数
    QLabel *m_cpcCardCnt = nullptr;        // 通行卡数
    QLabel *m_etcCardCnt = nullptr;        // 闽通卡数
    QLabel *m_badCardCnt = nullptr;        // 坏卡数
    QLabel *m_paperCardCnt = nullptr;      // 发纸券数
    QLabel *m_holidayFreeVehCnt = nullptr; // 动免车数
    QLabel *m_restTicketCnt = nullptr;     // 剩余券数
    QLabel *m_startTicketNum = nullptr;    // 起始券号
    QLabel *m_endTicketNum = nullptr;      // 结束券号
    QLabel *m_curTicketNum = nullptr;      // 当前券号
    PageArea *m_paperInfoArea = nullptr;   // 纸券信息区域

    // 当前车辆与卡内信息显示区域
    QLabel *m_cardType = nullptr;      // 卡类型
    QLabel *m_cardNum = nullptr;       // 卡号
    QLabel *m_balance = nullptr;       // 卡内余额
    QLabel *m_enTime = nullptr;        // 入口时间
    QLabel *m_enStationName = nullptr; // 入口站名
    QLabel *m_cardStatus = nullptr;    // 卡状态

    // 卡机状态区域
    QWidget *m_robotStatusArea = nullptr;
    QPushButton *m_robotStatus1 = nullptr;
    QPushButton *m_robotStatus2 = nullptr;
    QPushButton *m_robotStatus3 = nullptr;
    QPushButton *m_robotStatus4 = nullptr;
};
