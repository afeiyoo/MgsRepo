#include "CCardTransform.h"

CCardTransform::CCardTransform() {}

QString CCardTransform::GetPlateColorETC(BYTE nColor)
{
    QString sColor[] = {"蓝", "黄", "黑", "白", "绿", "拼", "渐", "临"};
    if (nColor < 0 || nColor > 7)
        return "";
    else
        return sColor[nColor];
}

//获取字符串中文个数
int CCardTransform::GetCountFromString(QString sData, int nCheckLen)
{
    if (sData.length() < nCheckLen)
        return 0;
    int nCount = 0;
    int nLen = nCheckLen;
    if (nLen == 0)
        nLen = sData.length();
    for (int i = 1; i <= nLen; i++) {
        if ((sData.toStdString().c_str()[i] & 0x80) >> 7) { //判断最高位是否为1
            nCount++;
            ++i;
        }
    }
    return nCount;
}

int CCardTransform::GetColorFromPlate(QString sVehPlate)
{ // CPC卡保存EF01文件
    if (sVehPlate.length() < 5)
        return 9;
    if (GetCountFromString(sVehPlate, 4) <= 1) {
        if (sVehPlate.mid(0, 2).indexOf("白") >= 0)
            return 3;
        if (sVehPlate.mid(0, 2).indexOf("黑") >= 0)
            return 9;
        //    return 9;
    }
    QString sCheckPlate = sVehPlate.mid(0, 1);
    if (sCheckPlate.indexOf("蓝") >= 0)
        return 0;
    else if (sCheckPlate.indexOf("黄") >= 0)
        return 1;
    else if (sCheckPlate.indexOf("黑") >= 0)
        return 2;
    else if (sCheckPlate.indexOf("白") >= 0)
        return 3;
    else if (sCheckPlate.indexOf("绿") >= 0)
        return 4;
    else if (sCheckPlate.indexOf("拼") >= 0)
        return 5;
    else if (sCheckPlate.indexOf("渐") >= 0)
        return 6;
    else if (sCheckPlate.indexOf("临") >= 0)
        return 7;
    else
        return 9;
}

QString CCardTransform::GetVehNoColor(QString sVehPlate)
{
    int nPos = -1;
    QString sCheckPlate = sVehPlate.mid(0, 1);
    if (GetCountFromString(sVehPlate, 4) <= 1) {
        if ((nPos = sCheckPlate.indexOf("白")) >= 0)
            return sVehPlate.mid(0, nPos - 1) + sVehPlate.mid(nPos + 1, sVehPlate.length() - nPos - 1);
        if ((nPos = sCheckPlate.indexOf("黑")) >= 0)
            return sVehPlate;
    }
    if ((nPos = sCheckPlate.indexOf("蓝")) >= 0)
        ;
    else if ((nPos = sCheckPlate.indexOf("黄")) >= 0)
        ;
    else if ((nPos = sCheckPlate.indexOf("白")) >= 0)
        ;
    else if ((nPos = sCheckPlate.indexOf("绿")) >= 0)
        ;
    else if ((nPos = sCheckPlate.indexOf("拼")) >= 0)
        ;
    else if ((nPos = sCheckPlate.indexOf("渐")) >= 0)
        ;
    else if ((nPos = sCheckPlate.indexOf("黑")) >= 0)
        ;
    else if ((nPos = sCheckPlate.indexOf("临")) >= 0)
        ;
    if (nPos >= 0)
        return sVehPlate.mid(0, nPos - 1) + sVehPlate.mid(nPos + 1, sVehPlate.length() - nPos - 1);
    else
        return sVehPlate;
}

void CCardTransform::ParseCardInfoResult(TReadCardInfoResult readCardInfoResult, TCPCSysInfo &cpcSysInfo, TCPCBaseInfo &cpcBaseInfo,
                                         TCPCEnExInfo &cpcEnExInfo, TCPCPassInfo &cpcPassInfo, TCPCFeeInfo &cpcFeeInfo, TCCBaseData &cpuBaseData,
                                         TCCTollData &cpuTollData, BYTE &Flag, BYTE &Seats)
{
    if (readCardInfoResult.CardType == 15) {
        memset(&cpcSysInfo, 0, sizeof(cpcSysInfo));
        memset(&cpcBaseInfo, 0, sizeof(cpcBaseInfo));
        memset(&cpcEnExInfo, 0, sizeof(cpcEnExInfo));
        memset(&cpcPassInfo, 0, sizeof(cpcPassInfo));
        memset(&cpcFeeInfo, 0, sizeof(cpcFeeInfo));
        //cpcSysInfo 30
        m_hbt.buf_to_struct((BYTE *) readCardInfoResult.CPCSysInfo, "X8X8bX4X45", cpcSysInfo.Issue, cpcSysInfo.CPCID, &cpcSysInfo.Version,
                            &cpcSysInfo.StartTime, &cpcSysInfo.StopTime, cpcSysInfo.Reserve);
        //cpcBaseInfo 64 = 2 +62
        m_hbt.buf_to_struct((BYTE *) readCardInfoResult.CPCBaseInfo, "bb", &cpcBaseInfo.Elec, &cpcBaseInfo.WorkStatus);
        //cpcEnExInfo 128 = 39 + 89
        m_hbt.buf_to_struct((BYTE *) readCardInfoResult.CPCEnExInfo, "bCbX2X2bubbbwbbuwb", &cpcEnExInfo.VehClass, cpcEnExInfo.VehPlate,
                            &cpcEnExInfo.PlateColor, cpcEnExInfo.RoadNetNo_h, cpcEnExInfo.Station_h, &cpcEnExInfo.LaneID, &cpcEnExInfo.EnExTime,
                            &cpcEnExInfo.WorkStatus, &cpcEnExInfo.EnExFlag, &cpcEnExInfo.VehStatus, &cpcEnExInfo.Operator, &cpcEnExInfo.ShiftID,
                            &cpcEnExInfo.TruckAxises, &cpcEnExInfo.TotalWeight, &cpcEnExInfo.LimitWeight, &cpcEnExInfo.SpecialTruck);
        QString sVehPlate = m_tool.gbk_to_utf(cpcEnExInfo.VehPlate);
        //memcpy(cpcEnExInfo.VehPlate,m_tool.sVehPlate.toStdString().c_str(),12);
        memcpy(cpcEnExInfo.VehPlate, sVehPlate.toStdString().c_str(), (sVehPlate.toStdString().length() > 15 ? 15 : sVehPlate.toStdString().length()));

        //cpcPassInfo
        int nLen = 0;
        if (readCardInfoResult.CPCPassLen > 0) {
            m_hbt.buf_to_struct((BYTE *) readCardInfoResult.CPCPassInfo, "bbwwX3uX3uww", &cpcPassInfo.ProvinceNum, &cpcPassInfo.LocalFlags,
                                &cpcPassInfo.LocalFeeSum, &cpcPassInfo.LocalMileAge, cpcPassInfo.EnHexFlag, &cpcPassInfo.EnFlagTime,
                                cpcPassInfo.LastHexFlag, &cpcPassInfo.LastFlagTime, &cpcPassInfo.LastFlagFee, &cpcPassInfo.LastMileAge);
            if (cpcPassInfo.LocalFlags > MAX_PASS_FLAG)
                cpcPassInfo.LocalFlags = MAX_PASS_FLAG;
            for (int i = 0; i < cpcPassInfo.LocalFlags; i++)
                m_hbt.Buf2Hex((BYTE *) readCardInfoResult.CPCPassInfo + 29 + i * 3, cpcPassInfo.LocalHexFlags[i], 3, false);
            //            for(int i=0;i<cpcPassInfo.LocalFlags;i++) nLen += m_hbt.struct_to_buf((BYTE*)readCardInfoResult.CPCPassInfo+nLen,"7",cpcPassInfo.LocalHexFlags[i]);
        }
        //cpcFeeInfo
        nLen = 0;
        cpcFeeInfo.Records = 0;
        if (cpcPassInfo.ProvinceNum > 0) {
            if (cpcPassInfo.ProvinceNum >= MAX_FEE_INFO)
                cpcPassInfo.ProvinceNum = MAX_FEE_INFO;
            for (int n = 0; n < cpcPassInfo.ProvinceNum; n++) {
                nLen += m_hbt.buf_to_struct((BYTE *) readCardInfoResult.CPCFeeInfo + nLen, "bbwwX3uX3ub", &cpcFeeInfo.FeeCode[n].Province,
                                            &cpcFeeInfo.FeeCode[n].PassFlags, &cpcFeeInfo.FeeCode[n].FeeSum, &cpcFeeInfo.FeeCode[n].MileAge,
                                            cpcFeeInfo.FeeCode[n].EnHexFlag, &cpcFeeInfo.FeeCode[n].EnFlagTime, cpcFeeInfo.FeeCode[n].LastHexFlag,
                                            &cpcFeeInfo.FeeCode[n].LastFlagTime, &cpcFeeInfo.FeeCode[n].FittingStatus);
                cpcFeeInfo.Records++;
            }
        }

    } else if (readCardInfoResult.CardType == 22 || readCardInfoResult.CardType == 23) {
        memset(&cpuBaseData, 0, sizeof(cpuBaseData));
        memset(&cpuTollData, 0, sizeof(cpuTollData));
        //0015文件
        BYTE nPlateColor = 0;
        if (readCardInfoResult.CPUBaseData[9] == 16) {
            m_hbt.buf_to_struct((BYTE *) readCardInfoResult.CPUBaseData, "X8bbX2X8X4X4Dbbb", cpuBaseData.hexPubOrg, &cpuBaseData.CardType,
                                &cpuBaseData.CardVer, cpuBaseData.hexCardNet, cpuBaseData.hexCardNo, cpuBaseData.hexStartDate,
                                cpuBaseData.hexStopDate, cpuBaseData.VehPlate, &cpuBaseData.UserType, &nPlateColor, &cpuBaseData.VehClass);
        } else {
            m_hbt.buf_to_struct((BYTE *) readCardInfoResult.CPUBaseData, "X8bbX2X8X4X4Cbbb", cpuBaseData.hexPubOrg, &cpuBaseData.CardType,
                                &cpuBaseData.CardVer, cpuBaseData.hexCardNet, cpuBaseData.hexCardNo, cpuBaseData.hexStartDate,
                                cpuBaseData.hexStopDate, cpuBaseData.VehPlate, &cpuBaseData.UserType, &nPlateColor, &cpuBaseData.VehClass);
        }
        NormalizeCP((unsigned char *) cpuBaseData.VehPlate, 17);
        QString sVehPlate = GetPlateColorETC(nPlateColor) + m_tool.gbk_to_utf(cpuBaseData.VehPlate, 12).trimmed();
        memset(cpuBaseData.VehPlate, 0, sizeof(cpuBaseData.VehPlate));
        memcpy(cpuBaseData.VehPlate, sVehPlate.toStdString().c_str(), (sVehPlate.toStdString().length() > 17 ? 17 : sVehPlate.toStdString().length()));
        cpuBaseData.RemainMoney = readCardInfoResult.RestMoney;
        //cpuBaseData.IsBinding
        //cpuBaseData.Seats;

        //0019文件
        m_hbt.buf_to_struct((BYTE *) readCardInfoResult.CPUTollData, "bbbX2X2bubbwuCbbwbu", &cpuTollData.CappType, &cpuTollData.RecLen,
                            &cpuTollData.LockFlag, cpuTollData.RoadNetNo_h, cpuTollData.EnStation_h, &cpuTollData.EnLane, &cpuTollData.EnTime,
                            &cpuTollData.VehClass, &cpuTollData.CardStatus, &cpuTollData.FlagID, &cpuTollData.FlagPassTime, cpuTollData.VehPlate,
                            &cpuTollData.PlateColor, &cpuTollData.AxisCount, &cpuTollData.TollWeight, &cpuTollData.VehicleStatus,
                            &cpuTollData.TollFee);
        QString sUtf8CehPlate = m_tool.gbk_to_utf(cpuTollData.VehPlate);
        memset(cpuTollData.VehPlate, 0, sizeof(cpuTollData.VehPlate));
        memcpy(cpuTollData.VehPlate, sUtf8CehPlate.toStdString().c_str(),
               ((sUtf8CehPlate.toStdString().length() > 15) ? 15 : sUtf8CehPlate.toStdString().length()));
        cpuTollData.Version = VERSION_0019_5F;
        if (cpuTollData.CardStatus != 1 && cpuTollData.CardStatus != 3 && cpuTollData.CardStatus != 5 && cpuTollData.CardStatus != 7
            && cpuTollData.CardStatus != 8) {
            strcpy(cpuTollData.ExStation_h, cpuTollData.EnStation_h);
            strcpy(cpuTollData.EnStation_h, "0000");
            cpuTollData.ExTime = cpuTollData.EnTime;
            cpuTollData.EnTime = 0;
        }

        //000C
        //m_hbt.buf_to_struct((BYTE*)readCardInfoResult.CPURemark,"bbu",&Flag,&Seats);
        m_hbt.buf_to_struct((BYTE *) readCardInfoResult.CPURemark, "bb", &Flag, &Seats);
        cpuBaseData.Seats = Seats;
        if ((Flag & 1) == 1)
            cpuBaseData.IsBinding = true;
        else
            cpuBaseData.IsBinding = false;
        if ((Flag & 2) == 2)
            cpuBaseData.IsFree = true;
        else
            cpuBaseData.IsFree = false;
        if ((Flag & 4) == 4)
            cpuBaseData.IsTruck = true;
        else
            cpuBaseData.IsTruck = false;

    } else if (readCardInfoResult.CardType == 20) {
        memset(&cpuBaseData, 0, sizeof(cpuBaseData));
        memset(&cpuTollData, 0, sizeof(cpuTollData));
        //0015文件
        BYTE nPlateColor = 0;

        m_hbt.buf_to_struct((BYTE *) readCardInfoResult.CPUBaseData, "X8bbX2X8X4X4Dbbb", cpuBaseData.hexPubOrg, &cpuBaseData.CardType,
                            &cpuBaseData.CardVer, cpuBaseData.hexCardNet, cpuBaseData.hexCardNo, cpuBaseData.hexStartDate, cpuBaseData.hexStopDate,
                            cpuBaseData.VehPlate, &cpuBaseData.UserType, &nPlateColor, &cpuBaseData.VehClass);
    }
}

void CCardTransform::ChangeToCPCWriteInfo(TCPCSysInfo cpcSysInfo, TCPCEnExInfo cpcEnExInfo, int passInfoLen, TCPCPassInfo cpcPassInfo, int feeInfoLen,
                                          TCPCFeeInfo cpcFeeInfo, TWriteCardInfo &writeCardInfo)
{
    memset(writeCardInfo.CPCSysInfo, 0, sizeof(writeCardInfo.CPCSysInfo));
    memset(writeCardInfo.CPCEnExInfo, 0, sizeof(writeCardInfo.CPCEnExInfo));
    memset(writeCardInfo.CPCPassInfo, 0, sizeof(writeCardInfo.CPCPassInfo));
    memset(writeCardInfo.CPCFeeInfo, 0, sizeof(writeCardInfo.CPCFeeInfo));

    //cpcSysInfo 30
    m_hbt.struct_to_buf((BYTE *) writeCardInfo.CPCSysInfo, "X8X8bX4X45", cpcSysInfo.Issue, cpcSysInfo.CPCID, cpcSysInfo.Version, cpcSysInfo.StartTime,
                        cpcSysInfo.StopTime, cpcSysInfo.Reserve);

    //cpcEnExInfo 128 = 39 + 89
    char VehPlate[50] = {0};
    QByteArray sVeh = m_tool.utf_to_gbk(cpcEnExInfo.VehPlate);
    memcpy(VehPlate, sVeh.data(), sVeh.size());
    m_hbt.struct_to_buf((BYTE *) writeCardInfo.CPCEnExInfo, "bCbX2X2bubbbwbbuwb", cpcEnExInfo.VehClass, VehPlate, cpcEnExInfo.PlateColor,
                        cpcEnExInfo.RoadNetNo_h, cpcEnExInfo.Station_h, cpcEnExInfo.LaneID, cpcEnExInfo.EnExTime, cpcEnExInfo.WorkStatus,
                        cpcEnExInfo.EnExFlag, cpcEnExInfo.VehStatus, cpcEnExInfo.Operator, cpcEnExInfo.ShiftID, cpcEnExInfo.TruckAxises,
                        cpcEnExInfo.TotalWeight, cpcEnExInfo.LimitWeight, cpcEnExInfo.SpecialTruck);
    //cpcPassInfo
    int nLen = 0;
    if (passInfoLen > 0) {
        writeCardInfo.CPCPassLen = 72;
        nLen = m_hbt.struct_to_buf((BYTE *) writeCardInfo.CPCPassInfo, "bbwwX3uX3uwwb", cpcPassInfo.ProvinceNum, cpcPassInfo.LocalFlags,
                                   cpcPassInfo.LocalFeeSum, cpcPassInfo.LocalMileAge, cpcPassInfo.EnHexFlag, cpcPassInfo.EnFlagTime,
                                   cpcPassInfo.LastHexFlag, cpcPassInfo.LastFlagTime, cpcPassInfo.LastFlagFee, cpcPassInfo.LastMileAge,
                                   cpcPassInfo.LocalFlags);
        for (int i = 0; i < cpcPassInfo.LocalFlags; i++)
            nLen += m_hbt.struct_to_buf((BYTE *) writeCardInfo.CPCPassInfo + 29, "X3", cpcPassInfo.LocalHexFlags[i]);
    }
    //cpcFeeInfo
    nLen = 0;
    if (feeInfoLen > 0) {
        writeCardInfo.CPCFeeLen = 24;
        for (int n = 0; n < cpcFeeInfo.Records; n++)
            nLen += m_hbt.struct_to_buf((BYTE *) writeCardInfo.CPCFeeInfo + nLen, "bbwwX3uX3ub", cpcFeeInfo.FeeCode[n].Province,
                                        cpcFeeInfo.FeeCode[n].PassFlags, cpcFeeInfo.FeeCode[n].FeeSum, cpcFeeInfo.FeeCode[n].MileAge,
                                        cpcFeeInfo.FeeCode[n].EnHexFlag, cpcFeeInfo.FeeCode[n].EnFlagTime, cpcFeeInfo.FeeCode[n].LastHexFlag,
                                        cpcFeeInfo.FeeCode[n].LastFlagTime, cpcFeeInfo.FeeCode[n].FittingStatus);
    }
}

void CCardTransform::ChangeToCPUWriteInfo(TCCBaseData cpuBaseData, TCCTollData cpuTollData, TWriteCardInfo &writeCardInfo)
{
    memset(writeCardInfo.CPUBaseData, 0, sizeof(writeCardInfo.CPUBaseData));
    memset(writeCardInfo.CPUTollData, 0, sizeof(writeCardInfo.CPUTollData));

    char VehPlate[50] = {0};
    QByteArray sVeh = m_tool.utf_to_gbk(GetVehNoColor(QString::fromUtf8(cpuBaseData.VehPlate)).toStdString().c_str());
    memcpy(VehPlate, sVeh.data(), sVeh.size());

    m_hbt.struct_to_buf((BYTE *) writeCardInfo.CPUBaseData, "X8bbX2X8X4X4Cbbb", cpuBaseData.hexPubOrg, cpuBaseData.CardType, cpuBaseData.CardVer,
                        cpuBaseData.hexCardNet, cpuBaseData.hexCardNo, cpuBaseData.hexStartDate, cpuBaseData.hexStopDate, VehPlate,
                        cpuBaseData.UserType, GetColorFromPlate(QString::fromUtf8(cpuBaseData.VehPlate)), cpuBaseData.VehClass);

    //0019文件
    //     m_hbt.struct_to_buf((BYTE*)writeCardInfo.CPUTollData,"bbbttbubbwuCbbwbu",cpuTollData.CappType,cpuTollData.RecLen,cpuTollData.LockFlag,
    //                               cpuTollData.RoadNetNo_h,cpuTollData.EnStation_h,cpuTollData.EnLane,cpuTollData.EnTime,cpuTollData.VehClass,
    //                               cpuTollData.CardStatus,cpuTollData.FlagID,cpuTollData.FlagPassTime,m_tool.utf_to_gbk(cpuTollData.VehPlate).toStdString().c_str(),cpuTollData.PlateColor,
    //                               cpuTollData.AxisCount,cpuTollData.TollWeight,cpuTollData.VehicleStatus,cpuTollData.TollFee);

    char VehPlate2[50] = {0};
    QByteArray sVeh2 = m_tool.utf_to_gbk(cpuTollData.VehPlate);
    memcpy(VehPlate2, sVeh2.data(), sVeh2.size());

    m_hbt.struct_to_buf((BYTE *) writeCardInfo.CPUTollData, "bbbX2X2bubbwuCbbwbu", cpuTollData.CappType, cpuTollData.RecLen, cpuTollData.LockFlag,
                        cpuTollData.RoadNetNo_h, cpuTollData.ExStation_h, cpuTollData.EnLane, cpuTollData.ExTime, cpuTollData.VehClass,
                        cpuTollData.CardStatus, cpuTollData.FlagID, cpuTollData.FlagPassTime, VehPlate2, cpuTollData.PlateColor,
                        cpuTollData.AxisCount, cpuTollData.TollWeight, cpuTollData.VehicleStatus, cpuTollData.TollFee);
}

void CCardTransform::NormalizeCP(unsigned char *buf, int maxLen)
{
    //#if defined(Q_OS_LINUX)
    try {
        int ilen = strlen((const char *) buf);
        if (ilen > maxLen)
            ilen = maxLen;
        //int ilen = maxLen;
        bool chkGBK = false;
        for (int i = 0; i < ilen; ++i) {
            if (chkGBK) {
                chkGBK = false;
                if (buf[i] >= 0x40 && buf[i] <= 0x7e || buf[i] >= 0x80 && buf[i] <= 0xfe)
                    ;
                else {
                    if (i > 0)
                        buf[i - 1] = ' ';
                }
            } else {
                if (buf[i] >= 0x81 && buf[i] <= 0xfe)
                    chkGBK = true;
            }
            if (buf[i] < 32 || buf[i] == 127 || buf[i] == 255 || buf[i] == '<' || buf[i] == '>' || buf[i] == '\'' || buf[i] == '"' || buf[i] == '&') {
                buf[i] = ' ';
            }
        }
        if (chkGBK) {
            if (ilen) {
                buf[ilen - 1] = ' ';
            }
        }
    } catch (...) {
    }
    //#endif
}
