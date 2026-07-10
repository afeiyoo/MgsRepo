#pragma once

#include "pages/basepage.h"

#include <QGroupBox>
#include <QStackedLayout>

class MtcOutPage : public BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit MtcOutPage(QWidget *parent = nullptr);
    ~MtcOutPage() override;

    // 抓拍显示区域API
    void setPrevImage(const QImage &img);

    // 工班信息显示区域API
    void enableSptShiftInfoShow(bool isSptShiftInfo); // 是否启用SPT工班信息显示
    void setStartTicketNum(int num);                  // 起始票据
    void setCurTicketNum(int num);                    // 当前票据
    void setNormalTicketCnt(int cnt);                 // 正常票数
    void setScrapTicketCnt(int cnt);                  // 废票张数
    void setRestTicketCnt(int cnt);                   // 剩余票据
    void setRecycleCardCnt(int cnt);                  // 回收卡数
    void setEtcCardCnt(int cnt);                      // 闽通卡数
    void setFleetVehCnt(int cnt);                     // 车队车数
    void setThirdPayCnt(int cnt);                     // 第三方支付
    void setUpStartTicketNum(int num);                // 上工位起始票据
    void setUpCurTicketNum(int num);                  // 上工位当前票据
    void setUpRestTicketCnt(int cnt);                 // 上工位剩余票据
    void setUpRecycleCardCnt(int cnt);                // 上工位回收票
    void setUpScrapTicketCnt(int cnt);                // 上工位废票数
    void setUpWriteErrCnt(int cnt);                   // 上工位写卡错误
    void setDownStartTicketNum(int num);              // 下工位起始票据
    void setDownCurTicketNum(int num);                // 下工位当前票据
    void setDownRestTicketCnt(int cnt);               // 下工位剩余票据
    void setDownRecycleCardCnt(int cnt);              // 下工位回收票
    void setDownScrapTicketCnt(int cnt);              // 下工位废票数
    void setDownWriteErrCnt(int cnt);                 // 下工位写卡错误

    // 当前车辆与卡内信息显示区域API
    void setCardType(const QString &cardType);           // 卡类型
    void setCardNum(const QString &cardNum);             // 卡号
    void setEnTime(const QString &enTime);               // 入口时间
    void setEnStationName(const QString &enStationName); // 入口站名
    void setWeightInfo(const QString &info);             // 称重
    void setEnPlate(const QString &plate);               // 入口车牌
    void setLabel1(const QString &info);                 // 标签1
    void setSplitProvincesInfo(const QString &info);     // 分省信息

    // 交易提示区域API
    void appendHintButton(const QString &hint, const QString &bgColor = "#08c134");

protected:
    void initLeftUi() override;
    void initRightUi() override;

    QWidget *initDisplayArea() override;
    PageArea *initShiftInfoArea() override;
    PageArea *initVehInfoArea() override;
    PageArea *initTradeHintArea() override;
    RecentTradePanel *initRecentTradeArea() override;

    void resizeEvent(QResizeEvent *event) override;

private:
    void updatePrevImageSize();
    void initNormalShiftInfoArea(PageArea *parent);
    void initSptShiftInfoArea(PageArea *parent);

private:
    // 抓拍显示区域
    ElaImageCard *m_prevImage = nullptr;

    // 工班信息显示区域
    bool m_isSptShiftInfo = false;
    QLabel *m_startTicketNum = nullptr;       // 起始票据
    QLabel *m_curTicketNum = nullptr;         // 当前票据
    QLabel *m_normalTicketCnt = nullptr;      // 正常票数
    QLabel *m_scrapTicketCnt = nullptr;       // 废票张数
    QLabel *m_restTicketCnt = nullptr;        // 剩余票据
    QLabel *m_recycleCardCnt = nullptr;       // 回收卡数
    QLabel *m_etcCardCnt = nullptr;           // 闽通卡数
    QLabel *m_fleetVehCnt = nullptr;          // 车队车数
    QLabel *m_thirdPayCnt = nullptr;          // 第三方
    QGroupBox *m_upStationInfoArea = nullptr; // 上工位信息
    QLabel *m_upStartTicketNum = nullptr;
    QLabel *m_upCurTicketNum = nullptr;
    QLabel *m_upRestTicketCnt = nullptr;
    QLabel *m_upRecycleCardCnt = nullptr;
    QLabel *m_upScrapTicketCnt = nullptr;
    QLabel *m_upWriteErrCnt = nullptr;
    QGroupBox *m_downStationInfoArea = nullptr; // 下工位信息
    QLabel *m_downStartTicketNum = nullptr;
    QLabel *m_downCurTicketNum = nullptr;
    QLabel *m_downRestTicketCnt = nullptr;
    QLabel *m_downRecycleCardCnt = nullptr;
    QLabel *m_downScrapTicketCnt = nullptr;
    QLabel *m_downWriteErrCnt = nullptr;

    // 当前车辆与卡内信息显示区域
    QLabel *m_cardType = nullptr;           // 卡类型
    QLabel *m_cardNum = nullptr;            // 卡号
    QLabel *m_enStationName = nullptr;      // 入口站名
    QLabel *m_enTime = nullptr;             // 入口时间
    QLabel *m_weightInfo = nullptr;         // 称重信息
    QLabel *m_enPlate = nullptr;            // 入口车牌
    QLabel *m_label1 = nullptr;             // 标签1
    QLabel *m_splitProvincesInfo = nullptr; // 标签2

    // 交易提示区域
    QWidget *m_hintButtonArea = nullptr; // 按钮提示（第三方支付，微信支付等）
};
