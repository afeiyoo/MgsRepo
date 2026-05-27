#pragma once

#include "pages/basepage.h"

#include <QLabel>

class PageArea;
class MtcInPage : public BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit MtcInPage(QWidget *parent = nullptr);
    ~MtcInPage() override;

    // 当前车辆与卡内信息显示区域API
    void setPlate(const QString &plate) override;
    void setVehClass(const QString &vehClass) override;
    void setVehStatus(const QString &vehStatus) override;
    void setSituation(const QString &situation) override;
    void setCardType(const QString &cardType) override;           // 卡类型
    void setCardNum(const QString &cardNum) override;             // 卡号
    void setBalance(const QString &balance) override;             // 卡内余额
    void setEnTime(const QString &enTime) override;               // 入口时间
    void setEnStationName(const QString &enStationName) override; // 入口站名
    void setCardStatus(const QString &cardStatus) override;       // 卡状态
    void setWeightInfo(const QString &info) override;             // 称重
    void setEnPlate(const QString &plate) override;               // 入口车牌
    void setLabel1(const QString &info) override;                 // 标签1
    void setLabel2(const QString &info) override;                 // 标签2
    void setToll(const QString &toll) override;                   // 收费

    // 交易提示区域API
    void setTradeHint(const QString &tradeHint, const QString &color = "#000000") override;
    void setObuHint(const QString &obuHint, const QString &color = "#0C4E94") override;
    void appendHintButton(const QString &hint, const QString &fontColor = "#FFFFFF", const QString &bgColor = "#08C134") override;

    // 设备图标显示区域API
    void setDeviceList(const QList<uint> &devList) override;
    void updateDeviceStatus(EM_DeviceIcon::DeviceIcon dev, uint status) override;

protected:
    QWidget *initDisplayArea() override;
    PageArea *initShiftInfoArea() override;
    PageArea *initVehInfoArea() override;
    PageArea *initTradeHintArea() override;
    RecentTradePanel *initRecentTradeArea() override;
    QWidget *initDeviceArea() override;

private:
    // 交易信息显示区域
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

    // 当前车辆与卡内信息显示区域
    QLabel *m_plate = nullptr;         // 车牌
    QLabel *m_vehClass = nullptr;      // 车型
    QLabel *m_vehStatus = nullptr;     // 车种
    QLabel *m_situation = nullptr;     // 特情
    QLabel *m_cardType = nullptr;      // 卡类型
    QLabel *m_cardNum = nullptr;       // 卡号
    QLabel *m_balance = nullptr;       // 卡内余额
    QLabel *m_enTime = nullptr;        // 入口时间
    QLabel *m_enStationName = nullptr; // 入口站名
    QLabel *m_cardStatus = nullptr;    // 卡状态

    // 交易提示区域
    QLabel *m_tradeHint = nullptr;
    QLabel *m_obuHint = nullptr;

    // 设备图标显示区域
    DevicePanel *m_deviceIconPanel = nullptr;
};
