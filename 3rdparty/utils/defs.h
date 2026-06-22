#pragma once

#include <QDateTime>
#include <QMap>
#include <QObject>
#include <QVariant>

//==============================================================================
// 设备数据
//==============================================================================
// 抓拍数据
struct ST_CapVehInfo
{
    QByteArray chHWYIP; //抓拍IP
    QByteArray chPlate; //车牌
    int nColor = 0;     //颜色
    QByteArray chSpecialImage;
    int nSpecialLen = 0;
    QByteArray chFullImage;
    int nFullLen = 0;
    QByteArray chPlateImage;
    int nPlateLen = 0;
    QByteArray chTwoImage;
    int nTwoLen = 0;
    uint cameraTime = 0;      //Unix
    uchar chVehicleColor;     //车身颜色
    uchar chVehicleClass = 0; //车型
    QByteArray reserve;       //备用
};
Q_DECLARE_METATYPE(ST_CapVehInfo)

//==============================================================================
// 软件配置
//==============================================================================
namespace ConfigDef {
// 数据库配置
struct ST_DatabaseCfg
{
    uint type; // 数据库类型
    QString ip;
    uint port;
    QString dbName;
    QString user;
    QString password;
};

// 后端服务配置
struct ST_RpcServerCfg
{
    uint mode;
    uint socketType; // 连接类型 0-tcp, 1-websocket
    uint port;
};

// 车道系统基础配置
struct ST_SystemCfg
{
    int serviceMode;
    int serviceSocketType;
    QString serviceIP;
    int servicePort;
    int versionCheckTime;
    int keyboardType;
    QString updateUrl;
};

// 车道系统业务配置
struct ST_BusinessCfg
{
    // 车道类型:1.混合入口 2.混合出口 3.ETC入口 4.ETC出口 5.匝道
    int laneType;
    QString netID;
    QString stationID;
    QString stationName;
    int laneID;
    QString showDevs;     // 设备图标显示，用-分割，图标数值参考EM_DeviceIcon
    QString virEnstation; // 虚拟入口站
    QString laneIP;       // 车道IP
    QString flagID;       // 门架编号
    QString CNLaneID;     // 注册车道编号
    int heartBeatTime;    // 心跳上传时间间隔
    bool doubleLane;      // 复式票亭
};

// 读卡器配置
struct ST_CardReaderCfg
{
    int type = 0;    // 0 串口读卡器 1 卡代理(考虑以后自己部署读卡服务)
    int slotNo;      // 卡槽
    QString apiName; // 调用厂商动态库名称
    int workNo = 0;  // 读卡器工位
    int index = 0;   // 0 主票亭 1 另一个票亭 默认0
    int apiType;     // 省标和国标
    QString comPort; // 串口号
    int baudRate;    // 波特率
    QString ip;      // IP地址
    int port;        // 端口
    bool isValid = false;
};

// 打印机配置
struct ST_PrinterCfg
{
    int type;        //打印机类型 1 ds并 2 star 并  3 ds串  4 star串
    QString comPort; //串口号
    int baudRate;    //波特率
    int baseAddr;    //基地址
    bool isValid = false;
};

// 自助发卡机配置，配置为空自助入口时使用
struct ST_CardRobotCfg
{
    int readerCnt = 0;
    ST_CardReaderCfg reader[4]; //读卡器
    int type;                   //自助机类型 0 TCP  1 串口
    int option;                 //是否1分4
    int palletDelayTime;        //托盘延时伸出时间,单位毫秒
    QString comPort;            //串口号
    int baudRate;               //波特率
    QString ip;                 //IP地址
    int port;                   //端口
    bool isValid = false;
};

// 自助缴费终端配置
struct ST_PaymentRobotCfg
{
    int readerCnt = 0;
    ST_CardReaderCfg reader[4]; //读卡器
    int palletDelayTime;        //拖盘延时伸出时间，单位毫秒
    QString ip;                 //IP地址
    int port;                   //端口
    bool isValid = false;
};

// 抓拍配置
struct ST_CapCfg
{
    int type;   //抓拍类型
    QString ip; //IP地址
    int port;   //端口
    bool isValid = false;
};

//车道控制器
struct ST_LaneCtrlCfg
{
    int baseAddr;         //基地址
    QString ceilingLamp;  //顶棚灯
    QString passingLamp;  //通行灯
    QString warning;      //声光报警
    QString railingUp1;   //升杆1
    QString railingDown1; //落杆1
    QString railingUp2;   //升杆2
    QString railingDown2; //落杆2
    QString beginCoil;    //抓拍线圈
    QString endCoil;      //落杆线圈
    int coilNum;          //线圈数
    QString coil1;
    QString coil2;
    QString coil3;
    QString coil4;
    QString coil5;
    QString coil6;
    int which;
    int type;
    bool isValid = false;
};

//一体化栏杆机/费显
struct ST_FeeScrCfg
{
    int type; //费显类型 1 新大陆 2 英沙 3 华工 4 化工恒富威 5 MTC紫凯 6 ETC紫凯 7 艾特斯 8 MGSKJ 9 MGSKJ2 10 HZH 11 HZH 4X8 15 一体化栏杆机
    int sleepMillisecond; //发送间隔 单位毫秒
    QString comPort;      //串口号
    int baudRate;         //波特率
    QString ip;           //IP地址
    int port;             //端口
    bool isValid = false;
};

//字符叠加器
struct ST_OverLapCfg
{
    int type;        //类型
    QString comPort; //串口号
    int baudRate;    //波特率
    bool isValid = false;
};

// 称重配置
struct ST_WeightCfg
{
    int addr;        //称重地址
    int IWP;         //是否整车秤
    QString comPort; //串口号
    int baudRate;    //波特率
    bool isValid = false;
};

// 情报板配置
struct ST_InfoBoardCfg
{
    int type;
    int index;
    QString ip; //IP地址
    int port;   //端口
    bool isValid = false;
};

// 智能车控器配置
struct ST_SmartLaneControllerCfg
{
    QString ip; //IP地址
    int port;   //端口
    bool isValid = false;
};

// RSU配置
struct ST_RsuCfg
{
    int index;
    QString ip;       //IP地址
    int port;         //端口
    int waitTime;     //交易等待时间，默认255
    int txPower;      //天线功率，默认31
    int pllChannelID; //信道号，默认0
    bool isValid = false;
};

//小黄人配置
struct ST_VehRecoCfg
{
    QString ip; //IP地址
    int port;   //端口
    bool isValid = false;
};

// 收费节点
struct ST_TollNode
{
    int index; //节点编号
    ST_PaymentRobotCfg paymentRobotCfg;
    ST_CardRobotCfg cardRobotCfg;
    ST_FeeScrCfg feeScrCfg;
    ST_CapCfg capCfg;
};

struct ST_DevCfg
{
    QMap<int, ST_TollNode> tollNodeCfgMap;
    QMap<int, ST_RsuCfg> rsuCfgMap;             //天线配置 index为天线编号，其中0为近天线 1为远天线
    QMap<int, ST_InfoBoardCfg> infoBoardCfgMap; //情报板 index为情报板编号

    int tollNodeCnt = 1;  //收费节点数，默认1
    int rsuCnt = 1;       //rsu数量，默认1
    int infoBoardCnt = 1; //情报板数量，默认1
    ST_PrinterCfg deskPrinterCfg;
    ST_CardReaderCfg deskCardReader;                  //桌面读卡器
    ST_WeightCfg weightCfg;                           //称重
    ST_OverLapCfg overLapCfg;                         //字符叠加
    ST_SmartLaneControllerCfg smartLaneControllerCfg; //车道智能网关
    ST_LaneCtrlCfg laneCtrlCfg;                       //车控器
    ST_VehRecoCfg vehRecoCfg;                         //小黄人
};
} // namespace ConfigDef

//==============================================================================
// 收费介质
//==============================================================================
namespace Media {
//读取PSAM卡结果
class ReadPsamInfoResult : public QObject
{
    Q_OBJECT
public:
    int ErrorCode = 0; //0-成功，1-失败，2-初始化读卡器失败，3-读卡器打开失败，4-获取PSAM卡号/终端机编号失败 5-PSAM卡不支持国密
    int WorkNo = 0; //工位号	0：桌面1：上工位，2：下工位
    QString TermNo = "";
    QString PsamNo = "";
    int IsNeedAuth = 0; //1:需要授权
    QString Random = "";
    QString ErrorDesc = "";

    ReadPsamInfoResult() {}
    ~ReadPsamInfoResult() override {}

public:
    Q_PROPERTY(int ErrorCode MEMBER ErrorCode)
    Q_PROPERTY(int WorkNo MEMBER WorkNo)
    Q_PROPERTY(QString TermNo MEMBER TermNo)
    Q_PROPERTY(QString PsamNo MEMBER PsamNo)
    Q_PROPERTY(int IsNeedAuth MEMBER IsNeedAuth)
    Q_PROPERTY(QString Random MEMBER Random)
    Q_PROPERTY(QString ErrorDesc MEMBER ErrorDesc)
};

//授权result
class PsamAuthResult : public QObject
{
    Q_OBJECT
public:
    int WorkNo = 0;
    int ErrorCode = 0; //0-成功，1-失败，2-读卡器初始化失败
    QString ErrorDesc = "";
    QString AuthResult = ""; //响应码0x9000
    PsamAuthResult() {}
    ~PsamAuthResult() override {}

public:
    Q_PROPERTY(int WorkNo MEMBER WorkNo)
    Q_PROPERTY(int ErrorCode MEMBER ErrorCode)
    Q_PROPERTY(QString AuthResult MEMBER AuthResult)
    Q_PROPERTY(QString ErrorDesc MEMBER ErrorDesc)
};

//写卡Result
class WriteCardResult : public QObject
{
    Q_OBJECT
public:
    int ErrorCode = 0; //0-成功，1:PSAM卡需要授权 2-其他原因导致的失败
    QString ErrorDesc = "";
    int WorkNo = 0;
    QVariant CappData;

    WriteCardResult() {}
    ~WriteCardResult() override {}

public:
    Q_PROPERTY(int ErrorCode MEMBER ErrorCode)
    Q_PROPERTY(QString ErrorDesc MEMBER ErrorDesc)
    Q_PROPERTY(int WorkNo MEMBER WorkNo)
    Q_PROPERTY(QVariant CappData MEMBER CappData)
};

class CappData : public QObject
{
    Q_OBJECT
public:
    // 成员变量：类型适配Qt（UINT/USHORT→int，char数组→QString，BYTE→int）
    uint ConsumeMoney = 0;    // 交易金额（UINT→int）
    QString ConsumeTime = ""; // 交易时间（char[15]→QString，格式yyyymmddhhnnss）
    QString hexTermNo = "";   // 终端机号（6B，char[13]→QString）
    uint RemainMoney = 0;     // 交易前余额（UINT→int）
    ushort TradeNo = 0;       // 脱机交易序号（USHORT→int）
    uint Overdraw = 0;        // 透支限额（3B UINT→int）
    int KeyVer = 0;           // 密钥版本（BYTE→int）
    int AlgoFlag = 0;         // 算法标识（BYTE→int）
    uint RandNum = 0;         // 随机数（UINT→int）
    uint UserTradeNo = 0;     // 用户脱机交易序号（UINT→int）
    uint MAC1 = 0;            // MAC1（UINT→int）
    uint TAC = 0;             // TAC码（UINT→int）
    uint MAC2 = 0;            // MAC2（UINT→int）

    // 构造/析构函数
    CappData() {}
    ~CappData() override {}

public:
    // Q_PROPERTY：暴露属性供Qt元对象系统使用
    Q_PROPERTY(uint ConsumeMoney MEMBER ConsumeMoney)
    Q_PROPERTY(QString ConsumeTime MEMBER ConsumeTime)
    Q_PROPERTY(QString hexTermNo MEMBER hexTermNo)
    Q_PROPERTY(uint RemainMoney MEMBER RemainMoney)
    Q_PROPERTY(ushort TradeNo MEMBER TradeNo)
    Q_PROPERTY(uint Overdraw MEMBER Overdraw)
    Q_PROPERTY(int KeyVer MEMBER KeyVer)
    Q_PROPERTY(int AlgoFlag MEMBER AlgoFlag)
    Q_PROPERTY(uint RandNum MEMBER RandNum)
    Q_PROPERTY(uint UserTradeNo MEMBER UserTradeNo)
    Q_PROPERTY(uint MAC1 MEMBER MAC1)
    Q_PROPERTY(uint TAC MEMBER TAC)
    Q_PROPERTY(uint MAC2 MEMBER MAC2)
};

class CPCSysInfo : public QObject
{
    Q_OBJECT
public:
    QString Issue = "";
    QString CPCID = "";
    int Version = 0;
    QString StartTime = "";
    QString StopTime = "";
    QString Reserve = "";

    CPCSysInfo() {}
    ~CPCSysInfo() override {}

public:
    Q_PROPERTY(QString Issue MEMBER Issue)
    Q_PROPERTY(QString CPCID MEMBER CPCID)
    Q_PROPERTY(int Version MEMBER Version)
    Q_PROPERTY(QString StartTime MEMBER StartTime)
    Q_PROPERTY(QString StopTime MEMBER StopTime)
    Q_PROPERTY(QString Reserve MEMBER Reserve)
};

class CPCBaseInfo : public QObject
{
    Q_OBJECT
public:
    int Elec = 0;         //电量信息最高位0正常 1低电 其它7bit为百分比
    int WorkStatus = 0;   //5.8G工作状态
    QString Reserve = ""; //厂商自定义62B

    CPCBaseInfo() {}
    ~CPCBaseInfo() override {}

public:
    Q_PROPERTY(int Elec MEMBER Elec)
    Q_PROPERTY(int WorkStatus MEMBER WorkStatus)
    Q_PROPERTY(QString Reserve MEMBER Reserve)
};

class CPCEnExInfo : public QObject
{
    Q_OBJECT
public:
    int VehClass = 0;         //车型1B(入口自动发卡时上工位填3,下工位填1)
    QString VehPlate = "";    //车牌12B
    QChar PlateColor = 0;     //车牌色
    QString RoadNetNo_h = ""; //路网号
    QString Station_h = "";   //入出口站
    int LaneID = 0;           //入出口车道1B
    uint EnExTime = 0;        //入出口时间
    int WorkStatus = 0;       //5.8G模块工作状态控制字节01打开02关闭(填2,只读)
    int EnExFlag = 0;         //进出站标志(只读,偏移24出口从此处开始写)
    int VehStatus = 0;        //车种(入口不明时填0,
    int Operator = 0;         //入口收费员工号(3B!!)
    int ShiftID = 0;          //入口班次
    int TruckAxises = 0;      //货车轴数
    int TotalWeight = 0;      //货车总轴重(入口不明时填0,出口时被覆盖)
    int LimitWeight = 0;      //货车轴限(3B!!,入口不明时填0,出口时被覆盖)
    int SpecialTruck = 0;     //特殊货车信息(入口不明时填0,出口时被覆盖)
    QString Reserve = "";     //备用字段

    CPCEnExInfo() {}
    ~CPCEnExInfo() override {}

public:
    Q_PROPERTY(int VehClass MEMBER VehClass)
    Q_PROPERTY(QString VehPlate MEMBER VehPlate)
    Q_PROPERTY(QChar PlateColor MEMBER PlateColor)
    Q_PROPERTY(QString RoadNetNo_h MEMBER RoadNetNo_h)
    Q_PROPERTY(QString Station_h MEMBER Station_h)
    Q_PROPERTY(int LaneID MEMBER LaneID)
    Q_PROPERTY(uint EnExTime MEMBER EnExTime)
    Q_PROPERTY(int WorkStatus MEMBER WorkStatus)
    Q_PROPERTY(int EnExFlag MEMBER EnExFlag)
    Q_PROPERTY(int VehStatus MEMBER VehStatus)
    Q_PROPERTY(int Operator MEMBER Operator)
    Q_PROPERTY(int ShiftID MEMBER ShiftID)
    Q_PROPERTY(int TruckAxises MEMBER TruckAxises)
    Q_PROPERTY(int TotalWeight MEMBER TotalWeight)
    Q_PROPERTY(int LimitWeight MEMBER LimitWeight)
    Q_PROPERTY(int SpecialTruck MEMBER SpecialTruck)
    Q_PROPERTY(QString Reserve MEMBER Reserve)
};

//CPC过站信息
class CPCPassInfo : public QObject
{
    Q_OBJECT
public:
    int ProvinceNum = 0;            //通行省份个数
    int LocalFlags = 0;             //本省通行门架数
    uint LocalFeeSum = 0;           //本省累计计费金额(3B)
    uint LocalMileAge = 0;          //本省累计计费里程(3B)
    QString EnHexFlag = "";         //本省第一个入口门架编号(3B),入口站不承载门架时清0
    uint EnFlagTime = 0;            //本省第一个入口门架通行时间(unix时间)
    QString LastHexFlag = "";       //最新门架编号
    uint LastFlagTime = 0;          //最新门架通行时间
    uint LastFlagFee = 0;           //最新门架计费金额(3B)
    uint LastMileAge = 0;           //最新门架计费里程(3B)
    QStringList LocalHexFlags = {}; //最多24个门架

    CPCPassInfo() {}
    ~CPCPassInfo() override {}

public:
    Q_PROPERTY(int ProvinceNum MEMBER ProvinceNum)
    Q_PROPERTY(int LocalFlags MEMBER LocalFlags)
    Q_PROPERTY(uint LocalFeeSum MEMBER LocalFeeSum)
    Q_PROPERTY(uint LocalMileAge MEMBER LocalMileAge)
    Q_PROPERTY(QString EnHexFlag MEMBER EnHexFlag)
    Q_PROPERTY(uint EnFlagTime MEMBER EnFlagTime)
    Q_PROPERTY(QString LastHexFlag MEMBER LastHexFlag)
    Q_PROPERTY(uint LastFlagTime MEMBER LastFlagTime)
    Q_PROPERTY(uint LastFlagFee MEMBER LastFlagFee)
    Q_PROPERTY(uint LastMileAge MEMBER LastMileAge)
    Q_PROPERTY(QStringList LocalHexFlags MEMBER LocalHexFlags)
};

//计费信息(DF01-EF04)
class CPCFeeCode : public QObject
{
    Q_OBJECT
public:
    int Province = 0;         //1 字节省份行政区域编码
    int PassFlags = 0;        //1 字节本省ETC 门架通行个数
    uint FeeSum = 0;          //3 字节本省累计计费金额
    uint MileAge = 0;         //3 字节本省累计计费里程（米）
    QString EnHexFlag = "";   //3 字节本省入口门架信息
    uint EnFlagTime = 0;      //4 字节本省入口门架通行时间UNIX 时间）
    QString LastHexFlag = ""; //3 字节最新ETC 门架编码
    uint LastFlagTime = 0;    //4 字节最新通行时间
    int FittingStatus = 0;    //1 字节最新ETC 门架拟合成功/失败标识（00 成功 01 失败）

    CPCFeeCode() {}
    ~CPCFeeCode() override {}

public:
    Q_PROPERTY(int Province MEMBER Province)
    Q_PROPERTY(int PassFlags MEMBER PassFlags)
    Q_PROPERTY(uint FeeSum MEMBER FeeSum)
    Q_PROPERTY(uint MileAge MEMBER MileAge)
    Q_PROPERTY(QString EnHexFlag MEMBER EnHexFlag)
    Q_PROPERTY(uint EnFlagTime MEMBER EnFlagTime)
    Q_PROPERTY(QString LastHexFlag MEMBER LastHexFlag)
    Q_PROPERTY(uint LastFlagTime MEMBER LastFlagTime)
    Q_PROPERTY(int FittingStatus MEMBER FittingStatus)
};

class CPCFeeInfo : public QObject
{
    Q_OBJECT
public:
    int Records = 0; //已写入计费信息数
    QVariantList CPCFeeCode = {};

    CPCFeeInfo() {}
    ~CPCFeeInfo() override {}

public:
    Q_PROPERTY(int Records MEMBER Records)
    Q_PROPERTY(QVariantList CPCFeeCode MEMBER CPCFeeCode)
};

//ETC卡(闽通卡)基本信息(0015+000C+0002)，此结构和天线共用
class CCBaseData : public QObject
{
    Q_OBJECT
public:
    QString hexPubOrg = "";    //福建3501001(8B)
    int CardType = 0;          //卡类型
    int CardVer = 0;           //卡版本号
    QString hexCardNet = "";   //卡网络编号(2B)(福建为0x2301)
    QString hexCardNo = "";    //卡内部编号(8B)
    QString hexStartDate = ""; //启用日期(4B)
    QString hexStopDate = "";  //停用日期(4B)
    QString VehPlate = "";     //车牌号(12B+2B中文颜色)
    int VehClass = 0;          //车型(0015文件,新版本增加)
    int UserType = 0;          //用户类型00-普通用户
    bool IsBinding = false;    //是否绑定车牌(不记名卡)
    bool IsFree = false;       //是否免费车
    bool IsTruck = false;      //是否货车
    int Seats = 0;             //座位数
    uint RemainMoney = 0;      //余额

    CCBaseData() {}
    ~CCBaseData() override {}

public:
    Q_PROPERTY(QString hexPubOrg MEMBER hexPubOrg)
    Q_PROPERTY(int CardType MEMBER CardType)
    Q_PROPERTY(int CardVer MEMBER CardVer)
    Q_PROPERTY(QString hexCardNet MEMBER hexCardNet)
    Q_PROPERTY(QString hexCardNo MEMBER hexCardNo)
    Q_PROPERTY(QString hexStartDate MEMBER hexStartDate)
    Q_PROPERTY(QString hexStopDate MEMBER hexStopDate)
    Q_PROPERTY(QString VehPlate MEMBER VehPlate)
    Q_PROPERTY(int VehClass MEMBER VehClass)
    Q_PROPERTY(int UserType MEMBER UserType)
    Q_PROPERTY(bool IsBinding MEMBER IsBinding)
    Q_PROPERTY(bool IsFree MEMBER IsFree)
    Q_PROPERTY(bool IsTruck MEMBER IsTruck)
    Q_PROPERTY(int Seats MEMBER Seats)
    Q_PROPERTY(uint RemainMoney MEMBER RemainMoney)
};

//ETC卡(闽通卡)过车数据(0019)，此结构和天线共用
class CCTollData : public QObject
{
    Q_OBJECT
public:
    int CappType = 0;         //复合应用类型，标准取值0xAA
    int RecLen = 0;           //记录长度，标准取值0x29
    int LockFlag = 0;         //应用锁定标记 0x00
    QString EnStation_h = ""; //入口站
    int EnLane = 0;           //出入口车道[**共用字段]
    uint EnTime = 0;          //入口时间(**新版本使用unix时间)
    int VehClass = 0;         //车型
    int CardStatus = 0;       //入出口状态3-入口 4出口
    uint EnOperator = 0;      //出入口收费员工号(3B)[**共用字段]
    QString VehPlate = "";    //出入口抓拍车牌14+1[**共用字段]
    int PlateColor = 0;       //车牌颜色(**新增字段 )
    QString ExStation_h = ""; //出口站
    uint ExTime = 0;          //出口时间(**新版本使用unix时间)
    QString ChkPoint = "";    //出口标识站(6B,**新版无用字段)
    uint Reserve = 0;         //备用(**新版无用字段)
    uint FlagID = 0;          //ETC门架编号(3字节**新增)
    uint FlagPassTime = 0;    //通行门架时间(Unix时间**新增)
    QString RoadNetNo_h;      //收费路网编号[**新增]
    int ShiftID = 0;          //入出口班次[**新增]
    int Version = 0;          //新旧版本号[**新增,0.旧版本 0x4F表示新版本]
    QString Remark = "";      //收费公路ETC预留[**新增]
    int AxisCount = 0;
    uint TollWeight = 0;
    int VehicleStatus = 0;
    uint TollFee = 0; //卡累计金额(2020.03)

    CCTollData() {}
    ~CCTollData() override {}

public:
    Q_PROPERTY(int CappType MEMBER CappType)
    Q_PROPERTY(int RecLen MEMBER RecLen)
    Q_PROPERTY(int LockFlag MEMBER LockFlag)
    Q_PROPERTY(QString EnStation_h MEMBER EnStation_h)
    Q_PROPERTY(int EnLane MEMBER EnLane)
    Q_PROPERTY(uint EnTime MEMBER EnTime)
    Q_PROPERTY(int VehClass MEMBER VehClass)
    Q_PROPERTY(int CardStatus MEMBER CardStatus)
    Q_PROPERTY(uint EnOperator MEMBER EnOperator)
    Q_PROPERTY(QString VehPlate MEMBER VehPlate)
    Q_PROPERTY(int PlateColor MEMBER PlateColor)
    Q_PROPERTY(QString ExStation_h MEMBER ExStation_h)
    Q_PROPERTY(uint ExTime MEMBER ExTime)
    Q_PROPERTY(QString ChkPoint MEMBER ChkPoint)
    Q_PROPERTY(uint Reserve MEMBER Reserve)
    Q_PROPERTY(uint FlagID MEMBER FlagID)
    Q_PROPERTY(uint FlagPassTime MEMBER FlagPassTime)
    Q_PROPERTY(QString RoadNetNo_h MEMBER RoadNetNo_h)
    Q_PROPERTY(int ShiftID MEMBER ShiftID)
    Q_PROPERTY(int Version MEMBER Version)
    Q_PROPERTY(QString Remark MEMBER Remark)
    Q_PROPERTY(int AxisCount MEMBER AxisCount)
    Q_PROPERTY(uint TollWeight MEMBER TollWeight)
    Q_PROPERTY(int VehicleStatus MEMBER VehicleStatus)
    Q_PROPERTY(uint TollFee MEMBER TollFee)
};

//处理后的读/写卡信息，读卡时传入，写卡时传出
class CardInfoDeal : public QObject
{
    Q_OBJECT
public:
    int ErrorCode = 0; //参见LaneDevCtrl中的定义
    QString ErrorDesc = "";
    int WorkNo = 0;
    int CardType = 0;
    QVariant CPCSysInfo;
    QVariant CPCBaseInfo;
    QVariant CPCEnExinfo;
    QVariant CPCPassInfo;
    QVariant CPCFeeInfo;
    QVariant File0015; //CCBaseData
    QVariant File0019; //CCTollData
    QVariant CappData; //写ETC卡时必填，消费信息
    int Flag = 0;
    int Seats = 0;
    QString cardID = "";       //卡号（用于解析后车道填写，便于后续处理，可写可不写）
    QString etcCardNetID = ""; //etc发行商编号（查表得到，用于解析后车道填写，便于后续处理，可写可不写）
    uint RestMoney = 0;        //CPU卡余额
    int PassInfoLen = 0;       //写卡时填写
    int FeeInfoLen = 0;        //写卡时填写

    CardInfoDeal() {}
    ~CardInfoDeal() override {}

public:
    Q_PROPERTY(int ErrorCode MEMBER ErrorCode)
    Q_PROPERTY(QString ErrorDesc MEMBER ErrorDesc)
    Q_PROPERTY(int WorkNo MEMBER WorkNo)
    Q_PROPERTY(int CardType MEMBER CardType)
    Q_PROPERTY(QVariant CPCSysInfo MEMBER CPCSysInfo)
    Q_PROPERTY(QVariant CPCBaseInfo MEMBER CPCBaseInfo)
    Q_PROPERTY(QVariant CPCEnExinfo MEMBER CPCEnExinfo)
    Q_PROPERTY(QVariant CPCPassInfo MEMBER CPCPassInfo)
    Q_PROPERTY(QVariant CPCFeeInfo MEMBER CPCFeeInfo)
    Q_PROPERTY(QVariant File0015 MEMBER File0015)
    Q_PROPERTY(QVariant File0019 MEMBER File0019)
    Q_PROPERTY(QVariant CappData MEMBER CappData)
    Q_PROPERTY(int Flag MEMBER Flag)
    Q_PROPERTY(int Seats MEMBER Seats)
    Q_PROPERTY(uint RestMoney MEMBER RestMoney)
    Q_PROPERTY(int PassInfoLen MEMBER PassInfoLen)
    Q_PROPERTY(int FeeInfoLen MEMBER FeeInfoLen)
};

//B0帧RSU状态
class B0Data : public QObject
{
    Q_OBJECT
public:
    int RSUStatus = 0;              //rsu主状态参数；0x00表示正常，否则表示异常；
    int PSAMNum = 0;                //Psam卡个数
    QString hexRSUTerminalId1 = ""; //PSAM卡终端机编号，省标PSAM卡号 (12+1)
    QString hexRSUTerminalId2 = ""; //PSAM卡终端机编号，国标PSAM卡号 (12+1)
    int RSUAlgId = 0;               //算法标识
    int RSUManuID = 0;              //RSU厂商代码，16进制表示
    QString hexRSUID = "";          //RSU编号，16进制表示(6+1)
    ushort RSUVersion = 0;          //RSU软件版本号，16进制表示
    QString Reserved = "";          //保留字节。

    B0Data() {}
    ~B0Data() override {}

public:
    Q_PROPERTY(int RSUStatus MEMBER RSUStatus)
    Q_PROPERTY(int PSAMNum MEMBER PSAMNum)
    Q_PROPERTY(QString hexRSUTerminalId1 MEMBER hexRSUTerminalId1)
    Q_PROPERTY(QString hexRSUTerminalId2 MEMBER hexRSUTerminalId2)
    Q_PROPERTY(int RSUAlgId MEMBER RSUAlgId)
    Q_PROPERTY(int RSUManuID MEMBER RSUManuID)
    Q_PROPERTY(QString hexRSUID MEMBER hexRSUID)
    Q_PROPERTY(ushort RSUVersion MEMBER RSUVersion)
    Q_PROPERTY(QString Reserved MEMBER Reserved)
};

class B1Data : public QObject
{
    Q_OBJECT
public:
    int PSAM1Channel = 0;   //原BYTE类型转为int并初始化
    int PSAM1Lock = 0;      //原BYTE类型转为int并初始化
    int PSAM1Auth = 0;      //原BYTE类型转为int并初始化
    int PSAM2Channel = 0;   //原BYTE类型转为int并初始化
    int PSAM2Lock = 0;      //原BYTE类型转为int并初始化
    int PSAM2Auth = 0;      //原BYTE类型转为int并初始化
    int Antenna1Status = 0; //原BYTE类型转为int并初始化
    int Antenna2Status = 0; //原BYTE类型转为int并初始化

    B1Data() {}
    ~B1Data() override {}

public:
    Q_PROPERTY(int PSAM1Channel MEMBER PSAM1Channel)
    Q_PROPERTY(int PSAM1Lock MEMBER PSAM1Lock)
    Q_PROPERTY(int PSAM1Auth MEMBER PSAM1Auth)
    Q_PROPERTY(int PSAM2Channel MEMBER PSAM2Channel)
    Q_PROPERTY(int PSAM2Lock MEMBER PSAM2Lock)
    Q_PROPERTY(int PSAM2Auth MEMBER PSAM2Auth)
    Q_PROPERTY(int Antenna1Status MEMBER Antenna1Status)
    Q_PROPERTY(int Antenna2Status MEMBER Antenna2Status)
};

class B2Data : public QObject
{
    Q_OBJECT
public:
    uint OBUID = 0;                   //OBU编号
    int ErrorCode = 0;                //执行状态代码，此处取值0x00
    int OBUType = 0;                  //OBU类型01H 为双片式 OBU，02H 为单片式 OBU，03H 为 CPC 卡，其他值保留
    QString hexIssuerIdentifier = ""; //发行商代码 16+1
    int ContractType = 0;             //合同类型(V2.5)
    int ContractVersion = 0;          //合同版本号(V2.5)
    QString hexSerialNumber = "";     //应用序列号16+1
    QString DateofIssue = "";         //启用日期(yyyymmdd)
    QString DateofExpire = "";        //过期日期(yyyymmdd)
    int EquitmentStatus = 0;          //设备类型（注：原词可能为EquipmentStatus，此处保持原拼写）
    int OBUStatus1 = 0;               //OBU存在状态
    int OBUStatus2 = 0;               //OBU存在状态

    B2Data() {}
    ~B2Data() override {}

public:
    Q_PROPERTY(uint OBUID MEMBER OBUID)
    Q_PROPERTY(int ErrorCode MEMBER ErrorCode)
    Q_PROPERTY(int OBUType MEMBER OBUType)
    Q_PROPERTY(QString hexIssuerIdentifier MEMBER hexIssuerIdentifier)
    Q_PROPERTY(int ContractType MEMBER ContractType)
    Q_PROPERTY(int ContractVersion MEMBER ContractVersion)
    Q_PROPERTY(QString hexSerialNumber MEMBER hexSerialNumber)
    Q_PROPERTY(QString DateofIssue MEMBER DateofIssue)
    Q_PROPERTY(QString DateofExpire MEMBER DateofExpire)
    Q_PROPERTY(int EquitmentStatus MEMBER EquitmentStatus)
    Q_PROPERTY(int OBUStatus1 MEMBER OBUStatus1)
    Q_PROPERTY(int OBUStatus2 MEMBER OBUStatus2)
};

class B3Data : public QObject
{
    Q_OBJECT
public:
    uint OBUID = 0;                           //OBU号
    int ErrorCode = 0;                        //执行状态代码，此处取值0x00
    QString VehicleLicencePlateNumber = "";   //OBU记载的车牌号(12+1)*
    short VehicleLicencePlateColor = 0;       //车牌颜色*
    int VehicleClass = 0;                     //车辆类型*
    int VehicleUserType = 0;                  //车辆用户类型*
    int VehicleDimensions = 0;                //车辆尺寸，包括长(2个字节)、宽(1个字节)、高(1个字节)
    int VehicleWheels = 0;                    //车轮数
    int VehicleAxles = 0;                     //车轴数
    short VehicleWheelBases = 0;              //轴距，二进制表示，长度为2个字节，单位为分米；如0x28，表示轴距为4米。
    uint VehicleWeightLimits = 0;             //车辆载重(货车)或座位数(客车)，二进制表示，单位为公斤/座 *
    QString VehicleSpecificInformation = "";  //车辆特征描述*，字符用ASCII编码表示，汉字用GB2312码表示，如"奔驰307"
    QString VehicleEngineNumber = "";         //车辆发动机号
    int VehicleLengh = 0;                     //货车长（注：原词可能为VehicleLength，此处保持原拼写）
    int VehicleWidth = 0;                     //货车宽
    int VehicleHeight = 0;                    //货车高
    uint VehicleWeightApproved = 0;           //货车核载质量/准牵引总质量kg
    uint VehicleWeightCurb = 0;               //货车整备质量kg
    uint VehicleWeightToll = 0;               //货车车辆总质量kg
    int VehiclePeopleNum = 0;                 //货车核定载人数
    QString VehicleIdentificationNumber = ""; //车辆识别代码
    QString Reserve = "";                     //预留 客车预留20字节，货车预留14字节
    int VehicleSeat = 0;                      //客车座位数,货车该字段值为0
    QVariant obuEfo4 = QVariant();            //OBU-ESAM EF04 内容（内部结构体转换为QVariant）
    QString HexOBUVehInfo = "";               //OBU车辆信息 HEX未解析的数据
    QString HexObuEF04 = "";                  //EF04信息 HEX未解析的数据

    B3Data() {}
    ~B3Data() override {}

public:
    Q_PROPERTY(uint OBUID MEMBER OBUID)
    Q_PROPERTY(int ErrorCode MEMBER ErrorCode)
    Q_PROPERTY(QString VehicleLicencePlateNumber MEMBER VehicleLicencePlateNumber)
    Q_PROPERTY(short VehicleLicencePlateColor MEMBER VehicleLicencePlateColor)
    Q_PROPERTY(int VehicleClass MEMBER VehicleClass)
    Q_PROPERTY(int VehicleUserType MEMBER VehicleUserType)
    Q_PROPERTY(int VehicleDimensions MEMBER VehicleDimensions)
    Q_PROPERTY(int VehicleWheels MEMBER VehicleWheels)
    Q_PROPERTY(int VehicleAxles MEMBER VehicleAxles)
    Q_PROPERTY(short VehicleWheelBases MEMBER VehicleWheelBases)
    Q_PROPERTY(uint VehicleWeightLimits MEMBER VehicleWeightLimits)
    Q_PROPERTY(QString VehicleSpecificInformation MEMBER VehicleSpecificInformation)
    Q_PROPERTY(QString VehicleEngineNumber MEMBER VehicleEngineNumber)
    Q_PROPERTY(int VehicleLengh MEMBER VehicleLengh)
    Q_PROPERTY(int VehicleWidth MEMBER VehicleWidth)
    Q_PROPERTY(int VehicleHeight MEMBER VehicleHeight)
    Q_PROPERTY(uint VehicleWeightApproved MEMBER VehicleWeightApproved)
    Q_PROPERTY(uint VehicleWeightCurb MEMBER VehicleWeightCurb)
    Q_PROPERTY(uint VehicleWeightToll MEMBER VehicleWeightToll)
    Q_PROPERTY(int VehiclePeopleNum MEMBER VehiclePeopleNum)
    Q_PROPERTY(QString VehicleIdentificationNumber MEMBER VehicleIdentificationNumber)
    Q_PROPERTY(QString Reserve MEMBER Reserve)
    Q_PROPERTY(int VehicleSeat MEMBER VehicleSeat)
    Q_PROPERTY(QVariant obuEfo4 MEMBER obuEfo4)
    Q_PROPERTY(QString HexOBUVehInfo MEMBER HexOBUVehInfo)
    Q_PROPERTY(QString HexObuEF04 MEMBER HexObuEF04)
};

class B4Data : public QObject
{
    Q_OBJECT
public:
    uint OBUID = 0;                 //OBU号
    int ErrorCode = 0;              //执行状态代码，此处取值0x00
    uint CardRestMoney = 0;         //卡余额，高位在前，低位在后；
    QVariant File0015 = QVariant(); //卡基本信息0015文件数据（内部结构体转换为QVariant）
    QVariant File0019 = QVariant(); //复合消费文件0019文件数据（内部结构体转换为QVariant）
    int VehFlag = 0;                //车辆属性(第0位1表示车卡绑定;第1位1表示收费卡;第2位1表示货车)
    int Seats = 0;                  //座位数
    QVariant enExInfo = QVariant(); //单片式 OBU 入出口信息(DF01/EF02)写入内容（内部结构体转换为QVariant）
    QString HexFile0015 = "";       //HexFile0015数组转换为QString
    QString HexFile0019 = "";       //HexFile0019数组转换为QString

    B4Data() {}
    ~B4Data() override {}

public:
    Q_PROPERTY(uint OBUID MEMBER OBUID)
    Q_PROPERTY(int ErrorCode MEMBER ErrorCode)
    Q_PROPERTY(uint CardRestMoney MEMBER CardRestMoney)
    Q_PROPERTY(QVariant File0015 MEMBER File0015)
    Q_PROPERTY(QVariant File0019 MEMBER File0019)
    Q_PROPERTY(int VehFlag MEMBER VehFlag)
    Q_PROPERTY(int Seats MEMBER Seats)
    Q_PROPERTY(QVariant enExInfo MEMBER enExInfo)
    Q_PROPERTY(QString HexFile0015 MEMBER HexFile0015)
    Q_PROPERTY(QString HexFile0019 MEMBER HexFile0019)
};

//B5帧,交易结束
class B5Data : public QObject
{
    Q_OBJECT
public:
    uint OBUID = 0;            //OBU号
    int ErrorCode = 0;         //执行状态代码
    QString hexGBtermid = "";  //国标PSAM卡终端号(6*2+1)
    QString hexTransTime = ""; //交易时间(yyyymmddhhnnss,7*2+1)
    int PSAMTransSerial = 0;   //PSAM卡交易序号
    ushort ETCTradNo = 0;      //联机交易序号 CPU卡
    int TransType = 0;         //交易类型9为复合消费,1为普通消费,新流程为9
    uint CardRestMoney = 0;    //卡余额，高位在前，低位在后；(扣款后卡片余额)
    uint TAC = 0;              //TAC码
    int AlgorithmFlag = 0;     //算法标识(只有V2.5版本才有)
    int KeyVersion = 0;        //消费密钥版本号

    B5Data() {}
    ~B5Data() override {}

public:
    Q_PROPERTY(uint OBUID MEMBER OBUID)
    Q_PROPERTY(int ErrorCode MEMBER ErrorCode)
    Q_PROPERTY(QString hexGBtermid MEMBER hexGBtermid)
    Q_PROPERTY(QString hexTransTime MEMBER hexTransTime)
    Q_PROPERTY(int PSAMTransSerial MEMBER PSAMTransSerial)
    Q_PROPERTY(ushort ETCTradNo MEMBER ETCTradNo)
    Q_PROPERTY(int TransType MEMBER TransType)
    Q_PROPERTY(uint CardRestMoney MEMBER CardRestMoney)
    Q_PROPERTY(uint TAC MEMBER TAC)
    Q_PROPERTY(int AlgorithmFlag MEMBER AlgorithmFlag)
    Q_PROPERTY(int KeyVersion MEMBER KeyVersion)
};

//B8帧,返回TAC信息
class B8Data : public QObject
{
    Q_OBJECT
public:
    uint OBUID = 0;         //OBU号
    int ErrorCode = 0;      //执行状态代码，此处取值0x00
    uint TAC = 0;           //交易 TAC 码
    uint TradeMac = 0;      //交易 MAC 码
    uint CardRestMoney = 0; //卡余额，高位在前，低位在后；(扣款后卡片余额)

    B8Data() {}
    ~B8Data() override {}

public:
    Q_PROPERTY(uint OBUID MEMBER OBUID)
    Q_PROPERTY(int ErrorCode MEMBER ErrorCode)
    Q_PROPERTY(uint TAC MEMBER TAC)
    Q_PROPERTY(uint TradeMac MEMBER TradeMac)
    Q_PROPERTY(uint CardRestMoney MEMBER CardRestMoney)
};

//B9帧,返回EF04拆分信息
class B9Data : public QObject
{
    Q_OBJECT
public:
    uint OBUID = 0;       //OBU号
    int ErrorCode = 0;    //执行状态代码，此处取值0x00
    uint ReadStatus = 0;  //读取拆分信息状态：0 为正常，非 0 异常，不操作填 0
    uint WriteStatus = 0; //更新信息状态：0 为正常，非 0 异常，不操作填 0
    uint ReadLen = 0;     //Ef04 文件读取长度
    QVariantList provFee; //OBU-ESAM EF04 中各省内容（内部结构体数组转换为QVariantList，初始化为空列表）
    int ProvCount = 0;    //拆分省份数（原BYTE类型转为int，初始化为0）

    B9Data() {}
    ~B9Data() override {}

public:
    Q_PROPERTY(uint OBUID MEMBER OBUID)
    Q_PROPERTY(int ErrorCode MEMBER ErrorCode)
    Q_PROPERTY(uint ReadStatus MEMBER ReadStatus)
    Q_PROPERTY(uint WriteStatus MEMBER WriteStatus)
    Q_PROPERTY(uint ReadLen MEMBER ReadLen)
    Q_PROPERTY(QVariantList provFee MEMBER provFee)
    Q_PROPERTY(int ProvCount MEMBER ProvCount)
};

//PSAM卡授权初始化信息
class PsamInfo : public QObject
{
    Q_OBJECT
public:
    int PSAMChannel = 0;        //原BYTE类型转为int，初始化为0
    QString HexTerminalId = ""; //原BYTE数组转为QString，初始化为空
    QString HexSerialNo = "";   //原BYTE数组转为QString，初始化为空
    int PSAMVersion = 0;        //原BYTE类型转为int，初始化为0
    QString HexDivFactor = "";  //原BYTE数组转为QString，初始化为空
    QString HexRandom = "";     //原BYTE数组转为QString，初始化为空

    PsamInfo() {}
    ~PsamInfo() override {}

public:
    Q_PROPERTY(int PSAMChannel MEMBER PSAMChannel)
    Q_PROPERTY(QString HexTerminalId MEMBER HexTerminalId)
    Q_PROPERTY(QString HexSerialNo MEMBER HexSerialNo)
    Q_PROPERTY(int PSAMVersion MEMBER PSAMVersion)
    Q_PROPERTY(QString HexDivFactor MEMBER HexDivFactor)
    Q_PROPERTY(QString HexRandom MEMBER HexRandom)
};

class BAData : public QObject
{
    Q_OBJECT
public:
    int ErrorCode = 0;               //原BYTE类型转为int，初始化为0
    QVariant PsamInfo1 = QVariant(); //内部结构体TPsamInfo转换为QVariant，初始化为空
    QVariant PsamInfo2 = QVariant(); //内部结构体TPsamInfo转换为QVariant，初始化为空

    BAData() {}
    ~BAData() override {}

public:
    Q_PROPERTY(int ErrorCode MEMBER ErrorCode)
    Q_PROPERTY(QVariant PsamInfo1 MEMBER PsamInfo1)
    Q_PROPERTY(QVariant PsamInfo2 MEMBER PsamInfo2)
};

//PSAM卡授权结果返回
class BBData : public QObject
{
    Q_OBJECT
public:
    int ErrorCode = 0;        //原BYTE类型转为int，初始化为0
    QString AuthResult1 = ""; //原BYTE数组转为QString，初始化为空
    QString AuthResult2 = ""; //原BYTE数组转为QString，初始化为空

    BBData() {}
    ~BBData() override {}

public:
    Q_PROPERTY(int ErrorCode MEMBER ErrorCode)
    Q_PROPERTY(QString AuthResult1 MEMBER AuthResult1)
    Q_PROPERTY(QString AuthResult2 MEMBER AuthResult2)
};

class BEData : public QObject
{
    Q_OBJECT
public:
    int Status = 0;    //原BYTE类型转为int，初始化为0
    uint BeaconID = 0; //原UINT类型转为uint，初始化为0

    BEData() {}
    ~BEData() override {}

public:
    Q_PROPERTY(int Status MEMBER Status)
    Q_PROPERTY(uint BeaconID MEMBER BeaconID)
};

class C0Data : public QObject
{
    Q_OBJECT
public:
    int LaneMode = 0;     //车道模式,3入口4出口
    int WaitTime = 0;     //最小重读时间 路侧单元自动发送 BST 的间隔，单位 ms
    int TxPower = 0;      //功率级数
    int PLLChannelID = 0; //信道号,0或1
    int TransMode = 0;    //复合交易09H
    int EF04Opt = 0;      //是否操作OBU-ESAM  DF01/EF04文件00H不操作，01H操作
    uint EF04Offset = 0;  //读EF04偏移量，如314字节为0x01 0x3A
    uint EF04Len = 0;     //EF04读长度，如91字节为0x00 0x5B
    QString Reserve = ""; //保留字节

    C0Data() {}
    ~C0Data() override {}

public:
    Q_PROPERTY(int LaneMode MEMBER LaneMode)
    Q_PROPERTY(int WaitTime MEMBER WaitTime)
    Q_PROPERTY(int TxPower MEMBER TxPower)
    Q_PROPERTY(int PLLChannelID MEMBER PLLChannelID)
    Q_PROPERTY(int TransMode MEMBER TransMode)
    Q_PROPERTY(int EF04Opt MEMBER EF04Opt)
    Q_PROPERTY(uint EF04Offset MEMBER EF04Offset)
    Q_PROPERTY(uint EF04Len MEMBER EF04Len)
    Q_PROPERTY(QString Reserve MEMBER Reserve)
};

//4D,线圈状态指令
class C4DData : public QObject
{
    Q_OBJECT
public:
    uint Status = 0; //线圈状态

    C4DData() {}
    ~C4DData() override {}

public:
    Q_PROPERTY(uint Status MEMBER Status)
};

//C1帧
class C1Data : public QObject
{
    Q_OBJECT
public:
    uint OBUID = 0; //原UINT类型转为uint，初始化为0
    int Type = 0;   //暂时没用

    C1Data() {}
    ~C1Data() override {}

public:
    Q_PROPERTY(uint OBUID MEMBER OBUID)
    Q_PROPERTY(int Type MEMBER Type)
};

//C2帧
class C2Data : public QObject
{
    Q_OBJECT
public:
    uint OBUID = 0;   //原UINT类型转为uint，初始化为0
    int StopType = 0; //01：结束交易，重新搜索车载单元；02：重新发送当前帧。03：该OBU已交易；04：特情需要播放语音

    C2Data() {}
    ~C2Data() override {}

public:
    Q_PROPERTY(uint OBUID MEMBER OBUID)
    Q_PROPERTY(int StopType MEMBER StopType)
};

//C3帧(只对入口有效)
class C3Data : public QObject
{
    Q_OBJECT
public:
    uint OBUID = 0;                 //原UINT类型转为uint，初始化为0
    QVariant File0019 = QVariant(); //内部结构体T0019FileData转换为QVariant，初始化为空
    QString hexEnTime = "";         //原char数组转为QString，初始化为空

    C3Data() {}
    ~C3Data() override {}

public:
    Q_PROPERTY(uint OBUID MEMBER OBUID)
    Q_PROPERTY(QVariant File0019 MEMBER File0019)
    Q_PROPERTY(QString hexEnTime MEMBER hexEnTime)
};

//分省
class EF04ProvFee : public QObject
{
    Q_OBJECT
public:
    int ProCode = 0; //省级行政区代码
    uint Fee = 0;    //金额

    EF04ProvFee() {}
    ~EF04ProvFee() {}

public:
    Q_PROPERTY(int ProCode MEMBER ProCode)
    Q_PROPERTY(uint Fee MEMBER Fee)
};

//OBU-ESAM EF04
//OBU EF04
class OBUEF04Data : public QObject
{
    Q_OBJECT
public:
    //注意，计费时不要传这两参数，仅用于天线数据交互
    QVariant File0019 = QVariant(); //CCBaseData
    QVariant File0015 = QVariant(); //CCTollData

    int VehicleUserType = 0;                               //车辆用户类型
    int PassProvCount = 0;                                 //通行省份个数
    uint TotalFee = 0;                                     //累计应收金额  3字节
    uint FactFee = 0;                                      //累计实收金额  3字节
    uint SuccessCount = 0;                                 //累计交易成功数  2字节
    uint TotalMileage = 0;                                 //累计计费里程
    int NoCardCount = 0;                                   //标签无卡累计次数
    QString EnCode = 0;                                    //本省入口编码
    uint LocatFee = 0;                                     //本省累计交易金额 3字节
    int LocatSuccessCount = 0;                             //本省累计交易成功数
    uint LocatFactFee = 0;                                 //本省累计实收金额
    QString HexEncry = "";                                 //加密摘要
    uint TollFee = 0;                                      //EF04 0019
    uint FlagID = 0;                                       //EF04 0019内的FlagID
    QDateTime FlagPassTime = QDateTime::currentDateTime(); //EF04 0019内的门架通行时间，转换为"yyyy-MM-dd hh:mm:ss"
    QVariantList ProvFee = {};

    OBUEF04Data() {}
    ~OBUEF04Data() {}

public:
    Q_PROPERTY(QVariant File0019 MEMBER File0019)
    Q_PROPERTY(QVariant File0015 MEMBER File0015)
    Q_PROPERTY(int VehicleUserType MEMBER VehicleUserType)
    Q_PROPERTY(int PassProvCount MEMBER PassProvCount)
    Q_PROPERTY(uint TotalFee MEMBER TotalFee)
    Q_PROPERTY(uint FactFee MEMBER FactFee)
    Q_PROPERTY(uint SuccessCount MEMBER SuccessCount)
    Q_PROPERTY(uint TotalMileage MEMBER TotalMileage)
    Q_PROPERTY(int NoCardCount MEMBER NoCardCount)
    Q_PROPERTY(QString EnCode MEMBER EnCode)
    Q_PROPERTY(uint LocatFee MEMBER LocatFee)
    Q_PROPERTY(int LocatSuccessCount MEMBER LocatSuccessCount)
    Q_PROPERTY(uint LocatFactFee MEMBER LocatFactFee)
    Q_PROPERTY(QString HexEncry MEMBER HexEncry)
    Q_PROPERTY(uint TollFee MEMBER TollFee)
    Q_PROPERTY(uint FlagID MEMBER FlagID)
    Q_PROPERTY(QDateTime FlagPassTime MEMBER FlagPassTime)
    Q_PROPERTY(QVariantList ProvFee MEMBER ProvFee)
};

//单片式 OBU 入出口信息(DF01/EF02)写入内容
class EnExInfo : public QObject
{
    Q_OBJECT
public:
    QString RoadNetNo = "";   //收费路网编号[**新增]（char数组转为QString，初始化为空）
    QString EnStation_h = ""; //入口站（char数组转为QString，初始化为空）
    int EnLane = 0;           //入口车道[**共用字段]（原BYTE类型转为int，初始化为0）
    uint EnTime = 0;          //入口时间(**新版本使用unix时间)（原UINT类型转为uint，初始化为0）
    QString ExStation_h = ""; //出口站（char数组转为QString，初始化为空）
    int ExLane = 0;           //出口车道[**共用字段]（原BYTE类型转为int，初始化为0）
    uint ExTime = 0;          //出口时间(**新版本使用unix时间)（原UINT类型转为uint，初始化为0）
    int VehClass = 0;         //车型（原BYTE类型转为int，初始化为0）
    int Status = 0;           //入出口状态3-入口 4出口（原BYTE类型转为int，初始化为0）
    QString VehPlate = "";    //出入口抓拍车牌12+1[**共用字段]（char数组转为QString，初始化为空）
    int PlateColor = 0;       //车牌颜色(**新增字段 )（原BYTE类型转为int，初始化为0）
    int AxisCount = 0;        //（原BYTE类型转为int，初始化为0）
    int VehicleLengh = 0;     //货车长2字节（保持int类型，初始化为0，注：原词可能为VehicleLength，此处保持原拼写）
    int VehicleWidth = 0;     //货车宽1字节（保持int类型，初始化为0）
    int VehicleHeight = 0;    //货车高1字节（保持int类型，初始化为0）
    uint TollWeight = 0;      //（原UINT类型转为uint，初始化为0）
    QString Remark = "";      //31预留（char数组转为QString，初始化为空）

    EnExInfo() {}
    ~EnExInfo() override {}

public:
    Q_PROPERTY(QString RoadNetNo MEMBER RoadNetNo)
    Q_PROPERTY(QString EnStation_h MEMBER EnStation_h)
    Q_PROPERTY(int EnLane MEMBER EnLane)
    Q_PROPERTY(uint EnTime MEMBER EnTime)
    Q_PROPERTY(QString ExStation_h MEMBER ExStation_h)
    Q_PROPERTY(int ExLane MEMBER ExLane)
    Q_PROPERTY(uint ExTime MEMBER ExTime)
    Q_PROPERTY(int VehClass MEMBER VehClass)
    Q_PROPERTY(int Status MEMBER Status)
    Q_PROPERTY(QString VehPlate MEMBER VehPlate)
    Q_PROPERTY(int PlateColor MEMBER PlateColor)
    Q_PROPERTY(int AxisCount MEMBER AxisCount)
    Q_PROPERTY(int VehicleLengh MEMBER VehicleLengh)
    Q_PROPERTY(int VehicleWidth MEMBER VehicleWidth)
    Q_PROPERTY(int VehicleHeight MEMBER VehicleHeight)
    Q_PROPERTY(uint TollWeight MEMBER TollWeight)
    Q_PROPERTY(QString Remark MEMBER Remark)
};

//C6帧(只对出口有效)
class C6Data : public QObject
{
    Q_OBJECT
public:
    uint OBUID = 0;                 //原UINT类型转为uint，初始化为0
    int ConsumeMoney = 0;           //单位:分（保持int类型，初始化为0）
    QString hexExTime = "";         //(7B)yyyymmddhhnnss（char数组转为QString，初始化为空）
    QVariant File0019 = QVariant(); //43字节（内部结构体T0019FileData转换为QVariant，初始化为空）
    int OBUTradeType = 0;           //交易类型（原BYTE类型转为int，初始化为0）
    QString hexOBUDivFactor = "";   //OBU一级分散因子 16+1  B2帧的issuer（char数组转为QString，初始化为空）
    uint EF04Offset = 0;            //读EF04偏移量，如314字节为0x01 0x3A（原UINT类型转为uint，初始化为0）
    uint EF04Len = 0;               //EF04读长度，如91字节为0x00 0x5B（原UINT类型转为uint，初始化为0）
    QVariant obuEfo4 = QVariant();  //OBU-ESAM EF04 内容（内部结构体TOBUEF04Data转换为QVariant，初始化为空）
    QVariant enExInfo = QVariant(); //单片式 OBU 入出口信息(DF01/EF02)写入内容（内部结构体TEnExInfo转换为QVariant，初始化为空）

    C6Data() {}
    ~C6Data() override {}

public:
    Q_PROPERTY(uint OBUID MEMBER OBUID)
    Q_PROPERTY(int ConsumeMoney MEMBER ConsumeMoney)
    Q_PROPERTY(QString hexExTime MEMBER hexExTime)
    Q_PROPERTY(QVariant File0019 MEMBER File0019)
    Q_PROPERTY(int OBUTradeType MEMBER OBUTradeType)
    Q_PROPERTY(QString hexOBUDivFactor MEMBER hexOBUDivFactor)
    Q_PROPERTY(uint EF04Offset MEMBER EF04Offset)
    Q_PROPERTY(uint EF04Len MEMBER EF04Len)
    Q_PROPERTY(QVariant obuEfo4 MEMBER obuEfo4)
    Q_PROPERTY(QVariant enExInfo MEMBER enExInfo)
};

//C8异常交易指令
class C8Data : public QObject
{
    Q_OBJECT
public:
    uint OBUID = 0;                //原UINT类型转为uint，初始化为0
    uint UserSerial = 0;           //用户卡交易序号（原UINT类型转为uint，初始化为0）
    int OpMode = 0;                //00H：仅仅获取 TAC；01H：获取 TAC 后并写入 EF04（原BYTE类型转为int，初始化为0）
    uint EF04Offset = 0;           //（原UINT类型转为uint，初始化为0）
    uint EF04Len = 0;              //（原UINT类型转为uint，初始化为0）
    QVariant obuEfo4 = QVariant(); //内部结构体TOBUEF04Data转换为QVariant，初始化为空

    C8Data() {}
    ~C8Data() override {}

public:
    Q_PROPERTY(uint OBUID MEMBER OBUID)
    Q_PROPERTY(uint UserSerial MEMBER UserSerial)
    Q_PROPERTY(int OpMode MEMBER OpMode)
    Q_PROPERTY(uint EF04Offset MEMBER EF04Offset)
    Q_PROPERTY(uint EF04Len MEMBER EF04Len)
    Q_PROPERTY(QVariant obuEfo4 MEMBER obuEfo4)
};

//C9获取EF04拆分指令
class C9Data : public QObject
{
    Q_OBJECT
public:
    uint OBUID = 0;      //原UINT类型转为uint，初始化为0
    int ReadMode = 0;    //00H：仅仅获取 TAC；01H：获取 TAC 后并写入 EF04（原BYTE类型转为int，初始化为0）
    uint ReadOffset = 0; //读 EF04 偏移（原UINT类型转为uint，初始化为0）
    uint ReadLen = 0;    //读 EF04 长度（原UINT类型转为uint，初始化为0）
    int WriteMode
        = 0; //00H：不更新 01H：更新(按 WriteInfo 指定的内容更新) 02H：固定清除(将指定偏移量和长度的 EF04 内容 写 0x00)（原BYTE类型转为int，初始化为0）
    uint WriteOffset = 0;          //写 Ef04 偏移量（原UINT类型转为uint，初始化为0）
    uint WriteLen = 0;             //写 Ef04 文件长度（原UINT类型转为uint，初始化为0）
    QVariant obuEfo4 = QVariant(); //OBU-ESAM EF04 内容（内部结构体TOBUEF04Data转换为QVariant，初始化为空）

    C9Data() {}
    ~C9Data() override {}

public:
    Q_PROPERTY(uint OBUID MEMBER OBUID)
    Q_PROPERTY(int ReadMode MEMBER ReadMode)
    Q_PROPERTY(uint ReadOffset MEMBER ReadOffset)
    Q_PROPERTY(uint ReadLen MEMBER ReadLen)
    Q_PROPERTY(int WriteMode MEMBER WriteMode)
    Q_PROPERTY(uint WriteOffset MEMBER WriteOffset)
    Q_PROPERTY(uint WriteLen MEMBER WriteLen)
    Q_PROPERTY(QVariant obuEfo4 MEMBER obuEfo4)
};

//地感信息
class D0Data : public QObject
{
    Q_OBJECT
public:
    int FrameType = 0; //原BYTE类型转为int，初始化为0
    uint OBUID = 0;    //原UINT类型转为uint，初始化为0
    uint X = 0;        //原UINT类型转为uint，初始化为0
    uint Y = 0;        //原UINT类型转为uint，初始化为0

    D0Data() {}
    ~D0Data() override {}

public:
    Q_PROPERTY(int FrameType MEMBER FrameType)
    Q_PROPERTY(uint OBUID MEMBER OBUID)
    Q_PROPERTY(uint X MEMBER X)
    Q_PROPERTY(uint Y MEMBER Y)
};

//PSAM卡授权初始化指令(CA,不需要定义)
//PSAM卡授权指令(CB,目前只用单卡，所以只有APDU1,其它为0)
class CBData : public QObject
{
    Q_OBJECT
public:
    int Channel1 = 0;   //原BYTE类型转为int，初始化为0
    int AuthFlag1 = 0;  //原BYTE类型转为int，初始化为0
    QString APDU1 = ""; //原char数组转为QString，初始化为空（长度13）
    int Channel2 = 0;   //原BYTE类型转为int，初始化为0
    int AuthFlag2 = 0;  //原BYTE类型转为int，初始化为0
    QString APDU2 = ""; //原char数组转为QString，初始化为空（长度13）

    CBData() {}
    ~CBData() override {}

public:
    Q_PROPERTY(int Channel1 MEMBER Channel1)
    Q_PROPERTY(int AuthFlag1 MEMBER AuthFlag1)
    Q_PROPERTY(QString APDU1 MEMBER APDU1)
    Q_PROPERTY(int Channel2 MEMBER Channel2)
    Q_PROPERTY(int AuthFlag2 MEMBER AuthFlag2)
    Q_PROPERTY(QString APDU2 MEMBER APDU2)
};

//变更天线ID 发送设置BeaconID
class CEData : public QObject
{
    Q_OBJECT
public:
    int SetMode = 0;  // 00. 停止持续改变 BeaconID 01. 间隔一定时间连续改变 BeaconID 02. 临时改变一次 BeaconID ;
    int Interval = 0; // Interval 改变 BeaconID 的间隔时间，
    CEData() {}
    ~CEData() override {}

public:
    Q_PROPERTY(int SetMode MEMBER SetMode)
    Q_PROPERTY(int Interval MEMBER Interval)
};

//心跳应答
class CFData : public QObject
{
    Q_OBJECT
public:
    int Status = 0; //
    CFData() {}
    ~CFData() override {}

public:
    Q_PROPERTY(int Status MEMBER Status)
};

//帧头
class RsuCmd : public QObject
{
    Q_OBJECT
public:
    int SelRsu = 0;   //原BYTE类型转为int，初始化为0
    int NewRsctl = 0; //原BYTE类型转为int，初始化为0
    int CmdType = 0;  //原BYTE类型转为int，初始化为0
    int DealType = 0; //原BYTE类型转为int，初始化为0（1:串口 2：网口）
    int Version = 0;  //原BYTE类型转为int，初始化为0
    uint DataLen = 0; //原UINT类型转为uint，初始化为0

    RsuCmd() {}
    ~RsuCmd() override {}

public:
    Q_PROPERTY(int SelRsu MEMBER SelRsu)
    Q_PROPERTY(int NewRsctl MEMBER NewRsctl)
    Q_PROPERTY(int CmdType MEMBER CmdType)
    Q_PROPERTY(int DealType MEMBER DealType)
    Q_PROPERTY(int Version MEMBER Version)
    Q_PROPERTY(uint DataLen MEMBER DataLen)
};
} // namespace Media

//==============================================================================
// 计费对象
//==============================================================================
namespace CalFee {
class Login : public QObject
{
    Q_OBJECT
public:
    int laneType = 0;          //必填 1.入口 2.出口
    QString stationID;         //收费站代码
    int laneID = 0;            //车道号
    int bizType = 0;           //0.手持机收费 1.手持机稽核 2. 车道前端 3.预计费#
    QString flagID = "";       //承载门架编号
    QString cnLaneID = "";     //路网中心车道号
    QString virEnStation = ""; //开放段时需配置
    QString operatorID = "";   //Hex字符串
    QString authCode = "";     //车道前端填000000
    QString sdkName = "";      //车道系统厂家名称MGS
    QString deviceID = "";     //车道前端填车道IP
    QString shiftDate = "";    //格式yyyy-MM-dd 如：2024-08-08 默认当前日期，用户可选
    int shiftID = 0;           //班次         1早班 2中班 3晚班 0点至8点默认 1 8点至16点默认 2 16点至0点默认 3 用户可选

    Login() {}
    ~Login() override {}

public:
    Q_PROPERTY(int laneType MEMBER laneType)
    Q_PROPERTY(QString stationID MEMBER stationID)
    Q_PROPERTY(int laneID MEMBER laneID)
    Q_PROPERTY(int bizType MEMBER bizType)
    Q_PROPERTY(QString flagID MEMBER flagID)
    Q_PROPERTY(QString cnLaneID MEMBER cnLaneID)
    Q_PROPERTY(QString virEnStation MEMBER virEnStation)
    Q_PROPERTY(QString operatorID MEMBER operatorID)
    Q_PROPERTY(QString authCode MEMBER authCode)
    Q_PROPERTY(QString sdkName MEMBER sdkName)
    Q_PROPERTY(QString deviceID MEMBER deviceID)
    Q_PROPERTY(QString shiftDate MEMBER shiftDate)
    Q_PROPERTY(int shiftID MEMBER shiftID)
};

class LoginResult : public QObject
{
    Q_OBJECT
public:
    int loginStatus = 1;      //默认登陆失败
    QString loginCode = "";   //登录标识
    QString desc = "";        //登班描述
    QString stationName = ""; //收费站名称
    QString flagName = "";    //承载门架名称
    QVariant shiftStatInfo;   //工班统计信息（结构为工班表，外部可转）
    QVariantList ticketInfo;  //票据信息（存在多条）
    int thirdPayCount = 0;    //第三方支付数
    LoginResult() {}
    ~LoginResult() override {}

public:
    Q_PROPERTY(int loginStatus MEMBER loginStatus)
    Q_PROPERTY(QString loginCode MEMBER loginCode)
    Q_PROPERTY(QString desc MEMBER desc)
    Q_PROPERTY(QString stationName MEMBER stationName)
    Q_PROPERTY(QString flagName MEMBER flagName)
    Q_PROPERTY(QVariant shiftStatInfo MEMBER shiftStatInfo)
    Q_PROPERTY(QVariantList ticketInfo MEMBER ticketInfo)
    Q_PROPERTY(int thirdPayCount MEMBER thirdPayCount)
};

//兜底OBJ
class ShortFee : public QObject
{
    Q_OBJECT
public:
    int lpFeeMil = 0; //本省兜底里程
    int lpFee = 0;    //本省兜底费用
    int lpEtcFee = 0; //本省ETC费用
    // int lpFee17;    //本省超6轴加收
    // int lpEtcFee17; //本省ETC超6轴加收
    int opFee = 0;        //外省兜底费用
    int opEtcFee = 0;     //外省ETC费用
    int opFeeMil = 0;     //外省兜底里程
    int proNum = 0;       //兜底跨省通行个数
    bool isValid = false; //是否成功获取兜底费率

    ShortFee() {}
    ~ShortFee() override {}
    // QObject *CreateNew() { return new ShortFee(); }
public:
    Q_PROPERTY(int lpFee MEMBER lpFee)
    Q_PROPERTY(int lpEtcFee MEMBER lpEtcFee)
    Q_PROPERTY(int lpFeeMil MEMBER lpFeeMil)
    Q_PROPERTY(int opFee MEMBER opFee)
    Q_PROPERTY(int opEtcFee MEMBER opEtcFee)
    Q_PROPERTY(int opFeeMil MEMBER opFeeMil)
    Q_PROPERTY(int proNum MEMBER proNum)
    Q_PROPERTY(bool isValid MEMBER isValid)
};

//远程计费OBJ
class CloudFee : public QObject
{
    Q_OBJECT
public:
    // 变量定义
    int shouldPay = 0;            //应收总金额，对应payFee
    int factPay = 0;              //实收总金额，注意，对应splitAmount
    int lpShouldPay = 0;          //本省应收
    int lpFactPay = 0;            //本省实收
    int lpFeeMil = 0;             //本省里程数
    int opShouldPay = 0;          //外省应收
    int opFactPay = 0;            //外省实收
    int opFeeMil = 0;             //外省里程数
    QString cloudFeeDetails = ""; //分省信息，用 | 分割
    bool isDiscount = false;      //调远程计费时有没有打折
    int discountType = 0;         //特殊折扣类型，specialDiscountType
    int exitFeeType = 0;          //计费方式 4,5 分别为省在线和部在线
    int proNum = 0;               //通行省份个数
    bool isValid = false;         //是否成功在线计费
    QString errDesc = "";         //在线计费失败原因

    CloudFee() {}
    ~CloudFee() override {}
    // QObject *CreateNew() { return new CloudFee(); }
public:
    // 统一编写的Q_PROPERTY
    Q_PROPERTY(int shouldPay MEMBER shouldPay)                 //应收总金额，对应payFee
    Q_PROPERTY(int factPay MEMBER factPay)                     //实收总金额，对应splitAmount
    Q_PROPERTY(int lpShouldPay MEMBER lpShouldPay)             //本省应收
    Q_PROPERTY(int lpFactPay MEMBER lpFactPay)                 //本省实收
    Q_PROPERTY(int lpFeeMil MEMBER lpFeeMil)                   //本省里程数
    Q_PROPERTY(int opShouldPay MEMBER opShouldPay)             //外省应收
    Q_PROPERTY(int opFactPay MEMBER opFactPay)                 //外省实收
    Q_PROPERTY(int opFeeMil MEMBER opFeeMil)                   //外省里程数
    Q_PROPERTY(QString cloudFeeDetails MEMBER cloudFeeDetails) //分省信息，用 | 分割
    Q_PROPERTY(bool isDiscount MEMBER isDiscount)              //调远程计费时有没有打折
    Q_PROPERTY(int discountType MEMBER discountType)           //特殊折扣类型，specialDiscountType
    Q_PROPERTY(int exitFeeType MEMBER exitFeeType)             //计费方式 4,5 分别为省在线和部在线
    Q_PROPERTY(int proNum MEMBER proNum)                       //通行省份个数
    Q_PROPERTY(bool isValid MEMBER isValid)                    //是否成功在线计费
    Q_PROPERTY(QString errDesc MEMBER errDesc)                 //在线计费失败原因
};

//承载门架计费OBJ
class FlagFee : public QObject
{
    Q_OBJECT
public:
    // 变量定义
    int fee = 0;                   //门架打折前的费用
    int payFee = 0;                //门架打折后的费用
    int discountType = 0;          //门架折扣类型
    QString feeVersion = "";       //费率版本
    QString intervalDiscount = ""; //计费单元折扣 |               1 | 2
    QString intervalFee = "";      //计费单元应收  |               3 | 4
    QString intervalPayFee = "";   //计费单元折扣后费用   |         2 | 2
    QString intervalID = "";       //计费单元ID      |              ...
    QString intervalName = "";     //计费单元名称     |
    QString agentFlags = "";       //
    QString agentFees = "";        //
    int gantryPassCount = 0;       //门架数
    QString gantryPassInfo = "";   //
    int feeMil = 0;                //门架计费里程
    int obuTotalTradeSuccNum = 0;  //交易成功数 （ETC交易时填写）
    int obuPayFeeSum = 0;          //门架计费总额（ETC交易时填写）
    int obuDiscountFeeSum = 0;     //折扣金额（ETC交易时填写）
    int obuProvfeeSum = 0;         //OBU省内金额（ETC交易时填写）
    int obuProvfactSum = 0;        //OBU省内实付（ETC交易时填写）
    int obuProvTradeSuccNum = 0;   //OBU省内交易成功数（ETC交易时填写）
    int feeSumLocalBefore = 0;     //CPC卡加上本省金额前（CPC交易时填写）
    int feeSumLocalAfter = 0;      //CPC卡加上本省金额后 （CPC交易时填写）
    QString remark = "";           //remark
    bool isValid = false;          //是否有效

    FlagFee() {}
    ~FlagFee() override {}
    // QObject *CreateNew() { return new FlagFee(); }
public:
    Q_PROPERTY(int fee MEMBER fee)
    Q_PROPERTY(int payFee MEMBER payFee)
    Q_PROPERTY(int discountType MEMBER discountType)
    Q_PROPERTY(QString feeVersion MEMBER feeVersion)
    Q_PROPERTY(QString intervalDiscount MEMBER intervalDiscount)
    Q_PROPERTY(QString intervalFee MEMBER intervalFee)
    Q_PROPERTY(QString intervalPayFee MEMBER intervalPayFee)
    Q_PROPERTY(QString intervalID MEMBER intervalID)
    Q_PROPERTY(QString intervalName MEMBER intervalName)
    Q_PROPERTY(QString agentFlags MEMBER agentFlags)
    Q_PROPERTY(QString agentFees MEMBER agentFees)
    Q_PROPERTY(int gantryPassCount MEMBER gantryPassCount)
    Q_PROPERTY(QString gantryPassInfo MEMBER gantryPassInfo)
    Q_PROPERTY(int feeMil MEMBER feeMil)
    Q_PROPERTY(int obuTotalTradeSuccNum MEMBER obuTotalTradeSuccNum)
    Q_PROPERTY(int obuPayFeeSum MEMBER obuPayFeeSum)
    Q_PROPERTY(int obuDiscountFeeSum MEMBER obuDiscountFeeSum)
    Q_PROPERTY(int obuProvfeeSum MEMBER obuProvfeeSum)
    Q_PROPERTY(int obuProvfactSum MEMBER obuProvfactSum)
    Q_PROPERTY(int obuProvTradeSuccNum MEMBER obuProvTradeSuccNum)
    Q_PROPERTY(int feeSumLocalBefore MEMBER feeSumLocalBefore)
    Q_PROPERTY(int feeSumLocalAfter MEMBER feeSumLocalAfter)
    Q_PROPERTY(QString remark MEMBER remark)
    Q_PROPERTY(bool isValid MEMBER isValid)
};

//CPC卡内介质计费OBJ
class CPCFee : public QObject
{
    Q_OBJECT
public:
    // 变量定义
    int fee = 0;                //卡面金额 (总金额)
    int lpFee = 0;              //本省卡面应收
    int lpFeeMil = 0;           //本省里程数
    int opFee = 0;              //外省卡面应收
    int opFeeMil = 0;           //外省里程数
    int proNum = 0;             //通行省份个数
    QString splitProvince = ""; //卡内分省信息，用 |分割
    bool isValid = false;       //是否有效

    // 统一编写的Q_PROPERTY
    Q_PROPERTY(int fee MEMBER fee)                         //卡面金额 (总金额)
    Q_PROPERTY(int lpFee MEMBER lpFee)                     //本省卡面应收
    Q_PROPERTY(int lpFeeMil MEMBER lpFeeMil)               //本省里程数
    Q_PROPERTY(int opFee MEMBER opFee)                     //外省卡面应收
    Q_PROPERTY(int opFeeMil MEMBER opFeeMil)               //外省里程数
    Q_PROPERTY(int proNum MEMBER proNum)                   //通行省份个数
    Q_PROPERTY(QString splitProvince MEMBER splitProvince) //卡内分省信息，用 |分割
    Q_PROPERTY(bool isValid MEMBER isValid)                //是否有效

    CPCFee() {}
    ~CPCFee() override {}
    // QObject *CreateNew() { return new CPCFee(); }
};

//OBU介质计费Obj
class OBUFee : public QObject
{
    Q_OBJECT
public:
    // 变量定义
    int fee = 0;                //OBU金额
    int payFee = 0;             //OBU打折后金额
    int lpFee = 0;              //本省OBU打折前
    int lpPayFee = 0;           //本省OBU打折后
    int lpFeeMil = 0;           //本省计费里程数
    int opFee = 0;              //外省OBU应收
    int opPayFee = 0;           //外省OBU实收
    int opFeeMil = 0;           //外省计费里程数
    int proNum = 0;             //通行省份个数
    QString splitProvince = ""; //OBU分省信息，用 |分割
    bool isValid = false;       //是否有效

    OBUFee() {}
    ~OBUFee() override {}
    // QObject *CreateNew() { return new OBUFee(); }
public:
    // 统一编写的Q_PROPERTY
    Q_PROPERTY(int fee MEMBER fee)                         //OBU金额
    Q_PROPERTY(int payFee MEMBER payFee)                   //OBU打折后金额
    Q_PROPERTY(int lpFee MEMBER lpFee)                     //本省OBU打折前
    Q_PROPERTY(int lpPayFee MEMBER lpPayFee)               //本省OBU打折后
    Q_PROPERTY(int lpFeeMil MEMBER lpFeeMil)               //本省计费里程数
    Q_PROPERTY(int opFee MEMBER opFee)                     //外省OBU应收
    Q_PROPERTY(int opPayFee MEMBER opPayFee)               //外省OBU实收
    Q_PROPERTY(int opFeeMil MEMBER opFeeMil)               //外省计费里程数
    Q_PROPERTY(int proNum MEMBER proNum)                   //通行省份个数
    Q_PROPERTY(QString splitProvince MEMBER splitProvince) //OBU分省信息，用 |分割
    Q_PROPERTY(bool isValid MEMBER isValid)                //是否有效
};

//计费请求
class CalFeeRequest : public QObject
{
    Q_OBJECT
public:
    QVariant oneTrade;
    QVariant cpcPassInfo;
    QVariant cpcFeeInfo;
    QVariant obuEF04Data;
    int laneType; //1 混合入 2 混合出 3 ETC入 4 ETC出
    bool doubleCardCal = false;
    bool discount = false;
    bool bindPlate = true; //是否绑定卡
    bool mixTrade = false; //混合车道是否天线交易（ETC车道默认true）

    CalFeeRequest() {}
    ~CalFeeRequest() override {}

public:
    Q_PROPERTY(QVariant oneTrade MEMBER oneTrade)
    Q_PROPERTY(QVariant cpcPassInfo MEMBER cpcPassInfo)
    Q_PROPERTY(QVariant cpcFeeInfo MEMBER cpcFeeInfo)
    Q_PROPERTY(QVariant obuEF04Data MEMBER obuEF04Data)
    Q_PROPERTY(bool doubleCardCal MEMBER doubleCardCal)
    Q_PROPERTY(bool discount MEMBER discount)
    Q_PROPERTY(bool bindPlate MEMBER bindPlate)
    Q_PROPERTY(bool mixTrade MEMBER mixTrade)
    Q_PROPERTY(int laneType MEMBER laneType)
};

//计费返回
class CalFeeResult : public QObject
{
    Q_OBJECT
public:
    QVariant oneTrade;
    QVariant shortFee;
    QVariant obuFee;
    QVariant cpcFee;
    QVariant cloudFee;
    QVariant flagFee;   //承载门架
    QVariant gantryFee; //拟合
    int errCode = 0;
    QString errDesc = "";
    int specialCode = 0;
    QString specialDesc = "";
    // 构造函数：初始化所有成员变量
    CalFeeResult() {}
    ~CalFeeResult() override {}

public:
    Q_PROPERTY(QVariant oneTrade MEMBER oneTrade)
    Q_PROPERTY(QVariant shortFee MEMBER shortFee)
    Q_PROPERTY(QVariant obuFee MEMBER obuFee)
    Q_PROPERTY(QVariant cpcFee MEMBER cpcFee)
    Q_PROPERTY(QVariant cloudFee MEMBER cloudFee)
    Q_PROPERTY(QVariant flagFee MEMBER flagFee)
    Q_PROPERTY(QVariant gantryFee MEMBER gantryFee)
    Q_PROPERTY(int errCode MEMBER errCode)
    Q_PROPERTY(QString errDesc MEMBER errDesc)
    Q_PROPERTY(int specialCode MEMBER specialCode)
    Q_PROPERTY(QString specialDesc MEMBER specialDesc)
};
} //namespace CalFee

//==============================================================================
// 交易对象
//==============================================================================
namespace Trade {
class OneTrade : public QObject
{
    Q_OBJECT
public:
    // 通用相关成员
    QString tradeID = "";     // STATIONID + LANEID + TIME + SEQ
    int seqNum = 0;           // 数据序列号
    QString passID = "";      // 通行ID
    QString cnLaneID = "";    // 路网中心车道号
    int vehSpeed = 0;         // 车速
    int peccType = 0;         // 违章类型
    int dealStatus = 0;       // 出口车情
    int deviceStatus = 0;     // 设备状态
    int recordType = 0;       // 记录类型
    int isValid = 0;          // 是否有效
    QString termCode = "";    // 终端机编码
    int cardTradeNo = 0;      // 用户卡交易序号
    int termTradeNo = 0;      // 终端交易序号
    int spendTime = 0;        // 收费耗时
    int provinceID = 0;       // 省份ID
    int verifycode = 0;       // 校验码
    int dataMonth = 0;        // 数据月份
    QString reserve = "";     // 备用字段
    QString appVersion = "";  // 车道系统版本号
    int userType = 0;         // 车种
    int transFee = 0;         // 卡面扣费金额
    QString specialType = ""; // 特情类型
    int scrapTickets = 0;     // 废票次数
    int axisLimit = 0;        // 总轴限

    // 卡介质相关成员
    int cardNetID = 0;      // 卡网络编号（识别省份）
    int cardType = 0;       // 卡类型（参照EM_CardType）
    int cardBizType = 0;    // 卡业务类型（参照EM_CardBizType）
    QString cardID = "";    // 卡号（"0"为无卡）
    QString cardPlate = ""; // 卡车牌
    int etcCardVer = 0;     // ETC卡版本号

    // 入口相关成员
    int enNetID = 0;                                                  // 入口路网编号
    QString enStation = "";                                           // 入口收费站ID
    QString enStationName = "";                                       // 入口收费站名称（对应StationName）
    int enLane = 0;                                                   // 入口车道号
    QDateTime enTime = QDateTime::currentDateTime().addSecs(-5);      // 入口时间
    QString enOperatorID = "";                                        // 入口操作人员ID
    QString enOperator = "";                                          // 入口操作人员名称
    int enShiftID = 0;                                                // 入口班次ID
    QDateTime enShiftDate = QDateTime::currentDateTime().addSecs(-5); // 入口班次日期
    int enVehClass = 0;                                               // 入口车型
    int enVehStatus = 0;                                              // 入口车种
    QString enVehPlate = "";                                          // 入口车牌

    // OBU相关成员
    QString obuPlate = "";     // OBU车牌
    int obuID = 0;             // 电子标签鉴别码(OBUID)
    uint restMoney = 0;        // 电子钱包余额
    double tac = 0.0;          // ETC TAC码
    QString obuSerialNum = ""; // OBU应用序列号
    int obuStatus = 0;         // OBU状态
    int algorithm = 0;         // 算法标识
    int obuContractVer = 0;    // OBU合同版本号
    int obuVehClass = 0;       // OBU车型

    // 称重相关成员
    int enAxisCount = 0;   // 入口轴数（对应AxisCount）
    int enTotalWeight = 0; // 入口总轴重（对应TotalWeight）

public:
    OneTrade() {}
    ~OneTrade() override {}
    //销毁后生成新的，用于清空交易
    virtual void tradeInit() = 0;
};

class OutTrade : public OneTrade
{
    Q_OBJECT
public:
    // 通用相关成员
    int payType = 0;         // 支付方式
    int changeType = 0;      // 变更类型
    double changedFee = 0.0; // 变更费用
    int chkPoint = 0;        // 标识站
    int opeateSign = 0;      // 操作标志
    QString reserve2 = "";   // 备用字段2
    double tsVehicle = 0.0;  // 交调口径折算车次
    int recordType2 = 0;     // 记录类型
    int tollSupport = 0;     // 计费支撑类型
    int openFlag = 0;        // 开放段标识

    // 费用相关成员
    int tollRateVer = 0;        // 费率版本号
    double shouldPay = 0.0;     // 收费总额(应收)
    int discountType = 0;       // 折扣类型
    double discount = 0.0;      // 折扣金额
    double opFactPay = 0.0;     // 外省实收
    double factPay = 0.0;       // 总费额(实收)
    int merRebateType = 0;      // 商户折扣类型
    QString merchant = "";      // 代扣用户帐号
    double rebateRatio = 0.0;   // 折扣率
    double merShouldPay = 0.0;  // 商户应付金额
    double rebatedPay = 0.0;    // 折扣后金额
    double specialPay = 0.0;    // 特殊路段收费
    int shortFee = 0;           // 最短路径交易金额
    int shortFeeMileage = 0;    // 最短计费里程
    double feeRate = 0.0;       // 交易金额占比
    int exitFeeType = 0;        // 计费方式
    int transPayType = 0;       // 交易支付方式
    uint postBalance = 0;       // 扣后余额
    int cardFeeSum = 0;         // 卡片交易累计金额
    int localFeeMileage = 0;    // 本地计费里程
    QString splitProvince = ""; // 多条省内交易信息

    // OBU相关成员
    int obuPayFeeSum = 0;         // OBU交易累计应收金额
    int obuDiscountFeeSum = 0;    // OBU交易累计优惠金额
    int obuMileage = 0;           // 累计计费里程
    int obuTotalTradeSuccNum = 0; // 总交易成功数量
    int obuProvFeeSum = 0;        // 本省交易累计金额
    int obuProvFactSum = 0;       // 本省交易累计实收金额
    int obuProvTradeSuccNum = 0;  // 本省交易成功数量
    QString ffObuCardNo = "";     // EF04文件中的卡号等信息
    int noCardTimes = 0;          // 标签无卡累计次数

    // 卡介质相关成员
    QString cpcCardID = ""; // CPC卡号
    int cpcElec = 0;        // CPC卡电量

    // 入口相关成员
    QString enRoadNet = ""; // 入口网络编码

    // 出口相关成员
    int exNetID = 3501;                                   // 出口路网编号（默认3501）
    QString exStation = "";                               // 出口收费站ID
    QString exStationName = "";                           // 出口收费站名称
    int exLane = 0;                                       // 出口车道
    QDateTime exTime = QDateTime::currentDateTime();      // 出口时间
    QString exOperatorID = "";                            // 出口操作人员ID
    QString exOperator = "";                              // 出口操作人员名称
    int exShiftID = 0;                                    // 出口班次ID
    QDateTime exShiftDate = QDateTime::currentDateTime(); // 出口班次时间
    int exVehClass = 0;                                   // 出口车型
    int exVehStatus = 0;                                  // 出口车种
    QString exVehPlate = "";                              // 出口车牌
    int provinceNum = 0;                                  // 通行省份个数
    QString provEnHex = "";                               // 本省入口编码

    // 称重相关成员
    int exAxises = 0;        // 出口轴数
    int exTotalWeight = 0;   // 出口总轴重
    int axisGroups = 0;      // 轴组数
    int origAxisType = 0;    // 原始轴型
    int axisType = 0;        // 轴型
    QString axisWeight = ""; // 各轴轴重
    int origWeight = 0;      // 原始总轴重
    int tollWeight = 0;      // 收费总重
    double exceedRate = 0.0; // 收费超限比例
    int weightClass = 0;     // 重量类型
    int tollType = 0;        // 收费类型

    // 票据相关成员
    int ticketNum = 0;     // 电脑票据号
    QString billCode = ""; // 账单编号
    int ticketYear = 0;    // 票据年份

    //远程获取入口介质信息
    int enMediaType = 0;
    QString enMediaNum = "";  //通行介质(CPC卡号、OBU序列号)
    QString enETCCardID = ""; //ETC卡号

    OutTrade() {}
    ~OutTrade() override {}
    void tradeInit() override
    {
        this->~OutTrade();
        new (this) OutTrade();
    }

public:
    Q_PROPERTY(QString TradeID MEMBER tradeID)             //STATIONID + LANEID + TIME(YYYYMMDDhhmmss) + SEQ
    Q_PROPERTY(int SeqNum MEMBER seqNum)                   //数据序列号
    Q_PROPERTY(QString PassID MEMBER passID)               //通行ID
    Q_PROPERTY(int TollRateVer MEMBER tollRateVer)         //费率版本号
    Q_PROPERTY(int CardNetID MEMBER cardNetID)             //卡网络编号，识别省份
    Q_PROPERTY(int CardType MEMBER cardType)               //卡类型 参照EM_CardType
    Q_PROPERTY(int CardBizType MEMBER cardBizType)         //卡业务类型 参照EM_CardBizType
    Q_PROPERTY(QString CardID MEMBER cardID)               //卡号，"0"为无卡
    Q_PROPERTY(int EnNetID MEMBER enNetID)                 //入口路网编号
    Q_PROPERTY(QString EnStation MEMBER enStation)         //入口收费站ID
    Q_PROPERTY(QString EnStationName MEMBER enStationName) //入口收费站名称
    Q_PROPERTY(QDateTime EnTime MEMBER enTime)             //入口时间 YYYY-MM-DD hh:mm:ss
    Q_PROPERTY(QString EnOperatorID MEMBER enOperatorID)   //入口操作人员ID
    Q_PROPERTY(int EnVehClass MEMBER enVehClass)           //入口车型
    Q_PROPERTY(QString EnVehPlate MEMBER enVehPlate)       //入口车牌
    Q_PROPERTY(int ExNetID MEMBER exNetID)                 //出口路网编号，默认3501
    Q_PROPERTY(QString ExStation MEMBER exStation)         //出口收费站ID
    Q_PROPERTY(QString ExStationName MEMBER exStationName) //出口收费站名称
    Q_PROPERTY(int ExLane MEMBER exLane)                   //出口车道
    Q_PROPERTY(QDateTime ExTime MEMBER exTime)             //出口时间
    Q_PROPERTY(QString ExOperatorID MEMBER exOperatorID)   //出口操作人员ID
    Q_PROPERTY(QString ExOperator MEMBER exOperator)       //出口操作人员名称
    Q_PROPERTY(int ExShiftID MEMBER exShiftID)             //出口班次ID
    Q_PROPERTY(QDateTime ExShiftDate MEMBER exShiftDate)   //出口班次时间 YYYY-MM-DD
    Q_PROPERTY(int ExVehClass MEMBER exVehClass)           //出口车型
    Q_PROPERTY(int ExVehStatus MEMBER exVehStatus)         //出口车种 参考EM_VehStatus
    Q_PROPERTY(QString ExVehPlate MEMBER exVehPlate)       //出口车牌
    Q_PROPERTY(QString OBUPlate MEMBER obuPlate)           //OBU车牌
    Q_PROPERTY(QString CardPlate MEMBER cardPlate)
    Q_PROPERTY(int VehSpeed MEMBER vehSpeed)
    Q_PROPERTY(int AxisGroups MEMBER axisGroups)
    Q_PROPERTY(int Axises MEMBER exAxises)
    Q_PROPERTY(int OrigAxisType MEMBER origAxisType)
    Q_PROPERTY(int AxisType MEMBER axisType)
    Q_PROPERTY(QString AxisWeight MEMBER axisWeight)
    Q_PROPERTY(int OrigWeight MEMBER origWeight)
    Q_PROPERTY(int TotalWeight MEMBER exTotalWeight)
    Q_PROPERTY(int TollWeight MEMBER tollWeight)
    Q_PROPERTY(int AxisLimit MEMBER axisLimit)
    Q_PROPERTY(double ExceedRate MEMBER exceedRate)
    Q_PROPERTY(int WeightClass MEMBER weightClass)
    Q_PROPERTY(int TollType MEMBER tollType)
    Q_PROPERTY(double ShouldPay MEMBER shouldPay)
    Q_PROPERTY(int DiscountType MEMBER discountType)
    Q_PROPERTY(double Discount MEMBER discount)
    Q_PROPERTY(double FactPay MEMBER factPay)
    Q_PROPERTY(int MerRebateType MEMBER merRebateType)
    Q_PROPERTY(double RebateRatio MEMBER rebateRatio)
    Q_PROPERTY(double MerShouldPay MEMBER merShouldPay)
    Q_PROPERTY(double RrebatedPay MEMBER rebatedPay)
    Q_PROPERTY(double SpecialPay MEMBER specialPay)
    Q_PROPERTY(int TicketNum MEMBER ticketNum)
    Q_PROPERTY(int PayType MEMBER payType)
    Q_PROPERTY(int DealStatus MEMBER dealStatus)
    Q_PROPERTY(int DeviceStatus MEMBER deviceStatus)
    Q_PROPERTY(int RecordType MEMBER recordType)
    Q_PROPERTY(int ChangeType MEMBER changeType)
    Q_PROPERTY(double ChangedFee MEMBER changedFee)
    Q_PROPERTY(int PeccType MEMBER peccType)
    Q_PROPERTY(int ChkPoint MEMBER chkPoint)
    Q_PROPERTY(int SpendTime MEMBER spendTime)
    Q_PROPERTY(int IsValid MEMBER isValid)
    Q_PROPERTY(int ScrapTickets MEMBER scrapTickets)
    Q_PROPERTY(int OBUID MEMBER obuID)
    Q_PROPERTY(QString TermCode MEMBER termCode)
    Q_PROPERTY(int CardTradeNo MEMBER cardTradeNo)
    Q_PROPERTY(int TermTradeNo MEMBER termTradeNo)
    Q_PROPERTY(uint RestMoney MEMBER restMoney)
    Q_PROPERTY(double TAC MEMBER tac)
    Q_PROPERTY(QString OBUSerialNum MEMBER obuSerialNum)
    Q_PROPERTY(int OBUStatus MEMBER obuStatus)
    Q_PROPERTY(int ProvinceID MEMBER provinceID)
    Q_PROPERTY(int Verifycode MEMBER verifycode)
    Q_PROPERTY(double TSVehicle MEMBER tsVehicle)
    Q_PROPERTY(QString Reserve MEMBER reserve)
    Q_PROPERTY(int DataMonth MEMBER dataMonth)
    Q_PROPERTY(int EnLane MEMBER enLane)
    Q_PROPERTY(uint PostBalance MEMBER postBalance)
    Q_PROPERTY(int Algorithm MEMBER algorithm)
    Q_PROPERTY(QString CPCCardID MEMBER cpcCardID)
    Q_PROPERTY(int CPCElec MEMBER cpcElec)
    Q_PROPERTY(double OpFactPay MEMBER opFactPay)
    Q_PROPERTY(int ETCCardVer MEMBER etcCardVer)
    Q_PROPERTY(int OBUContractVer MEMBER obuContractVer)
    Q_PROPERTY(QString Reserve2 MEMBER reserve2)
    Q_PROPERTY(QString AppVersion MEMBER appVersion)
    Q_PROPERTY(int RecordType2 MEMBER recordType2)
    Q_PROPERTY(int TollSupport MEMBER tollSupport)
    Q_PROPERTY(int EnAxisCount MEMBER enAxisCount)
    Q_PROPERTY(int EnTotalWeight MEMBER enTotalWeight)
    Q_PROPERTY(int UserType MEMBER userType)
    Q_PROPERTY(int OBUTotalTradeSuccNum MEMBER obuTotalTradeSuccNum)
    Q_PROPERTY(int ProvinceNum MEMBER provinceNum)
    Q_PROPERTY(int NoCardTimes MEMBER noCardTimes)
    Q_PROPERTY(int OBUPayFeeSum MEMBER obuPayFeeSum)
    Q_PROPERTY(int OBUDiscountFeeSum MEMBER obuDiscountFeeSum)
    Q_PROPERTY(int OBUMileage MEMBER obuMileage)
    Q_PROPERTY(QString ProvEnHex MEMBER provEnHex)
    Q_PROPERTY(int OBUProvFeeSum MEMBER obuProvFeeSum)
    Q_PROPERTY(int OBUProvFactSum MEMBER obuProvFactSum)
    Q_PROPERTY(int OBUProvTradeSuccNum MEMBER obuProvTradeSuccNum)
    Q_PROPERTY(QString FFObuCardNo MEMBER ffObuCardNo)
    Q_PROPERTY(int CardFeeSum MEMBER cardFeeSum)
    Q_PROPERTY(int ShortFee MEMBER shortFee)
    Q_PROPERTY(int ShortFeeMileage MEMBER shortFeeMileage)
    Q_PROPERTY(double FeeRate MEMBER feeRate)
    Q_PROPERTY(int ExitFeeType MEMBER exitFeeType)
    Q_PROPERTY(int TransPayType MEMBER transPayType)
    Q_PROPERTY(QString SplitProvince MEMBER splitProvince)
    Q_PROPERTY(QString SpecialType MEMBER specialType)
    Q_PROPERTY(QString Merchant MEMBER merchant)
    Q_PROPERTY(QString CNLaneID MEMBER cnLaneID)
    Q_PROPERTY(QString EnRoadNet MEMBER enRoadNet)
    Q_PROPERTY(int OpenFlag MEMBER openFlag)
    Q_PROPERTY(int LocalFeeMileage MEMBER localFeeMileage)
    Q_PROPERTY(int OBUVehClass MEMBER obuVehClass)
    Q_PROPERTY(QString BillCode MEMBER billCode)
    Q_PROPERTY(int TicketYear MEMBER ticketYear)
    Q_PROPERTY(int OpeateSign MEMBER opeateSign)
    Q_PROPERTY(int enMediaType MEMBER enMediaType)
    Q_PROPERTY(QString enMediaNum MEMBER enMediaNum)
    Q_PROPERTY(QString enETCCardID MEMBER enETCCardID)
};

class InTrade : public OneTrade
{
    Q_OBJECT
public:
    InTrade() {}
    ~InTrade() override {}
    void tradeInit() override
    {
        this->~InTrade();
        new (this) InTrade();
    }

public:
    int tradeType = 0;         // 交易类型
    bool isHoliday = false;    // 是否节假日车
    bool isFree = false;       // 是否免费车
    bool isBlack = false;      // 是否黑名单
    bool isMixETC = false;     // 是否混合车道天线交易
    bool isPlateMatch = false; // 抓拍车牌与OBU车牌是否一致
    QString scrapVehPlate;     // 作废车牌

public:
    Q_PROPERTY(QString TradeID MEMBER tradeID)
    Q_PROPERTY(int SeqNum MEMBER seqNum)
    Q_PROPERTY(int CardNetID MEMBER cardNetID)
    Q_PROPERTY(int CardType MEMBER cardType)
    Q_PROPERTY(int CardBizType MEMBER cardBizType)
    Q_PROPERTY(QString CardID MEMBER cardID)
    Q_PROPERTY(int EnNetID MEMBER enNetID)
    Q_PROPERTY(QString EnStation MEMBER enStation)
    Q_PROPERTY(QString StationName MEMBER enStationName)
    Q_PROPERTY(int EnLane MEMBER enLane)
    Q_PROPERTY(QDateTime EnTime MEMBER enTime)
    Q_PROPERTY(QString EnOperatorID MEMBER enOperatorID)
    Q_PROPERTY(QString EnOperator MEMBER enOperator)
    Q_PROPERTY(int EnShiftID MEMBER enShiftID)
    Q_PROPERTY(QDateTime EnShiftDate MEMBER enShiftDate)
    Q_PROPERTY(int EnVehClass MEMBER enVehClass)
    Q_PROPERTY(int EnVehStatus MEMBER enVehStatus)
    Q_PROPERTY(QString EnVehPlate MEMBER enVehPlate)
    Q_PROPERTY(QString OBUPlate MEMBER obuPlate)
    Q_PROPERTY(QString CardPlate MEMBER cardPlate)
    Q_PROPERTY(int PeccType MEMBER peccType)
    Q_PROPERTY(int DealStatus MEMBER dealStatus)
    Q_PROPERTY(int DeviceStatus MEMBER deviceStatus)
    Q_PROPERTY(int RecordType MEMBER recordType)
    Q_PROPERTY(int IsValid MEMBER isValid)
    Q_PROPERTY(int ScrapTickets MEMBER scrapTickets)
    Q_PROPERTY(int OBUID MEMBER obuID)
    Q_PROPERTY(QString TermCode MEMBER termCode)
    Q_PROPERTY(int CardTradeNo MEMBER cardTradeNo)
    Q_PROPERTY(int TermTradeNo MEMBER termTradeNo)
    Q_PROPERTY(double RestMoney MEMBER restMoney)
    Q_PROPERTY(double TAC MEMBER tac)
    Q_PROPERTY(QString OBUSerialNum MEMBER obuSerialNum)
    Q_PROPERTY(int OBUStatus MEMBER obuStatus)
    Q_PROPERTY(int SpendTime MEMBER spendTime)
    Q_PROPERTY(int VehSpeed MEMBER vehSpeed)
    Q_PROPERTY(int ProvinceID MEMBER provinceID)
    Q_PROPERTY(int Verifycode MEMBER verifycode)
    Q_PROPERTY(int DataMonth MEMBER dataMonth)
    Q_PROPERTY(QString Reserve MEMBER reserve)
    Q_PROPERTY(int Algorithm MEMBER algorithm)
    Q_PROPERTY(int EtcCardVer MEMBER etcCardVer)
    Q_PROPERTY(int OBUContractVer MEMBER obuContractVer)
    Q_PROPERTY(QString AppVersion MEMBER appVersion)
    Q_PROPERTY(int AxisCount MEMBER enAxisCount)
    Q_PROPERTY(int TotalWeight MEMBER enTotalWeight)
    Q_PROPERTY(int UserType MEMBER userType)
    Q_PROPERTY(int TransFee MEMBER transFee)
    Q_PROPERTY(QString PassID MEMBER passID)
    Q_PROPERTY(QString SpecialType MEMBER specialType)
    Q_PROPERTY(QString CNLaneID MEMBER cnLaneID)
    Q_PROPERTY(int AxisLimit MEMBER axisLimit)
    Q_PROPERTY(int OBUVehClass MEMBER obuVehClass)
};

class ETCTrade : public QObject
{
    Q_OBJECT
public:
    QVariant oneTrade;    //根据出入口来，出口则是OutTrade，入口则是InTrade
    QVariant obuEF04Data; //Meidia::OBUEF04Data;
    int errCode = 0;
    QString errDesc = "";
    QStringList feeScrLines;
    ETCTrade() {}
    ~ETCTrade() override {}

public:
    Q_PROPERTY(QVariant OneTrade MEMBER oneTrade)
    Q_PROPERTY(QVariant OBUEF04Data MEMBER obuEF04Data)
    Q_PROPERTY(int ErrCode MEMBER errCode)
    Q_PROPERTY(QString ErrDesc MEMBER errDesc)
    Q_PROPERTY(QStringList FeeScrLines MEMBER feeScrLines)
};
} // namespace Trade

//==============================================================================
// 设备数据
//==============================================================================
namespace DevData {

// 小黄人车型信息
struct ST_VehRecoTypeInfo
{
    QString VehPlate = ""; // 车牌
    int VehColor = 0;      // 车牌颜色： 0蓝色; 1黄色; 2黑色; 3白色; 4渐变绿色; 5黄绿双拼色; 6蓝白渐变色; 9未确定
    QString VehTime = "";  // 过车时间，BCD码， YYYYMMDDhhmmss
    int VehClass = 0;      // 车型： 客车1-4   货车11-16   专项作业车21-26
    QString AxleType = ""; // 轴型，字符串格式，并以\0结尾
    int AxleCount = 0;     // 轴数
    int TotalLength = 0;   // 车货总长度，单位厘米 BCD码       0~999999，无法识别填0x000000
    int TotalWidth = 0;    // 总宽度，单位厘米 BCD码，          0~999999，无法识别填0x000000
    int TotalHeight = 0;   // 总高度，单位厘米 BCD码，     0~999999，无法识别填0x000000
    int ExtFlag = 0; // 扩展标识。默认0 无扩展标识  从低位开始按位存放扩展信息，最多32个扩展信息，可叠加。1 拖挂车标识，2 两客一危标识
    QString Reserve = ""; //备用
};

// 小黄人图片
struct ST_VehRecoImageInfo
{
    QString VehPlate = ""; // 车牌，gb2312字符串格式
    int VehColor = 0;      // 车牌颜色： 0蓝色; 1黄色; 2黑色; 3白色; 4渐变绿色; 5黄绿双拼色; 6蓝白渐变色; 9未确定
    QString VehTime = "";  // 过车时间，YYYYMMDDhhmmss
    int ImageType = 0;     // 类型：1车头JPG图像、2车尾JPG图像、3车身JPG图像、4短视频MP4，其他值不处理
    int ImageLen = 0;      // 长度：图像不超过300KB，短视频不超过1MB。
    QByteArray ImageInfo;  // 图像或短视频数据
    QString Reserve = "";  // 备用
};

// 小黄人心跳信息
struct ST_VehRecoHeartInfo
{
    QString CameraIP;   //识别器IP地址
    QString StationHex; //收费站编码
    int LaneNum;        //车道号
    uint CurrentTime;   //当前时间(unix时间)
    int WorkStatus;     //  工作状态: 0.正常 1.故障 2.时间同步异常(时差大于10min) 4.识别率异常（小于98%）
    QString Reserve;    //备用
};

// 称重设备返回数据
struct ST_VehicleWeightInfo
{
    int cmd = 0;    // 命令
    int idx = 0;    // 序号
    int err = 0;    // 0成功 1失败
    int carnum = 0; // 缓冲区数量
    int status = 0; // 设备状态
    int year = 0;
    int mon = 0;
    int day = 0;
    int hour = 0;
    int min = 0;
    int sec = 0;          //
    int flag = 0;         // 超限标志
    int velocity = 0;     // 速度
    int acceleration = 0; // 加速度
    int axnum = 0;        // 轴数
    int axgnum = 0;       // 轴组数
    QList<int> weight;    // 轴重
    QList<int> axtype;    // 轴型
};

//处理后用于业务的称重
struct ST_WeightInfo
{
    int index;               // 队列号
    int vehSpeed;            // 速度Km/h
    QDateTime weightTime;    // 称重返回时间
    int axisGNum;            // 轴组数 3
    int axisNum;             // 轴数 5
    int axisType;            // 轴型 115
    int origAxisType;        // 原始轴型 127 （即人工修改轴型此数据不动）
    int origTollWeight;      // 原始重量（即分割称重前的重量）
    int tollWeight;          // 车辆总重量 1800
    QString groupTollWeight; // 各轴组重 550|560|600
    QString axisTollWeight;  // 各轴重 550|560|600
    int lawLimit;            // 超限重量（原始版）
    int factLimit;           // 超限重量（提升0.05版）
    qreal exceedRate;        // 超限率
    int dealType;            // 是否手动插入 0:称重设备返回  1:手动录入车辆轴型 2:手工录入车辆轴限  4:手工插入称重
    int userType;            // 用户类型 0x1B是拖挂车 0x1A是大件运输车
    QString vehPlate;        // 预留抓拍
    bool isLawVeh = false;   // 是否路政超限
    bool isManualUserType = false; // 是否人工确认过 是否已经确认过
    bool allowPass = false;        // 是否允许过车（入口用）
    bool isRareAxis = false;       // 是否罕见轴型
    bool isValid = false;          // 用于判断当前称重是否有效，默认构造false
    int changeFlag;                // 2 称重降级时人工插入  4 改轴后轴型不一致   8 改轴后轴数不一致
};

//io数据
struct ST_IOVdStatus
{
    int coilNum = 0;
    ushort coilStatus = 0;
    QString reserve = "";
};

// 卡机相关数据
class MsgInfo : public QObject
{
    Q_OBJECT
public:
    ushort LParamLo = 0; //原Word类型（16位无符号整数）转为ushort，初始化为0
    ushort LParamHi = 0; //原Word类型转为ushort，初始化为0

    MsgInfo() {}
    ~MsgInfo() override {}

public:
    Q_PROPERTY(ushort LParamLo MEMBER LParamLo)
    Q_PROPERTY(ushort LParamHi MEMBER LParamHi)
};

class BoxStatus : public QObject
{
    Q_OBJECT
public:
    int Status = 0;          //原BYTE类型转为int，初始化为0（卡位状态:0正常 1故障 2有坏卡）
    bool HasCardBag = false; //是否装有卡夹（bool类型保持，初始化为false）
    int RestCards = 0;       //剩余卡数(包括天线区卡)（int类型保持，初始化为0）
    bool HasAnnCard = false; //天线区是否有卡（bool类型保持，初始化为false）

    BoxStatus() {}
    ~BoxStatus() override {}

public:
    Q_PROPERTY(int Status MEMBER Status)
    Q_PROPERTY(bool HasCardBag MEMBER HasCardBag)
    Q_PROPERTY(int RestCards MEMBER RestCards)
    Q_PROPERTY(bool HasAnnCard MEMBER HasAnnCard)
};

class RobotStatus : public QObject
{
    Q_OBJECT
public:
    int AboveBoxNo = 0;     //原BYTE类型转为int，初始化为0（上工位当前卡机: 0无卡, 1或2号卡机）
    int BelowBoxNo = 0;     //原BYTE类型转为int，初始化为0（下工位当前卡机: 0无卡 3或4号卡机）
    QVariantList BoxStatus; //原TBoxStatus数组转为QVariantList
    RobotStatus() {}

    ~RobotStatus() override {}

public:
    Q_PROPERTY(int AboveBoxNo MEMBER AboveBoxNo)
    Q_PROPERTY(int BelowBoxNo MEMBER BelowBoxNo)
    Q_PROPERTY(QVariantList BoxStatus MEMBER BoxStatus)
};

class CardRobotStatus : public QObject
{
    Q_OBJECT
public:
    int upWork = 0;      //原BYTE类型转为int，初始化为0（上工位当前卡机：30H 无；31H 为 1#机；32H 为 2#机）
    int downWork = 0;    //原BYTE类型转为int，初始化为0（下工位当前卡机：30H 无；33H 为 3#机；34H 为 4#机）
    int boxStatus1 = 0;  //原BYTE类型转为int，初始化为0（1#卡机状态：30H 正常；31H 故障；32H 有坏卡；33H 离线）
    int hasCardBag1 = 0; //原BYTE类型转为int，初始化为0（1#卡夹状态：30H 已装上；31H 已卸下）
    uint restCards1 = 0; //原UINT类型转为uint，初始化为0（1#卡机卡计数：如“050”表示50张）
    int hasAnnCard1 = 0; //原BYTE类型转为int，初始化为0（1#轨道有卡状态：30H 无卡；31H 天线有卡；32H 卡口有卡）
    int boxStatus2 = 0;  //原BYTE类型转为int，初始化为0（2#卡机状态，同1#）
    int hasCardBag2 = 0; //原BYTE类型转为int，初始化为0（2#卡夹状态，同1#）
    uint restCards2 = 0; //原UINT类型转为uint，初始化为0（2#卡机卡计数）
    int hasAnnCard2 = 0; //原BYTE类型转为int，初始化为0（2#轨道有卡状态，同1#）
    int boxStatus3 = 0;  //原BYTE类型转为int，初始化为0（3#卡机状态，同1#）
    int hasCardBag3 = 0; //原BYTE类型转为int，初始化为0（3#卡夹状态，同1#）
    uint restCards3 = 0; //原UINT类型转为uint，初始化为0（3#卡机卡计数）
    int hasAnnCard3 = 0; //原BYTE类型转为int，初始化为0（3#轨道有卡状态，同1#）
    int boxStatus4 = 0;  //原BYTE类型转为int，初始化为0（4#卡机状态，同1#）
    int hasCardBag4 = 0; //原BYTE类型转为int，初始化为0（4#卡夹状态，同1#）
    uint restCards4 = 0; //原UINT类型转为uint，初始化为0（4#卡机卡计数）
    int hasAnnCard4 = 0; //原BYTE类型转为int，初始化为0（4#轨道有卡状态，同1#）

    CardRobotStatus() {}
    ~CardRobotStatus() override {}

public:
    Q_PROPERTY(int upWork MEMBER upWork)
    Q_PROPERTY(int downWork MEMBER downWork)
    Q_PROPERTY(int boxStatus1 MEMBER boxStatus1)
    Q_PROPERTY(int hasCardBag1 MEMBER hasCardBag1)
    Q_PROPERTY(uint restCards1 MEMBER restCards1)
    Q_PROPERTY(int hasAnnCard1 MEMBER hasAnnCard1)
    Q_PROPERTY(int boxStatus2 MEMBER boxStatus2)
    Q_PROPERTY(int hasCardBag2 MEMBER hasCardBag2)
    Q_PROPERTY(uint restCards2 MEMBER restCards2)
    Q_PROPERTY(int hasAnnCard2 MEMBER hasAnnCard2)
    Q_PROPERTY(int boxStatus3 MEMBER boxStatus3)
    Q_PROPERTY(int hasCardBag3 MEMBER hasCardBag3)
    Q_PROPERTY(uint restCards3 MEMBER restCards3)
    Q_PROPERTY(int hasAnnCard3 MEMBER hasAnnCard3)
    Q_PROPERTY(int boxStatus4 MEMBER boxStatus4)
    Q_PROPERTY(int hasCardBag4 MEMBER hasCardBag4)
    Q_PROPERTY(uint restCards4 MEMBER restCards4)
    Q_PROPERTY(int hasAnnCard4 MEMBER hasAnnCard4)
};

class CardBoxInfo : public QObject
{
    Q_OBJECT
public:
    uint boxNo1 = 0;     //原UINT类型转为uint，初始化为0（1#卡机卡夹编号：无为0x00,0x00,0x00,0x00）
    ushort cardNum1 = 0; //原USHORT类型转为ushort，初始化为0（1#卡机卡数信息）
    uint boxNo2 = 0;     //原UINT类型转为uint，初始化为0（2#卡机卡夹编号）
    ushort cardNum2 = 0; //原USHORT类型转为ushort，初始化为0（2#卡机卡数信息）
    uint boxNo3 = 0;     //原UINT类型转为uint，初始化为0（3#卡机卡夹编号）
    ushort cardNum3 = 0; //原USHORT类型转为ushort，初始化为0（3#卡机卡数信息）
    uint boxNo4 = 0;     //原UINT类型转为uint，初始化为0（4#卡机卡夹编号）
    ushort cardNum4 = 0; //原USHORT类型转为ushort，初始化为0（4#卡机卡数信息）

    CardBoxInfo() {}
    ~CardBoxInfo() override {}

public:
    Q_PROPERTY(uint boxNo1 MEMBER boxNo1)
    Q_PROPERTY(ushort cardNum1 MEMBER cardNum1)
    Q_PROPERTY(uint boxNo2 MEMBER boxNo2)
    Q_PROPERTY(ushort cardNum2 MEMBER cardNum2)
    Q_PROPERTY(uint boxNo3 MEMBER boxNo3)
    Q_PROPERTY(ushort cardNum3 MEMBER cardNum3)
    Q_PROPERTY(uint boxNo4 MEMBER boxNo4)
    Q_PROPERTY(ushort cardNum4 MEMBER cardNum4)
};

class PaperStatus : public QObject
{
    Q_OBJECT
public:
    int WorkStation = 0;   //原BYTE类型转为int，初始化为0（工位：0默认；1上工位；2下工位）
    int Cause = 0;         //原BYTE类型转为int，初始化为0（原因：0正常取走纸券；1设备状态）
    QString PaperNum = ""; //原char数组转为QString，初始化为空（纸券号，长度20）
    int StatusUp = 0;      //原BYTE类型转为int，初始化为0（上工位打印机状态）
    int StatusDown = 0;    //原BYTE类型转为int，初始化为0（下工位打印机状态）

    PaperStatus() {}
    ~PaperStatus() override {}

public:
    Q_PROPERTY(int WorkStation MEMBER WorkStation)
    Q_PROPERTY(int Cause MEMBER Cause)
    Q_PROPERTY(QString PaperNum MEMBER PaperNum)
    Q_PROPERTY(int StatusUp MEMBER StatusUp)
    Q_PROPERTY(int StatusDown MEMBER StatusDown)
};

// 缴费机相关数据
class SPTOperation : public QObject
{
    Q_OBJECT
public:
    int cmd = 0;                   // 1 应答  2 插卡  3  用户选择  4  卡取走  5 状态（int类型保持，初始化为0）
    int pos = 0;                   // 工位  1 上   2 下（int类型保持，初始化为0）
    int option = 0;                // 选项值（int类型保持，初始化为0）
    QString code = "";             // 支付码等（原char数组转为QString，初始化为空，长度64）
    int rsv = 0;                   // 预留字段（int类型保持，初始化为0）
    QByteArray pic = QByteArray(); // 二进制数据（原unsigned char数组转为QByteArray，初始化为空，长度409600）

    SPTOperation() {}
    ~SPTOperation() override {}

public:
    Q_PROPERTY(int cmd MEMBER cmd)
    Q_PROPERTY(int pos MEMBER pos)
    Q_PROPERTY(int option MEMBER option)
    Q_PROPERTY(QString code MEMBER code)
    Q_PROPERTY(int rsv MEMBER rsv)
    Q_PROPERTY(QByteArray pic MEMBER pic)
};
} // namespace DevData

//==============================================================================
// 计费服务返回
//==============================================================================
namespace ServiceBack {
class AppUpdateInfo : public QObject
{
    Q_OBJECT
public:
    AppUpdateInfo() {}
    ~AppUpdateInfo() override {}

    int upgradeStatus = 0;                  //升级状态
    QString upgradeURL = "";                //升级地址
    QString upgradeContent = "";            //升级内容
    QString upgradeVersion = "";            //升级版本
    QString libVersion = "";                //计费动态库版本
    QString shortFeeVersion = "";           //兜底费率
    QString completeBlackListVersion = "";  //全量黑名单
    QString incrementBlackListVersion = ""; //增量版本
    QString feeOutVersion = "";             //对外费率版本
    QString notenableSpcRateVersion = "";   //待生效费率
    QString notenableSpcRateRecTime = "";   //待生效时间
public:
    Q_PROPERTY(int upgradeStatus MEMBER upgradeStatus)
    Q_PROPERTY(QString upgradeURL MEMBER upgradeURL)
    Q_PROPERTY(QString upgradeContent MEMBER upgradeContent)
    Q_PROPERTY(QString upgradeVersion MEMBER upgradeVersion)
    Q_PROPERTY(QString libVersion MEMBER libVersion)
    Q_PROPERTY(QString shortFeeVersion MEMBER shortFeeVersion)
    Q_PROPERTY(QString completeBlackListVersion MEMBER completeBlackListVersion)
    Q_PROPERTY(QString incrementBlackListVersion MEMBER incrementBlackListVersion)
    Q_PROPERTY(QString feeOutVersion MEMBER feeOutVersion)
    Q_PROPERTY(QString notenableSpcRateVersion MEMBER notenableSpcRateVersion)
    Q_PROPERTY(QString notenableSpcRateRecTime MEMBER notenableSpcRateRecTime)
};
} //namespace ServiceBack
Q_DECLARE_METATYPE(DevData::ST_VehicleWeightInfo)
Q_DECLARE_METATYPE(DevData::ST_WeightInfo)
Q_DECLARE_METATYPE(DevData::ST_IOVdStatus)
Q_DECLARE_METATYPE(DevData::ST_VehRecoHeartInfo)
Q_DECLARE_METATYPE(DevData::ST_VehRecoImageInfo)
Q_DECLARE_METATYPE(DevData::ST_VehRecoTypeInfo)
