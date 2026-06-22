#pragma once

#include <QDateTime>
#include <QLabel>
#include <QPlainTextEdit>
#include <QWidget>

#include "IPageController.h"

class ElaPushButton;
class ElaImageCard;
class PageArea;
class ScrollText;
class IconButton;
class WeightInfoPanel;
class RecentTradePanel;
class DevicePanel;
class AuthDialog;
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
    void logAppend(IPageController::EM_LogLevel logLevel, const QString &log);

    // 当前车辆与卡内信息显示区域API
    void setPlate(const QString &plate);         // 抓拍车牌
    void setVehClass(const QString &vehClass);   // 车型
    void setVehStatus(const QString &vehStatus); // 车种
    void setSituation(const QString &situation); // 特情

    // 交易提示区域API
    void setTradeHint(const QString &tradeHint, const QString &color = "#000000");
    void setObuHint(const QString &obuHint, const QString &color = "#0C4E94");

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
    void setDeviceList(const QList<uint> &devList);
    void updateDeviceStatus(IPageController::EM_DeviceIcon dev, uint status);

    // 窗口API
    void showAuthDialog(const QString &id, const QString &name);
    void showInfoDialog(const QString &title, const QStringList &strs, bool switchLine = true);

    void setApi(int newApi);

protected:
    virtual void initLeftUi() = 0;
    virtual void initRightUi() = 0;

    // 抓拍显示区域初始化（混合出口车道重写）
    virtual QWidget *initDisplayArea();

    // 车道状态显示区域初始化（ETC车道重写）
    virtual QWidget *initLaneStatusArea();

    // 工班信息显示区域初始化（由子类各自实现）
    virtual PageArea *initShiftInfoArea() = 0;

    // 日志显示区域初始化
    PageArea *initLogBrowseArea();

    // 滚动提示区域初始化
    PageArea *initScrollTipArea();

    // 车辆及卡内信息区域初始化（由子类各自实现）
    virtual PageArea *initVehInfoArea() = 0;

    // 交易提示信息区域初始化（混合出口车道重写）
    virtual PageArea *initTradeHintArea();

    // 称重信息区域初始化
    PageArea *initWeightInfoArea();

    // 近期交易记录查看区域初始化（由子类各自实现）
    virtual RecentTradePanel *initRecentTradeArea() = 0;

    // 设备区域初始化
    DevicePanel *initDeviceArea();

    // 子类工具函数
    QWidget *createShiftInfoBlock(const QString &infoTitle, ushort titleFontPx, QLabel *&numLabel, ushort labelFontPx, ushort width, ushort height,
                                  QWidget *parent);

    // 重写按键事件
    void keyPressEvent(QKeyEvent *event) override;

private:
    // 创建顶部信息栏
    void createTopWidget();
    // 添加主显示区
    void addCentralWidget(QWidget *w);
    // 创建底部状态栏
    void createBottomWidget();

protected:
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
    QWidget *m_leftWidget = nullptr;  //  左侧主区域
    QWidget *m_rightWidget = nullptr; // 右侧主区域

    // 抓拍显示区域
    QWidget *m_displayArea = nullptr;
    ElaImageCard *m_capImage = nullptr;

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
    QLabel *m_plate = nullptr;     // 车牌
    QLabel *m_vehClass = nullptr;  // 车型
    QLabel *m_vehStatus = nullptr; // 车种
    QLabel *m_situation = nullptr; // 特情

    // 交易提示区域
    PageArea *m_tradeHintArea = nullptr;
    QLabel *m_tradeHint = nullptr;
    QLabel *m_obuHint = nullptr;

    // 称重信息显示区域
    PageArea *m_weightInfoArea = nullptr;
    QLabel *m_curWeightInfo = nullptr;
    QLabel *m_curWeightInfoCount = nullptr;
    IconButton *m_weightLow = nullptr;
    WeightInfoPanel *m_weightInfoPanel = nullptr;

    // 近期交易记录查看区
    RecentTradePanel *m_recentTradePanel = nullptr;

    // 设备显示区域
    DevicePanel *m_deviceIconPanel = nullptr;

    // 窗口
    AuthDialog *m_authDlg = nullptr;

    // other变量
    QDateTime m_lastKeyPressTime;
    int m_lastKey;
    int m_api;
};
