#pragma once

#include <QString>
#include <QStringList>

// 门架节点信息
struct ST_Node
{
    QString nodeId;
    QString nodeHex;
    QString passTime;
    QString nodeName;
    int nodeType;
    QString captureId;
};

// 路径拟合信息
struct ST_PathFitting
{
    QString plateNumber;
    uint plateColor;
    int mediaType;
    int vehicleType;  // 车型
    int vehicleClass; // 车种
    int receivableMoney;
    int tradingMoney;
    int discountsMoney;
    int axesCount;
    QString enNode;
    QString exNode;
    QStringList gantryUnits;
    QString tollIntervalIDs;
    QString tollIntervalIDsTime;
};

// 绿通二维码信息
struct ST_GreenScanInfo
{
    QString plateNumber;
    int plateColor;
    QString enStationId; // 入口收费站编号,参考收费站编号，14位
    QString exStationId;
    int enWeight; // 车货总重，单位：KG
    int exWeight;
    int mediaType;         // 通行介质,1位数字,1-OBU,2-CPC卡,3-纸券,9-无通行介质
    QString transactionId; // 出口交易编号=出口车道编号+交易批次号（YYYYMMDDHH）+批次内流水号（6位）
    QString passId;
    QString exTime;
    int transPayType; // 交易支付方式,1-出口ETC通行,2-出口ETC刷卡通行
    long fee;         // 总交易金额
    long payFee;      // 应收金额
    // 车辆状态标识,0x02-绿通车，0x03-联合收割机，0xFF-默认值,车辆在预约名单内，出口交易二维码生成该字段为0x02或0x03，否则为0xFF或255
    QString vehicleSign;
    int provinceNum;
};

// 补费对象
struct ST_AuditInfo
{
    bool isPreBlack;
    QString enStationId;
    QString exStationId;
    QString passId;
    QString escapeType;
    QString escapeTypeDesc;
    QString remark;
};

// 基础业务配置
struct ST_BaseConfig
{
    QString mapUrl;
    QString plateOcrUrl;
    QString lanePicUrl;
    QString gantryPicUrl;
    QString blackStatusUrl;
    QString multiBulkUrl;
    QString containerConfirmUrl;
    QString arrearsUrl;
    QString payBackUrl;
    QString cloudPayUrl;
    QString cloudPayKey;
    QString billQueryUrl;
    QString refundUrl;
    QString remoteAPIUrl;
    QString eInvoiceUrl;
};

// 数据库配置
struct ST_DBConfig
{
    QString type;
    QString driver;
    QString host;
    QString user;
    QString dbName;
    QString password;
    uint port;
};

// 日志配置
struct ST_LogConfig
{
    QString format;
    uint filesLimit;
};
