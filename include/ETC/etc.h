#pragma once

#include <QMessageBox>
#include <QObject>

#include "etc_global.h"

class ETC_EXPORT ETC : public QObject
{
    Q_OBJECT
public:
    explicit ETC(QObject *parent = nullptr);
    ~ETC() override;

    int init(int argc, char *argv[]);

public slots:
    void onKeyPress(int key);                               // 前端按键响应
    void onShowFormResp(int api, const QJsonValue &values); // 前端弹窗响应

signals:
    // 顶部信息栏API
    void sigSetStationInfo(const QString &stationInfo);
    void sigSetUserInfo(const QString &userInfo);
    void sigSetLaneID(uint laneID);
    void sigSetShiftInfo(const QString &shiftInfo);
    void sigSetModeText(const QString &mode);

    // 底部状态栏API
    void sigSetFullBlackVer(const QString &ver);
    void sigSetPartBlackVer(const QString &ver);
    void sigSetVirtualGantryInfo(const QString &info);
    void sigSetAppVer(const QString &ver);
    void sigSetFeeRateVer(const QString &ver);

    // 抓拍显示区域API
    void sigSetCapImage(const QImage &img);

    // 滚动提示区域API
    void sigSetScrollTip(const QString &tip);

    // 日志显示区域API
    void sigLogAppend(uint logLevel, const QString &log);

    // 当前车辆与卡内信息显示区域API
    void sigSetPlate(const QString &plate);
    void sigSetVehClass(const QString &vehClass);
    void sigSetVehStatus(const QString &vehStatus);
    void sigSetSituation(const QString &situation);

    // 交易提示区域API
    void sigSetTradeHint(const QString &tradeHint, bool isWarn = false);
    void sigSetObuHint(const QString &obuHint, bool isWarn = false);

    // 近期交易记录查看区域API
    void sigAppendTradeItem(const QStringList &trade);
    void sigClearTradeItems();

    // 设备图标显示区域API
    void sigSetDeviceList(const QList<uint> &devList);
    void sigUpdateDeviceStatus(uint dev, uint status);

    // 工班信息显示区域API
    void sigSetTotalVehCnt(int cnt);
    void sigSetNormalVehCnt(int cnt);
    void sigSetFreeVehCnt(int cnt);
    void sigSetTotalToll(qreal fee);
    void sigSetCreditCardCnt(int cnt);
    void sigSetPrePayCardCnt(int cnt);
    void sigSetHolidayFreeVehCnt(int cnt);
    void sigSetPeccanyVehCnt(int cnt);
    void sigSetLastShiftTotalVehCnt(int cnt);

    // 当前车辆与卡内信息显示区域API
    void sigSetCardType(const QString &cardType);
    void sigSetProvince(const QString &province);
    void sigSetBalance(const QString &balance);
    void sigSetTradeTime(const QString &time);
    void sigSetEnStationName(const QString &enStationName);
    void sigSetToll(const QString &toll);

    // 窗口API
    void sigShowFormRequest(int formType, int api, const QJsonValue &values);
};
