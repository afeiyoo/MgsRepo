#pragma once

#include "global/uidefs.h"
#include "utils/enums.h"

#include <QLabel>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QWidget>

class ElaToolButton;
class ElaPushButton;
class ElaImageCard;
class PageArea;
class ScrollText;
class IconButton;
class WeightInfoPanel;
class RecentTradePanel;
class DevicePanel;
class BasePage : public QWidget
{
    Q_OBJECT
public:
    explicit BasePage(QWidget *parent = nullptr);
    virtual ~BasePage() override;

    // 主显示区域初始化
    void initUi();

    // 顶部信息栏API
    void setStationInfo(const QString &stationInfo);
    void setUserInfo(const QString &userInfo);
    void setLaneID(uint laneID);
    void setShiftInfo(const QString &shiftInfo);
    void setModeText(const QString &mode);

    // 底部状态栏API（版本信息）
    void setFullBlackVer(const QString &ver);
    void setPartBlackVer(const QString &ver);
    void setVirtualGantryInfo(const QString &info);
    void setAppVer(const QString &ver);
    void setFeeRateVer(const QString &ver);

    // 抓拍显示区域API
    void setCapImage(const QImage &img);

    // 滚动提示显示区域API
    void setScrollTip(const QString &tip);

    // 日志显示区域API
    void logAppend(EM_LogLevel::LogLevel logLevel, const QString &log);

    // 当前车辆与卡内信息显示区域API
    virtual void setPlate(const QString &plate) = 0;                 // 抓拍车牌
    virtual void setVehClass(const QString &vehClass) = 0;           // 车型
    virtual void setVehStatus(const QString &vehStatus) = 0;         // 车种
    virtual void setSituation(const QString &situation) = 0;         // 特情
    virtual void setCardType(const QString &cardType) = 0;           // 卡类型
    virtual void setCardNum(const QString &cardNum) = 0;             // 卡号
    virtual void setBalance(const QString &balance) = 0;             // 卡内余额
    virtual void setEnTime(const QString &enTime) = 0;               // 入口时间
    virtual void setEnStationName(const QString &enStationName) = 0; // 入口站名
    virtual void setCardStatus(const QString &cardStatus) = 0;       // 卡状态
    virtual void setWeightInfo(const QString &info) = 0;             // 称重
    virtual void setEnPlate(const QString &plate) = 0;               // 入口车牌
    virtual void setLabel1(const QString &info) = 0;                 // 标签1
    virtual void setLabel2(const QString &info) = 0;                 // 标签2
    virtual void setToll(const QString &toll) = 0;                   // 收费

    // 交易提示区域API
    virtual void setTradeHint(const QString &tradeHint, const QString &color = "#000000") = 0;
    virtual void setObuHint(const QString &obuHint, const QString &color = "#0C4E94") = 0;
    virtual void appendHintButton(const QString &hint, const QString &fontColor = "#ffffff", const QString &bgColor = "#08c134") = 0;

    // 称重信息显示区域API
    void setCurWeightInfo(const QString &curWeightInfo, const QString &color = "#0C4E94");
    void setCurWeightStatus(uint status);
    void setCurWeightInfoCount(uint curWeightInfoCount);
    void appendWeightInfoItem(const ST_WeightInfoItem &item);
    void removeWeightInfoItem(uint index);
    void refreshWeightInfoItem(uint index, const ST_WeightInfoItem &item);
    void clearWeightInfoItem();
    void setWeightLow(bool isLow);

    // 近期交易记录查看区域API
    void appendTradeItem(const QStringList &trade);
    void clearTradeItems();

    // 设备图标显示区域API
    virtual void setDeviceList(const QList<uint> &devList) = 0;
    virtual void updateDeviceStatus(EM_DeviceIcon::DeviceIcon dev, uint status) = 0;

protected:
    // 抓拍显示区域初始化
    virtual QWidget *initDisplayArea() = 0;

    // 车道状态显示区域初始化
    QWidget *initLaneStatusArea();

    // 工班信息显示区域初始化（由子类各自实现）
    virtual PageArea *initShiftInfoArea() = 0;

    // 日志显示区域初始化
    PageArea *initLogBrowseArea();

    // 滚动提示区域初始化
    PageArea *initScrollTipArea();

    // 车辆及卡内信息区域初始化（由子类各自实现）
    virtual PageArea *initVehInfoArea() = 0;

    // 交易提示信息区域初始化（由子类各自实现）
    virtual PageArea *initTradeHintArea() = 0;

    // 称重信息区域初始化
    PageArea *initWeightInfoArea();

    // 近期交易记录查看区域初始化（由子类各自实现）
    virtual RecentTradePanel *initRecentTradeArea() = 0;

    // 设备状态栏区域初始化
    virtual QWidget *initDeviceArea() = 0;

    // 子类工具函数
    QWidget *createShiftInfoBlock(const QString &infoTitle, QLabel *numLabel, ushort width, ushort height, QWidget *parent = nullptr);

private:
    // 创建顶部信息栏
    void createTopWidget();
    // 添加主显示区
    void addCentralWidget(QWidget *w);
    // 创建底部状态栏
    void createBottomWidget();

    void initLeftUi();
    void initRightUi();

protected:
    // 抓拍显示区域
    ElaImageCard *m_capImage = nullptr;
    // TODO prevImage属于出口专属，所以成员变量放入出口页面中

private:
    // 顶部信息栏
    QWidget *m_topWidget = nullptr;
    QLabel *m_stationInfo = nullptr;     // 站信息
    QLabel *m_userInfo = nullptr;        // 用户信息
    QLabel *m_laneID = nullptr;          // 车道号
    QLabel *m_shiftInfo = nullptr;       // 班次信息
    ElaPushButton *m_modeText = nullptr; // 车道模式

    // 底部信息栏
    QWidget *m_bottomWidget = nullptr;
    QLabel *m_fullBlackVer = nullptr;      // 全量状态名单版本
    QLabel *m_partBlackVer = nullptr;      // 增量状态名单版本
    QLabel *m_virtualGantryInfo = nullptr; // 承载门架信息
    QLabel *m_appVer = nullptr;            // 软件版本
    QLabel *m_feeRateVer = nullptr;        // 费率版本

    // 主区域
    QWidget *m_centralWidget = nullptr;
    QWidget *m_leftWidget = nullptr;  //  左侧主布局
    QWidget *m_rightWidget = nullptr; // 右侧主布局

    // 抓拍显示区域
    QWidget *m_displayArea = nullptr;

    // 车道状态显示区域
    QWidget *m_laneStatusArea = nullptr;
    QLabel *m_vehMode = nullptr;
    QLabel *m_infoBoard = nullptr;

    // 工班信息显示区域
    PageArea *m_shiftInfoArea = nullptr;

    // 日志显示区域
    PageArea *m_logArea = nullptr;
    QPlainTextEdit *m_logBrowser = nullptr;
    QStringList m_logBuffer;

    // 滚动提示显示区域
    PageArea *m_scrollTipArea = nullptr;
    ScrollText *m_scrollTip = nullptr;

    // 当前车辆与卡内信息显示区域
    PageArea *m_vehInfoArea = nullptr;

    // 交易提示区域
    PageArea *m_tradeHintArea = nullptr;

    // 称重信息显示区域
    PageArea *m_weightInfoArea = nullptr;
    QLabel *m_curWeightInfo = nullptr;
    QLabel *m_curWeightInfoCount = nullptr;
    IconButton *m_weightLow = nullptr;
    WeightInfoPanel *m_weightInfoPanel = nullptr;

    // 近期交易记录查看区
    RecentTradePanel *m_recentTradePanel = nullptr;

    // 设备图标显示区域
    QWidget *m_deviceArea = nullptr;
};
