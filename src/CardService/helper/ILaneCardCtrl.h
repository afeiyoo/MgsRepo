#ifndef ILANECARDCTRL_H
#define ILANECARDCTRL_H

#include <QObject>
#include "CXCardCtrlDef.h"
typedef unsigned char BYTE;
typedef unsigned short USHORT;
typedef unsigned int  UINT;

#define MAX_READER  3 //最大读卡器数量


//读取PSAM卡信息
typedef struct tagReadPsamInfo{
    BYTE WorkNo;                //工位号	0：桌面1：上工位，2：下工位
    char Reserve[8];
}TReadPsamInfo;

//读取PSAM卡结果
typedef struct tagReadPsamInfoResult{
    BYTE ErrorCode;      //0-成功，1-失败
    BYTE WorkNo;                //工位号	0：桌面1：上工位，2：下工位
    char Termno[12+1];
    char Psamno[20+1];
    BYTE IsNeedAuth;  //1:需要授权
    char Random[32+1];
}TReadPsamInfoResult;

//读取卡信息
typedef struct tagReadCardInfo{
    BYTE WorkNo;                //工位号	0：桌面1：上工位，2：下工位
    char Reserve[8];
}TReadCardInfo;

//读取卡信息result
typedef struct tagReadCardInfoResult{
    BYTE ErrorCode;      //0-成功，1-失败 3-打开卡失败 4-读卡器故障
    BYTE WorkNo;
    BYTE CardType;               //卡类型，22：储值卡 23：记账卡 15：CPC卡
    char CPCSysInfo[30];        //	CPC卡(EF01)系统信息文件(可选，CPC卡时必选)
    char CPCBaseInfo[64];       //	CPC卡(EF02)基本信息文件(可选，CPC卡时必选)
    char CPCEnExInfo[128];        //	CPC卡(DF01/EF01)入/出口信息文件(可选，CPC卡时必选)
    USHORT CPCPassLen;             //CPC卡过站信息长度，0:无CPCPassInfo信息
    char CPCPassInfo[256];        //CPC卡(DF01/EF02)过站信息文件(可选，CPC卡时必选)72
    USHORT CPCFeeLen;              //CPC卡计费信息长度，0:无CPCFeeInfo信息
    char CPCFeeInfo[1024];         //CPC卡(DF01/EF04)计费信息文件(可选，CPC卡时必选)24
    char  CPUBaseData[50];      //CPU卡(0015)基本信息
    char CPUTollData[43];        //CPU卡(0019)过车信息
    char CPURemark[2];          //CPU卡福建的000C文件
    UINT RestMoney;             //CPU卡余额
}TReadCardInfoResult;

typedef struct tagReadCardInfoDeal{
    BYTE ErrorCode;      //0-成功，1-失败
    BYTE WorkNo;
    BYTE CardType;
    TCPCSysInfo CPCSysInfo;
    TCPCBaseInfo CPCBaseInfo;
    TCPCEnExInfo CPCEnExInfo;
    TCPCPassInfo CPCPassInfo;
    TCPCFeeInfo CPCFeeInfo;
    TCCBaseData File0015;
    TCCTollData File0019;
    BYTE Flag;
    BYTE Seats;
    UINT RestMoney;             //CPU卡余额
}TReadCardInfoDeal;


//写卡
typedef struct tagWriteCardInfo{
    BYTE CardType;               //卡类型，22：储值卡 23：记账卡 15：CPC卡
    BYTE WorkNo;                //工位号	0：桌面1：上工位，2：下工位
    char CardNum[20];            //卡号
    char CPCSysInfo[30];           //CPC卡系统信息
    char CPCEnExInfo[128];       //CPC卡入出口信息
    USHORT CPCPassLen;              //CPC卡过站信息长度，0:无CPCPassInfo信息
    char CPCPassInfo[256];        //CPC卡过站信息
    USHORT CPCFeeLen;              //CPC卡计费信息长度，0:无CPCFeeInfo信息
    char CPUBaseData[50];           //CPU卡(0015)基本信息
    char CPCFeeInfo[1024];         //CPC卡计费信息
    char CPUTollData[43];        //CPU卡(0019)过车信息
    UINT ConsumeMoney;          //CPU卡消费金额
    char  ConsumeTime[14];       //CPU卡消费时间
    char hexTermNo[13];          //终端机编号
    UINT RemainMoney;           //储值卡当前余额
}TWriteCardInfo;

//写卡Result
typedef struct tagWriteCardResult{
    BYTE ErrorCode;      //0-成功，1:PSAM卡需要授权 2-其他原因导致的失败
    BYTE WorkNo;
    TCappData CappData;
}TWriteCardResult;

//授权
typedef struct tagPsamAuthInfo{
    BYTE WorkNo;                //工位号	0：桌面1：上工位，2：下工位
    char APDUCommand[26];
    char Reserve[8];
}TPsamAuthInfo;

//授权result
typedef struct tagPsamAuthResult{
    BYTE WorkNo;
    BYTE ErrorCode;      //0-成功，1-失败
    char AuthResult[4];         //响应码0x9000
}TPsamAuthResult;

//读卡器配置
typedef struct tagSingleCardReaderCfg{
   char Port[50];
   int SlotNo;
   int ApiType;
   char ApiFileName[100]; //动态库文件名ApiType为0时,默认是API_JKM115;ApiType为1时,默认为ICC_HTXX
}TSingleCardReaderCfg;

typedef struct tagCardReaderCfg{
    TSingleCardReaderCfg  oneCardCfg[MAX_READER];
    int LaneType;
}TCardReaderCfg;


class ILaneCardCtrl:public QObject
{
    Q_OBJECT

public:
    explicit ILaneCardCtrl(){
        qRegisterMetaType<BYTE>("BYTE");
        qRegisterMetaType<TReadCardInfoResult>("TReadCardInfoResult");
        qRegisterMetaType<TWriteCardResult>("TWriteCardResult");
        qRegisterMetaType<TReadPsamInfoResult>("TReadPsamInfoResult");
        qRegisterMetaType<TPsamAuthResult>("TPsamAuthResult");
        qRegisterMetaType<TWriteCardInfo>("TWriteCardInfo");
        qRegisterMetaType<TPsamAuthInfo>("TPsamAuthInfo");
        qRegisterMetaType<TSingleCardReaderCfg>("TSingleCardReaderCfg");
        qRegisterMetaType<TCardReaderCfg>("TCardReaderCfg");
        qRegisterMetaType<TCCBaseData>("TCCBaseData");
        qRegisterMetaType<TCardLoadData>("TCardLoadData");

    }
signals:
    //读卡器指令
    void cardInfo(TReadCardInfoResult readCardInfoResult);
    void writeCardResult(TWriteCardResult writeCardResult);
    void psamInfo(TReadPsamInfoResult readPsamInfoResult);
    void psamAuthResult(TPsamAuthResult PsamAuthResult);
    void InitCardReaderStatus(int nWorkNo,int nStatus);
    void cpuVerifyResult(BYTE nWorkNo,int nVerify); //0:认证成功,其他值认证失败
    void InitCardLoadResult(BYTE nWorkNo,int nStatus,TCardLoadData data);//圈存初始化，获取卡内金额
    void CardLoadResult(BYTE nWorkNo,int nStatus,TCardLoadData data);//圈存结果
    void customResult(BYTE nWorkNo,UINT nOperateID,int ErrCode,QString sData);
    //车道主服务指令
    void readCardInfo(BYTE WorkNo);
    void writeCardInfo(TWriteCardInfo writeCardInfo);
    void getPsamInfo(BYTE WorkNo);
    void psamAuth(TPsamAuthInfo psamAuthInfo);
    void getCardType(int sLot);
    void cpuVerify(BYTE nWorkNo,QString sPasswd,bool bDeal);
    void config(TCardReaderCfg cardCfg);
    void InitCardLoad(BYTE nWorkNo,TCCBaseData info,TCardLoadData data);
    void CardLoad(BYTE nWorkNo,TCCBaseData ccBase,TCardLoadData data);
    void customOperate(BYTE nWorkNo,UINT nOperateID,QString Data);
};



#endif // ILANECARDCTRL_H
