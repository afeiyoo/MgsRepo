#pragma once

#include "CHexBufTool.h"
#include "CommonTool.h"
#include "ILaneCardCtrl.h"

class CCardTransform
{
private:
    CommonTool m_tool;
    CHexBufTool m_hbt;
    int GetColorFromPlate(QString sVehPlate);
    QString GetVehNoColor(QString sVehPlate);
    int GetCountFromString(QString sData, int nCheckLen);
    QString GetPlateColorETC(BYTE nColor);
    void NormalizeCP(unsigned char *buf, int maxLen);

public:
    CCardTransform();
    void ParseCardInfoResult(TReadCardInfoResult readCardInfoResult, TCPCSysInfo &CPCSysInfo, TCPCBaseInfo &CPCBaseInfo, TCPCEnExInfo &CPCEnExInfo,
                             TCPCPassInfo &CPCPassInfo, TCPCFeeInfo &CPCFeeInfo, TCCBaseData &File0015, TCCTollData &cpuTollData, BYTE &Flag,
                             BYTE &Seats);
    void ChangeToCPCWriteInfo(TCPCSysInfo cpcSysInfo, TCPCEnExInfo CPCEnExInfo, int passInfoLen, TCPCPassInfo cpcPassInfo, int feeInfoLen,
                              TCPCFeeInfo cpcFeeInfo, TWriteCardInfo &writeCardInfo);
    void ChangeToCPUWriteInfo(TCCBaseData cpuBaseData, TCCTollData cpuTollData, TWriteCardInfo &writeCardInfo);
};
