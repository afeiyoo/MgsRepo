#pragma once

#include <QObject>

#include "deploytool_global.h"

// 抓拍配置
struct ST_Capture
{
    bool isEnable = false;
    QString ip;          // 抓拍IP
    QString stationHex;  // 3501+站代码
    QString stationName; // 站名称
    int laneID = 0;      // 车道号
    int port = 0;
    int triggerMode = 0; // 1线圈触发，0视频触发
    int type = 0;
};

// 发卡机配置
struct ST_CardRobot
{
    bool isEnable = false;
    QString ip;
    QString port;              // 串口卡机-填串口号
    QString upReaderPort;      // 上读卡器串口
    QString dnReaderPort;      // 下读卡器串口
    QString readerApiFileName; // 读卡器动态库
    int producer = 0;          // 产家
    int readerSlot = 0;
    int readerApiType = 0;
    int baudRate = 0; // 串口卡机专用-波特率
    int option = 0;
    bool isShareReader = false; // 是否上下工位共享读卡器
};

// 字符叠加配置
struct ST_Overlap
{
    bool isEnable = false;
    QString comPort;
    int type = 0;
    int baudRate = 0;
};

// 称重配置
struct ST_Weight
{
    bool isEnable = false;
    QString comPort;
    int addr = 0;
    int baudRate = 0;
    int IWP = 0;
};

// 智能网关配置
struct ST_SmartLaneCtrl
{
    bool isEnable = false;
    QString ip;
    int port = 0;
};

// 费显配置
struct ST_FeeScr
{
    bool isEnable = false;
    QString port;
    int baudRate = 0;
    int sleepMillisecond = 0;
    int type = 0;
};

// 车型识别器配置
struct ST_VehRecognizer
{
    bool isEnable = false;
    QString ip;
    int port = 0;
};

// 缴费机配置
struct ST_PayRobot
{
    bool isEnable = false;
    QString ip;
    QString port;
    QString upReaderPort;      // 上读卡器串口
    QString dnReaderPort;      // 下读卡器串口
    QString readerApiFileName; // 读卡器动态库
    int producer = 0;          // 产家
    int readerSlot = 0;
    int readerApiType = 0;
    bool isShareReader = false; // 是否上下工位共享读卡器
};

// 车控器配置
struct ST_SwitchDev
{
    bool isEnable = true; // 车控器永远启用
    QString baseAddr;
    QString beginCoil;
    QString ceilingLamp;
    QString coil1;
    QString coil2;
    QString coil3;
    QString coil4;
    QString endCoil;
    QString passingLamp;
    QString railingDown1;
    QString railingUp1;
    QString warning;
    int type = 0;
    int which = 0;
    int coilNum = 0;
};

// 天线配置
struct ST_Rsu
{
    bool isEnable = false;
    QString ip;
    int power = 0;
    int port = 0;
};

// 桌面读卡器配置
struct ST_Reader
{
    bool isEnable = false;
    int slot = 0;
    int apiType = 0;
    QString comPort;
    QString apiFileName;
};

// 折叠情报板配置
struct ST_Infoboard
{
    bool isEnable = false;
    int type = 0;
    QString ip;
    int port = 0;
};

// 基础信息
struct ST_BaseInfo
{
    QString stationID;             // 站代码
    QString stationName;           // 站名称
    QString stationIP;             // 站级IP
    QString laneIP;                // 车道IP
    QString laneNetmask;           // 车道子网掩码
    QString laneGateway;           // 车道网关
    QString roadNetNo;             // 路网编号
    QString heartIP;               // 车道心跳上传IP
    QString flagHexNo;             // 承载门架编号
    QString flagName;              // 承载门架名称
    int laneType = 0;              // 车道类型
    int laneID = 0;                // 车道号
    bool isConvenientLane = false; // 是否后通道
};

// Redis信息
struct ST_Redis
{
    bool isEnable = false;
    QString ip;
    QString dbName;
    QString dbUser;
    QString password;
    int port = 0;
};

// 数据库信息
struct ST_DataBase
{
    bool isEnable = true; // 数据库永远启用
    QString ip = "127.0.0.1";
    QString dbName = "tolllanedb";
    QString dbUser = "tlman";
    QString dbPassword = "ds18fjeit";
    int dbPort = 3306;
    int dbType = 1;
};

// 部署信息
struct ST_DeployInfo
{
    // 基础信息
    ST_BaseInfo baseInfo;

    // 数据库配置信息
    ST_DataBase db;

    // Redis配置信息
    ST_Redis redis;

    // 桌面读卡器配置
    ST_Reader reader;

    // 天线配置信息
    ST_Rsu rsu;

    // 抓拍配置
    ST_Capture capture;

    // 是否自助缴费车道(用于界面UI配置，是否启用缴费机信息)
    bool isAutoPay = false;

    // 发卡机配置
    ST_CardRobot cardRobot;

    // 字符叠加器配置
    ST_Overlap overlap;

    // 称重配置
    ST_Weight weight;

    // 费显配置
    ST_FeeScr feeScr;

    // 智能网关配置
    ST_SmartLaneCtrl smartLaneCtrl;

    // 车控器配置
    ST_SwitchDev switchDev;

    // 小黄人配置
    ST_VehRecognizer recognizer;

    // 缴费机配置
    ST_PayRobot payRobot;

    // 情报板配置
    ST_Infoboard infoboard;
};

class DEPLOYTOOL_EXPORT IDeployTool : public QObject
{
    Q_OBJECT
public:
    explicit IDeployTool(QObject *parent = nullptr)
        : QObject(parent)
    {}

    virtual ~IDeployTool() = default;

    // 加载部署信息
    virtual bool loadDeployInfo(const QString &path, QString &errDesc) = 0;

    // 获取指定车道的部署信息
    virtual ST_DeployInfo getCurDeployInfo(const QString &stationID, int laneID) = 0;

    // 生成DeviceCtrl.json配置文件
    virtual bool saveDeviceCtrlFile(const ST_DeployInfo &info, const QString &path, QString &errDesc) = 0;

    // 生成LaneUI.json配置文件
    virtual bool saveLaneUIFile(const ST_DeployInfo &info, const QString &path, QString &errDesc) = 0;

    // 生成LaneBaseConfig.ini配置文件
    virtual bool saveLaneBaseConfigFile(const ST_DeployInfo &info, const QString &path, QString &errDesc) = 0;

    // 生成DtpAgent.cfg配置文件
    virtual bool saveDtpAgentFile(const ST_DeployInfo &info, const QString &fullBlackName, const QString &path, QString &errDesc) = 0;

    // 生成Start123.json配置文件
    virtual bool saveStartFile(const ST_DeployInfo &info, const QString &ver, const QString &path, QString &errDesc) = 0;

    // 同步数据库配置
    virtual bool syncDBConfig(const ST_DeployInfo &info, QString &errDesc) = 0;

    // 同步590费率文件
    virtual bool syncFeeRate(const QString &filePath, const QString &stationID, QString &errDesc) = 0;

    // 修改指定网卡的网络配置。返回true仅表示任务已受理，最终结果通过networkUpdateFinished()通知。
    virtual bool updateNetwork(const ST_DeployInfo &info, const QString &interfaceName, QString &errDesc) = 0;

    virtual bool isNetworkUpdating() const = 0;

signals:
    void sigNetworkUpdateStarted();
    void sigNetworkUpdateFinished(bool success, const QString &message);
};

extern "C" DEPLOYTOOL_EXPORT IDeployTool *createDeployTool();
extern "C" DEPLOYTOOL_EXPORT void destroyDeployTool(IDeployTool *tool);
