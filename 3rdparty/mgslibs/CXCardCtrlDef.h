//--------------------------------------------------------------------------
//Module:M1卡和CPU卡控制模块(DLL)  Author:Huanglr  FromDate:2014-09-11(重构)
//Remark:注意(1)读卡器可打开一次。(2)在opencard与closecard之间可以有多个函数调用
//--------------------------------------------------------------------------
//2019-10-25
//读0019文件时,传进去什么版本就按什么版本读写
//写出口时,EnStation_h为空时,不写入口站代码
//
#ifndef UCARDCTRLDEF_H_                          
#define UCARDCTRLDEF_H_

//#include "windows.h"
#include <QtCore>

#define CARD_TYPE_PASSING   10  //通行卡(M1卡)
#define CARD_TYPE_PERSON    20  //身份卡(M1卡)
#define CARD_TYPE_PREPAID   22  //储值卡(CPU卡)
#define CARD_TYPE_CREDIT    23  //记帐卡(CPU卡)
#define CARD_TYPE_CPC       15  //CPC卡

#define ERR_IC_ENCHECKSUM      -31 //IC通行卡入口数据校验错
#define ERR_IC_PLATECHECKSUM   -32 //IC通行卡车牌数据校验错
#define ERR_IC_SRVCHECKSUM     -33 //IC通行卡服务区信息校验错
#define ERR_IC_ENNUMDEC        -34 //IC卡入口信息加密校验错
#define ERR_IC_NUMDEC_DLL      -41 //载入NumDec.dll出错

#define API_TYPE_FJ     0   //福建接口
#define API_TYPE_ICC    1   //部接口

typedef unsigned char BYTE;
typedef unsigned int  UINT;
typedef unsigned short USHORT;

//----------------普通通行卡数据 --------------------------------------------
//通行卡基础数据
typedef struct tag_BaseInfo
{
   UINT CardSerialNum;   //卡序列号
   BYTE CardType;
   UINT CardNum;         //卡号
   char StationID_h[5];  //收费站代码(16进制字符串,下同)
   UINT Operator;
   char OperatorName[8];
} TBaseInfo;

//通行卡入口信息
typedef struct tag_EnInfo{
   char EnNetNo_h[5];    //入口收费网络号(16进制字符串，下同)
   char EnStation_h[5];  //入口收费站
   BYTE   EnLane;         //入口车道
   UINT   EnTime;        //入口时间
   UINT   EnOperator;    //入口收费员编码
   BYTE  EnShift;        //入口班次
   BYTE  DealStatus;     //入口车情*(货车0,客车1)
   BYTE  EnVehClass;     //入口车型+卡流通状态
   BYTE  CardStatus;     //卡流通状态* 0.发出的卡 1.回收的卡
   char  EnVehPlate[14]; //车牌
   BYTE  ChkPoint;       //最多8个标识站
} TEnInfo;

//通行卡出口信息
typedef struct tag_ExInfo{
   USHORT FactPay;     //费额(最大65535)
   char ExStation_h[5];   //出口收费站
   BYTE  ExLane;       //出口车道
   UINT   ExOperator;  //出口收费员
   UINT   ExTime;      //出口时间
   BYTE  ExShift;      //出口班次
   BYTE  DealStatus;   //出口车情
   BYTE  ExVehClass;   //出口车型
} TExInfo;

//服务区停留信息
typedef struct tag_M1ServiceInfo{
   USHORT ServiceID;  //最后一个服务区
   UINT   EnTime;     //进入时间
   UINT   ExTime;     //出口时间
   UINT   StayTime;   //总停留时间
} TM1ServiceInfo;


//---------ETC卡数据格式------------------------------------------------------
//PSAM卡获取随机数信息
typedef struct tag_PsamRand
{
   char  hexCardID[21];   //10字节PSAM卡号(16进制字符串)
   BYTE  CardVersion;     //1字节卡片版本号
   BYTE  CardKeyType;     //1字节卡密钥类型
   char  hexRand[33];     //16字节随机数(16进制字符串)
}TPsamRand;

//ETC卡(闽通卡)基本信息(0015+000C+0002)
typedef struct tag_CCBaseData
{
   char hexPubOrg[17];    //福建3501001(8B)
   BYTE CardType;         //卡类型
   BYTE CardVer;          //卡版本号
   char hexCardNet[5];    //卡网络编号(2B)(福建为0x2301)
   char hexCardNo[17];    //卡内部编号(8B)
   char hexStartDate[9];  //启用日期(4B)
   char hexStopDate[9];   //停用日期(4B)
   char VehPlate[18];     //车牌号(12B+2B中文颜色)
   BYTE VehClass;         //车型(0015文件,新版本增加)
   BYTE UserType;         //用户类型00-普通用户
   bool IsBinding;        //是否绑定车牌(不记名卡)
   bool IsFree;           //是否免费车
   bool IsTruck;          //是否货车
   BYTE Seats;            //座位数
   UINT RemainMoney;      //余额
} TCCBaseData;

#define VERSION_0019_00  0x00
#define VERSION_0019_4F  0x4F
#define VERSION_0019_5F  0x5F
#define VERSION_0019_3F  0x3F


//ETC卡(闽通卡)过车数据(0019)
typedef struct tag_CCTollData
{
   BYTE CappType;        //复合应用类型，标准取值0xAA
   BYTE RecLen;          //记录长度，标准取值0x29
   BYTE LockFlag;        //应用锁定标记 0x00
   char EnStation_h[5];     //入口站
   BYTE EnLane;          //出入口车道[**共用字段]
   UINT EnTime;          //入口时间(**新版本使用unix时间)
   BYTE VehClass;        //车型
   BYTE CardStatus;      //入出口状态3-入口 4出口
   UINT EnOperator;      //出入口收费员工号(3B)[**共用字段]
   char VehPlate[15];    //出入口抓拍车牌14+1[**共用字段]
   BYTE PlateColor;      //车牌颜色(**新增字段 )
   char ExStation_h[5];     //出口站
   UINT ExTime;          //出口时间(**新版本使用unix时间)
   char ChkPoint[6];     //出口标识站(6B,**新版无用字段)
   USHORT Reserve;       //备用(**新版无用字段)
   UINT FlagID;            //ETC门架编号(3字节**新增)
   UINT FlagPassTime;      //通行门架时间(Unix时间**新增)
   char RoadNetNo_h[5];     //收费路网编号[**新增]
   BYTE   ShiftID;       //入出口班次[**新增]
   BYTE   Version;       //新旧版本号[**新增,0.旧版本 0x4F表示新版本]
   char   Remark[3];     //收费公路ETC预留[**新增]
   BYTE   AxisCount;
   UINT   TollWeight;
   BYTE   VehicleStatus;
   UINT   TollFee;       //卡累计金额(2020.03)
} TCCTollData;

/*************弃用作参考********************
typedef struct tag_0019File
{
   BYTE   CappType;    //复合应用类型，标准取值0xAA
   BYTE   RecLen;      //记录长度，标准取值0x29
   BYTE   LockFlag;    //应用锁定标记 0x00
   USHORT RoadNetNo;   //收费路网号
   USHORT Station;     //收费站号
   BYTE   LaneID;      //车道号
   UINT   EnExTime;    //入出口时间
   BYTE   VehClass;    //车型
   BYTE   EnExFlag;    //入出口状态
   char   Reserve[9];  //预留,各省自定义
   UINT   Operator;    //收费员工号(3B)
   BYTE   ShiftID;     //班次
   char   VehPlate[12];//车牌
   char   Remark[4];  //收费公路ETC预留
}T0019File;
********************************************/

//复合消费交易数据(前三项为输入参数)
typedef struct tag_CappData{
   UINT ConsumeMoney;     //交易金额
   char ConsumeTime[15];  //交易时间(yyyymmddhhnnss)
   char hexTermNo[13];    //终端机号(6B)
   UINT RemainMoney;      //交易前余额
   USHORT TradeNo;        //脱机交易序号
   UINT Overdraw;         //透支限额(3B)
   BYTE KeyVer;           //密钥版本
   BYTE  AlgoFlag;        //算法标识
   UINT RandNum;          //随机数
   UINT UserTradeNo;      //用户脱机交易序号(与TradeNo的关系??)
   UINT MAC1;             //MAC1
   UINT TAC;              //TAC码
   UINT MAC2;             //MAC2
} TCappData;

//最近复合消费记录(仅支持旧版闽通卡,外省卡不支持)
typedef struct tag_0018File{
   USHORT  CardTradeNo;   //卡交易序号
   UINT    Overdraw;      //透支限额(3B)
   UINT    ConsumeMoney;  //交易金额
   BYTE    TradeType;     //交易类型标识
   char    hexTermNo[13];     //终端机编号(hex,12B)
   char    ConsumeTime[15];   //交易时间(14B)
} T0018File;

//圈存数据打*为圈存输入参数,其余为各阶段返回
typedef struct tag_CardLoadData{
   char    hexCardNo[17];  //*卡内部编号(hex,8B)
   char    hexTermNo[13];  //*终端机编号(hex,6B)
   UINT    LoadMoney;      //*圈存金额
   char    LoadTime[15];  //*圈存时间(hex,7B)
   UINT    RestMoney;      //余额[初始化返回]
   USHORT  CardTradeNo;    //卡交易序号[初始化返回]
   UINT    RandNum;        //随机数[初始化返回]
   UINT    MAC1;           //MAC1值[初始化返回]
   UINT    MAC2;           //MAC2值[服务端返回]
   UINT    TAC;            //TAC码[圈存返回]
} TCardLoadData;

//----CPC卡格式数据(2019)----------------------------------------------------------
//系统信息文件EF01
typedef struct tag_CPCSysInfo
{
   char Issue[17];  //发行方标识8B
   char CPCID[17];  //HEX格式8B
   BYTE Version;
   char StartTime[9]; //HEX格式合同签著日期4B
   char StopTime[9];   //Hex格式合同过期日期4B
   char Reserve[5];    //自定义5B
}TCPCSysInfo;

//CPC卡基本信息(64B)
typedef struct tag_CPCBaseInfo
{
   BYTE Elec;  //电量信息最高位0正常 1低电 其它7bit为百分比
   BYTE WorkStatus; //5.8G工作状态
   char Reserve[62];  //厂商自定义62B
}TCPCBaseInfo;

//CPC卡出入口信息(DF01-EF01,总共128字节,利用前面40B,出口时覆盖入口)
typedef struct tag_CPCEnExInfo
{
   BYTE   VehClass;        //车型1B(入口自动发卡时上工位填3,下工位填1)
   char   VehPlate[16];    //车牌12B
   char   PlateColor;      //车牌色
   char  RoadNetNo_h[5];       //路网号
   char  Station_h[5];         //入出口站
   BYTE   LaneID;          //入出口车道1B
   UINT   EnExTime;        //入出口时间
   BYTE   WorkStatus;      //5.8G模块工作状态控制字节01打开02关闭(填2,只读)
   BYTE   EnExFlag;        //进出站标志(只读,偏移24出口从此处开始写)
   BYTE   VehStatus;       //车种(入口不明时填0,
   int    Operator;        //入口收费员工号(3B!!)
   BYTE   ShiftID;         //入口班次
   //short  AxisType;        //货车轴型(入口不明时填0,出口时被覆盖)
   BYTE   TruckAxises;     //货车轴数
   int    TotalWeight;     //货车总轴重(入口不明时填0,出口时被覆盖)
   int    LimitWeight;     //货车轴限(3B!!,入口不明时填0,出口时被覆盖)
   BYTE   SpecialTruck;    //特殊货车信息(入口不明时填0,出口时被覆盖)
   char   Reserve[89];     //备用字段
} TCPCEnExInfo;

//CPC卡门架编码
typedef struct stFlagCode
{
   BYTE   ProvinceID;  //省份代码(福建为13)
   BYTE   FlagType;    //门架类型:0路段ETC门架 1.省界ETC门架
   BYTE   FlagWay;     //门架方向:0.上行 1.下行
   USHORT FlagSeq;     //门架序号
}TFlagCode;

//CPC卡过站信息(DF01/EF02)
typedef struct tag_PassCode
{
   char   HexFlag[7]; //门架编号
   UINT   FlagTime;   //门架通行时间
   UINT   FlagFee;    //门架计费金额(3B)
   UINT   MileAge;    //门架计费里程(3B)
}TPassCode;

#define MAX_PASS_FLAG 24
typedef struct tag_CPCPassInfo  //总共72字节
{
   BYTE   ProvinceNum;    //通行省份个数
   BYTE   LocalFlags;     //本省通行门架数
   UINT   LocalFeeSum;    //本省累计计费金额(3B)
   UINT   LocalMileAge;   //本省累计计费里程(3B)
   char   EnHexFlag[7];   //本省第一个入口门架编号(3B),入口站不承载门架时清0
   UINT   EnFlagTime;     //本省第一个入口门架通行时间(unix时间)
   char   LastHexFlag[7]; //最新门架编号
   UINT   LastFlagTime;   //最新门架通行时间
   UINT   LastFlagFee;    //最新门架计费金额(3B)
   UINT   LastMileAge;    //最新门架计费里程(3B)
   char   LocalHexFlags[MAX_PASS_FLAG][7];   //最多24个门架
}TCPCPassInfo;

//计费信息(DF01-EF04)
typedef struct tag_CPCFeeCode    //23字节
{
   BYTE Province;   //1 字节省份行政区域编码
   BYTE PassFlags;  //1 字节本省ETC 门架通行个数
   UINT FeeSum;     //3 字节本省累计计费金额
   UINT MileAge;    //3 字节本省累计计费里程（米）
   char EnHexFlag[7]; //3 字节本省入口门架信息
   UINT EnFlagTime;   //4 字节本省入口门架通行时间UNIX 时间）
   char LastHexFlag[7];  //3 字节最新ETC 门架编码
   UINT LastFlagTime;    //4 字节最新通行时间
   BYTE FittingStatus;   //1 字节最新ETC 门架拟合成功/失败标识（00 成功 01 失败）
}TCPCFeeCode;

#define MAX_FEE_INFO 22
typedef struct tag_CPCFeeInfo
{
   BYTE Records;    //已写入的计费信息数(不会超过22)
   TCPCFeeCode FeeCode[MAX_FEE_INFO];  //计费信息
}TCPCFeeInfo;

//-----------卡控制操作虚拟类------------------------------------------------------
class ICardCtrl32
{
public: //卡控制通用操作
    virtual int  OpenReader(char* ComPort,int SlotNo)=0;   //打开读卡器(参数为com口和PSAM卡座号[从1开始])
    virtual void CloseReader()=0;                        //关读卡器
    virtual int  OpenCard()=0;                           //打开IC卡(0.成功, -1.无卡 -2.坏卡 其它非0.其它错误)
    virtual int  OpenCardEx()=0;                         //打开IC卡(0或1.M1卡,2.ETC卡 4.CPC卡 -1.无卡 -2.坏卡 其它非0.其它错误)
    virtual void CloseCard()=0;                          //关闭IC卡
    virtual int  GetCardType()=0;                        //获取卡的类型(0.无效卡或无卡,10.通行卡 15.CPC卡 22.储值卡 23.记帐卡 20.身份卡)
    virtual void Beep()=0;                               //发出声音
    virtual int  GetCurDosTime()=0;                      //获取当前DOS时间
    virtual void  GetComPort(char* comPort)=0;           //获取串口号
    virtual void SetLogFile(char* logFile)=0;            //设置日志文件
public: //IC卡专有操作接口
    virtual int  ReadBaseInfoIC(TBaseInfo &info)=0;      //读IC卡基本信息(身份卡与通行卡均可使用)
    virtual int  Verify(int nOperator,char *encrypt_passwd)=0;  //身份卡认证(认证成功返回0)
    virtual void CopyVerify(ICardCtrl32 *reader)=0;      //复制身份认证
    virtual void SetCardKey(char *sKey)=0;               //设置Key
    virtual int  ReadEnInfo(TEnInfo &info,TM1ServiceInfo &srvInfo)=0;  //读入口信息(含服务区信息)
    virtual int  WriteEnInfo(const TEnInfo info,const TM1ServiceInfo srvInfo)=0; //写入口信息(含服务区信息)
    virtual int  ReadExInfo(TExInfo &data)=0;            //读出口信息
    virtual int  WriteExInfo(const TExInfo data)=0;      //写出口信息
    virtual int  ReadBitImage(unsigned char *bitBuf)=0;  //读入口图片
    virtual int  WriteBitImage(char *bitBuf)=0;          //写入口二值化图片
    virtual int  WritePrepareCard(char nStation_h[5],int nOperator,BYTE nShift)=0; //写预制卡数据
    virtual int  ResetFlagIC(BYTE nVehClass, BYTE nFlag)=0;  //重置标志(nFlag:0.已入站 1.已出站)
    virtual int  ResetBadCard(char nStation_h[5])=0;             //坏卡重置(一般在校验出错，无法读出时处理）
    virtual int  GetBit2Bmp(const unsigned char* bitBuf,unsigned char* bmpBuf)=0;  //将二值化图转为bmp图,保存在缓存中
    virtual UINT  GetEnDecNum(UINT CardSerialNum,UINT CardID,char EnStation_h[5],UINT WorkNum,UINT EnTime)=0; //获取入口加密值
public: //ETC卡专有操作
    virtual int  FetchTermNo(char *hexTermNo)=0;
    virtual int  FetchPsamNo(char *bcdPsamNo)=0;
    virtual int  FetchTermNoEx(char *hexTermNo,BYTE nSlot)=0;
    virtual int  FetchPsamNoEx(char *bcdPsamNo,BYTE nSlot)=0;
    virtual int  GetPsamRand(const BYTE nWhich,TPsamRand &data)=0;
    virtual int  PsamExternalVerify(const BYTE nWhich,char *hexCosCommand,char *hexReply)=0;
    virtual int  CheckPsamNeedAuth(const BYTE nWhichSlot)=0;   //检查PSAM卡是否需要授权:0.不需要 1.需要 其它值为错误
    virtual int  ReadBaseInfoCC(TCCBaseData &data)=0;       //读基本信息
    virtual int  ReadTollData(TCCTollData &data)=0;         //读过车数据
    virtual int  GetRemainMoney()=0;                        //获取卡片余额
    virtual int  CappConsume(TCCTollData data19,TCappData &capp)=0; //复合消费(入口消费金额为0,capp前两个为输入参数!!)
    virtual int  CappConsumeEx(TCCBaseData &data15,TCCTollData &data19,TCappData &capp)=0;
    virtual int  ResetFlagCC(BYTE nCardStatus)=0;           //卡复位
    virtual int  GetTransTac(char *hexTermNo,USHORT &nCardTradeNo,UINT &nTac)=0; //取回交易的TAC码(如果交易成功)
    virtual int  GetLastTrade(T0018File &data)=0;
    virtual int  InitCardLoad(TCCBaseData &info,TCardLoadData &data)=0;     //圈存初始化
    virtual int  CardLoad(TCardLoadData &data)=0;        //圈存
    virtual UINT  CipherData(char *hexKey16,char *hexData8,char *hexDec)=0;  //临时密钥16字节；加密原文8字节
    virtual int  CPUVerify(char *encrypt_passwd,bool bDeal)=0;
    virtual int  CosCommand(const BYTE nWhichSlot,char *hexCosCommand,bool isSam,char *hexReply)=0;
    //virtual int  WriteCheckPoint(const USHORT nPoint)=0;  //写标识站信息(2019新增)
    //virtual int  ReadRouteInfo(TCPCRouteInfo &info)=0;    //读路径信息(2019新增)
    //virtual int  EarseRouteInfo(bool bQuick)=0;           //清除路径信息(2019新增)
public:  //CPC卡专有操作(2019新增)
   virtual UINT MakeFlagNum(const TFlagCode stCode)=0;  //门架编号转换1
   virtual TFlagCode ParseFlagNum(UINT nFlag)=0;        //门架编号转换2
   virtual int ReadCPCSysInfo(TCPCSysInfo &info)=0;     //读CPC卡系统信息
   virtual int ReadCPCBaseInfo(TCPCBaseInfo &info)=0;   //读CPC卡基础信息
   virtual int WriteCPCEnExInfo(const TCPCEnExInfo info,bool bEntrance)=0; //写进出口信息
   virtual int WriteCPCEnExInfoWithEarse(TCPCSysInfo sysInfo,const TCPCEnExInfo info,bool bEntrance,TCPCPassInfo *passInfo,TCPCFeeCode *feeCode)=0;  //写进出口信息同时清过站和计费信息
   virtual int ReadCPCEnExInfo(TCPCEnExInfo &info)=0;                      //读进出口信息
   virtual int ResetCPCEnExFlag()=0; //CPC卡进出站标志复位(设为已出站卡)

   virtual int WriteCPCPassInfo(const TCPCPassInfo passInfo)=0;  //写过站信息
   virtual int ReadCPCPassInfo(TCPCPassInfo &info)=0;         //读过站信息
   virtual int EarseCPCPassInfo(bool bQuick)=0;           //清除路径信息
   virtual int WriteCPCFeeInfo(const TCPCFeeCode stCode,BYTE provinces)=0; //写计费信息,provinces省份总数
   virtual int ReadCPCFeeInfo(TCPCFeeCode &stCode,BYTE nWhich)=0;       //读某块计费信息(nWhich从1开始计数),每一块为23字节
   virtual int ReadCPCAllFeeInfo(TCPCFeeInfo &info,BYTE provinces)=0;         //读所有计费信息,provinces省份总数
   virtual int EarseCPCFeeInfo(BYTE provinces)=0;          //清除计费信息,当provinces为0时清除所有
   virtual UINT GetCPCSN()=0;        //获取CPC卡ESAM芯片编号
public: //新增车道控制器服务调用
   virtual int ReadCPCInfo(char* CPCSysInfo,char* CPCBaseInfo,char* CPCEnExInfo,char* CPCPassInfo,USHORT &CPCPassLen,char* CPCFeeInfo,USHORT &CPCFeeLen, bool bEntrance)=0;
   virtual int ReadCPUInfo(char* File0015,char* File0019,char* File000C,UINT &restMoney)=0;
   virtual int WriteCPCInfo(char* CPCSysInfo,char* CPCEnExInfo,int passInfoLen,char* CPCPassInfo,int feeInfoLen,char* CPCFeeInfo,bool bEntrance)=0;
   virtual int WriteCPUInfo(char* File0015,char* File0019,TCappData& cappData)=0;
};

/*卡控制对象操作方式(不适用C#)*/
extern "C" ICardCtrl32*  GetCardCtrl32();
extern "C" ICardCtrl32*  GetCardCtrl32Ex(char* dllFile,int apiType);

#endif
 
