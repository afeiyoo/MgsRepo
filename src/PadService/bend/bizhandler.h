#pragma once

#include <QObject>

#include "HttpClient/src/httpreply.h"
#include "bend/dataservice.h"
#include "global/defs.h"

class BizHandler : public QObject
{
    Q_OBJECT
public:
    explicit BizHandler(QObject *parent = nullptr);
    ~BizHandler() override;

    QString doMainDeal(int cmdType, const QVariantMap &dataMap, const QByteArray &reqBody);

private:
    // 3.3.1 稽核登录
    QString doDealCmd01(const QVariantMap &aMap);

    // 3.3.16 稽核补费打票
    QString doDealCmd16(const QVariantMap &aMap);

    // 3.3.18 路径展示
    QString doDealCmd18(const QVariantMap &aMap);
    // 通过车牌查询路径
    QString getVehicleWayFromPlate(const QString &plateNumber);
    // 通过二维码查询路径
    QString getVehicleWayFromScan(const QString &scan);
    // 计费路径请求报文组装
    QString packFeePath(const ST_PathFitting &pathFitting, int exitFeeType);
    // 节点信息报文组装
    QString packNode(const ST_Node &node);
    // 获取门架节点信息表
    void getGantryNodes(const QString &passID, QStringList &nodeList);
    // 获取门架节点分省信息
    void getGantryNodeSplitOut(const QString &sTradeID, QString &tollIntervalsGroup, QString &transTimeGroup);

    // 3.3.19 绿通查验
    QString doDealCmd19(const QVariantMap &aMap);
    // 通过车牌查询绿通车辆信息
    QString getGreenVehicleInfoFromPlate(const QString &plateNumber);
    // 通过交易号查询绿通车辆信息
    QString getGreenVehicleInfoFromTradeID(const QString &tradeId);
    // 通过二维码查询绿通车辆路径
    QString getGreenVehicleWayFromScan(const QString &scan);
    // 解析绿通二维码
    ST_GreenScanInfo getGreenPassScanInfo(const QString &scan);

    // 3.3.20 获取图片
    QString doDealCmd20(const QVariantMap &aMap);
    // 门架图片获取
    QString getGantryImage(const QString &plateNumber, const QString &gantryId, const QString &tradeTime);
    // 车道图片获取
    QString getLaneImage(const QString &captureId);
    // 集装箱图片获取
    QString getContainerImage(const QString &picName);
    // 稽核审核图片获取
    QString getDiscardTicketReviewImage(const QString &picName, const QString &stationId);

    // 3.3.23 设备控制
    QString doDealCmd23(const QVariantMap &aMap);

    // 3.3.24 车牌识别
    QString doDealCmd24(const QByteArray &reqBody);

    // 3.3.25 状态名单信息查询
    QString doDealCmd25(const QVariantMap &aMap);

    // 3.3.26 交接班数据
    QString doDealCmd26(const QVariantMap &aMap);

    // 3.3.27 特情记录
    QString doDealCmd27(const QVariantMap &aMap);

    // 3.3.28 查询工号
    QString doDealCmd28(const QVariantMap &aMap);

    // 3.3.30 查询逃费车辆欠费信息
    QString doDealCmd30(const QVariantMap &aMap);

    // 3.3.31 省内逃漏费车辆补费上传
    QString doDealCmd31(QVariantMap aMap);
    // 第三方支付请求
    QVariantMap cloudPay(const QString &tradeNum, const QVariantMap &aMap);
    // 核单请求
    QVariantMap getBillState(const QString &tradeNum);
    // 退款
    QVariantMap refund(const QString &tradeNum, const QVariantMap &aMap);
    // 生成唯一交易号
    int getUniqueTradeNum(const QString &stationId);
    // 第三方支付时，根据错误码获取错误信息
    QString getErrInfo(int errorCode);

    // 3.3.32 大件运输车预约信息查询
    QString doDealCmd32(const QVariantMap &aMap);

    // 3.3.33 集装箱预约信息查询
    QString doDealCmd33(const QVariantMap &aMap);
    // 保存集装箱图片到本地，并将图片的UUID返回给手持机
    void saveAndReplaceContainerPic(const QString &base64Data, QVariantMap &infoMap, const QString &key);

    // 3.3.34 临时免征车查询列表
    QString doDealCmd34(const QVariantMap &aMap);

    // 3.3.37 新增图片上传接口
    QString doDealCmd37(const QVariantMap &aMap);

    // 3.3.38 获取出口流水是否补打票信息
    QString doDealCmd38(const QVariantMap &aMap);
    // 开票信息查询，判断是否已经开过电子票，开过则返回不打票
    bool checkHasEInvoice(const QString &tradeId);

    // 3.3.39 获取出口绿通流水
    QString doDealCmd39(const QVariantMap &aMap);

    // 3.3.40 云坐席台账
    QString doDealCmd40(const QVariantMap &aMap);
    // 请求云坐席台账接口
    QString requestRemoteAPI(int type, const QString &id, const QString &data, const QString &token);

    // 3.3.41 权限管理
    QString doDealCmd41(const QVariantMap &aMap);

private:
    DataService m_ds;
    static QMap<QString, ST_AuditInfo> m_auditInfos;
};
