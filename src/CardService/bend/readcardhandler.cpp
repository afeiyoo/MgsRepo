#include "readcardhandler.h"

#include "Logger.h"
#include "core/globalmanager.h"
#include "core/readerctrl.h"
#include "helper/CCardTransform.h"
#include "utils/datadealutils.h"

using namespace Utils;

namespace {
QString bufferToString(const char *buffer, int bufferSize)
{
    return QString::fromUtf8(DataDealUtils::bufferToByteArray(buffer, bufferSize));
}
} // namespace

ReadCardHandler::ReadCardHandler(QObject *parent)
    : HttpRequestHandler{parent}
{}

ReadCardHandler::~ReadCardHandler() {}

void ReadCardHandler::service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response)
{
    const QByteArray reqData = request.getBody();
    LOG_INFO().noquote() << "远程辅助请求读卡:" << reqData;

    TReadCardInfoResult readCardInfoResult;
    bool readOk = GM_INS->m_reader->readCard(readCardInfoResult);

    if (readOk) {
        LOG_INFO().noquote() << "读卡信息成功";
    } else {
        LOG_INFO().noquote() << "读卡信息失败";
    }

    TCPCSysInfo cpcSysInfo = {};
    TCPCBaseInfo cpcBaseInfo = {};
    TCPCEnExInfo cpcEnExInfo = {};
    TCPCPassInfo cpcPassInfo = {};
    TCPCFeeInfo cpcFeeInfo = {};
    TCCBaseData ccBaseData = {};
    TCCTollData ccTollData = {};
    uchar flag = 0;
    uchar seats = 0;

    CCardTransform transformer;
    transformer.ParseCardInfoResult(readCardInfoResult, cpcSysInfo, cpcBaseInfo, cpcEnExInfo, cpcPassInfo, cpcFeeInfo, ccBaseData, ccTollData, flag,
                                    seats);

    QVariantMap resMap;
    resMap["ErrorCode"] = readCardInfoResult.ErrorCode;
    resMap["CardType"] = readCardInfoResult.CardType;
    if (readCardInfoResult.CardType == 15) {
        QVariantMap cpcSysInfoMap;
        cpcSysInfoMap["Issue"] = bufferToString(cpcSysInfo.Issue, sizeof(cpcSysInfo.Issue));
        cpcSysInfoMap["CPCID"] = bufferToString(cpcSysInfo.CPCID, sizeof(cpcSysInfo.CPCID));
        cpcSysInfoMap["Version"] = cpcSysInfo.Version;
        cpcSysInfoMap["StartTime"] = bufferToString(cpcSysInfo.StartTime, sizeof(cpcSysInfo.StartTime));
        cpcSysInfoMap["StopTime"] = bufferToString(cpcSysInfo.StopTime, sizeof(cpcSysInfo.StopTime));
        cpcSysInfoMap["Reserve"] = bufferToString(cpcSysInfo.Reserve, sizeof(cpcSysInfo.Reserve));
        resMap["CPCSysInfo"] = cpcSysInfoMap;

        QVariantMap cpcBaseInfoMap;
        cpcBaseInfoMap["Elec"] = cpcBaseInfo.Elec;
        cpcBaseInfoMap["WorkStatus"] = cpcBaseInfo.WorkStatus;
        cpcBaseInfoMap["Reserve"] = bufferToString(cpcBaseInfo.Reserve, sizeof(cpcBaseInfo.Reserve));
        resMap["CPCBaseInfo"] = cpcBaseInfoMap;

        QVariantMap cpcEnExInfoMap;
        cpcEnExInfoMap["VehClass"] = cpcEnExInfo.VehClass;
        // 保留旧接口的字段名拼写，避免影响现有客户端。
        cpcEnExInfoMap["VehPalte"] = bufferToString(cpcEnExInfo.VehPlate, sizeof(cpcEnExInfo.VehPlate));
        cpcEnExInfoMap["PlateColor"] = static_cast<int>(cpcEnExInfo.PlateColor);
        cpcEnExInfoMap["RoadNetNo_h"] = bufferToString(cpcEnExInfo.RoadNetNo_h, sizeof(cpcEnExInfo.RoadNetNo_h));
        cpcEnExInfoMap["Station_h"] = bufferToString(cpcEnExInfo.Station_h, sizeof(cpcEnExInfo.Station_h));
        cpcEnExInfoMap["LaneID"] = cpcEnExInfo.LaneID;
        cpcEnExInfoMap["EnExTime"] = cpcEnExInfo.EnExTime;
        cpcEnExInfoMap["WorkStatus"] = cpcEnExInfo.WorkStatus;
        cpcEnExInfoMap["EnExFlag"] = cpcEnExInfo.EnExFlag;
        cpcEnExInfoMap["VehStatus"] = cpcEnExInfo.VehStatus;
        cpcEnExInfoMap["Operator"] = cpcEnExInfo.Operator;
        cpcEnExInfoMap["ShiftID"] = cpcEnExInfo.ShiftID;
        cpcEnExInfoMap["TruckAxises"] = cpcEnExInfo.TruckAxises;
        cpcEnExInfoMap["TotalWeight"] = cpcEnExInfo.TotalWeight;
        cpcEnExInfoMap["LimitWeight"] = cpcEnExInfo.LimitWeight;
        cpcEnExInfoMap["SpecialTruck"] = cpcEnExInfo.SpecialTruck;
        cpcEnExInfoMap["Reserve"] = bufferToString(cpcEnExInfo.Reserve, sizeof(cpcEnExInfo.Reserve));
        resMap["CPCEnExInfo"] = cpcEnExInfoMap;

        QVariantMap cpcPassInfoMap;
        cpcPassInfoMap["ProvinceNum"] = cpcPassInfo.ProvinceNum;
        cpcPassInfoMap["LocalFlags"] = cpcPassInfo.LocalFlags;
        cpcPassInfoMap["LocalFeeSum"] = cpcPassInfo.LocalFeeSum;
        cpcPassInfoMap["LocalMileAge"] = cpcPassInfo.LocalMileAge;
        cpcPassInfoMap["EnHexFlag"] = bufferToString(cpcPassInfo.EnHexFlag, sizeof(cpcPassInfo.EnHexFlag));
        cpcPassInfoMap["EnFlagTime"] = cpcPassInfo.EnFlagTime;
        cpcPassInfoMap["LastHexFlag"] = bufferToString(cpcPassInfo.LastHexFlag, sizeof(cpcPassInfo.LastHexFlag));
        cpcPassInfoMap["LastFlagTime"] = cpcPassInfo.LastFlagTime;
        cpcPassInfoMap["LastFlagFee"] = cpcPassInfo.LastFlagFee;
        cpcPassInfoMap["LastMileAge"] = cpcPassInfo.LastMileAge;
        for (int i = 0; i < cpcPassInfo.LocalFlags; ++i) {
            cpcPassInfoMap[QString("LocalHexFlags%1").arg(i + 1)] = bufferToString(cpcPassInfo.LocalHexFlags[i], sizeof(cpcPassInfo.LocalHexFlags[i]));
        }
        resMap["CPCPassInfo"] = cpcPassInfoMap;

        QVariantMap cpcFeeInfoMap;
        cpcFeeInfoMap["Records"] = cpcFeeInfo.Records;
        for (int i = 0; i < cpcFeeInfo.Records; ++i) {
            const TCPCFeeCode &feeCode = cpcFeeInfo.FeeCode[i];
            QVariantMap feeCodeMap;
            feeCodeMap["Province"] = feeCode.Province;
            feeCodeMap["PassFlags"] = feeCode.PassFlags;
            feeCodeMap["FeeSum"] = feeCode.FeeSum;
            feeCodeMap["MileAge"] = feeCode.MileAge;
            feeCodeMap["EnHexFlag"] = bufferToString(feeCode.EnHexFlag, sizeof(feeCode.EnHexFlag));
            feeCodeMap["EnFlagTime"] = feeCode.EnFlagTime;
            feeCodeMap["LastHexFlag"] = bufferToString(feeCode.LastHexFlag, sizeof(feeCode.LastHexFlag));
            feeCodeMap["LastFlagTime"] = feeCode.LastFlagTime;
            feeCodeMap["FittingStatus"] = feeCode.FittingStatus;
            cpcFeeInfoMap[QString("CPCFeeCode%1").arg(i + 1)] = feeCodeMap;
        }
        resMap["CPCFeeInfo"] = cpcFeeInfoMap;
    } else {
        QVariantMap ccBaseDataMap;
        ccBaseDataMap["HexPubOrg"] = bufferToString(ccBaseData.hexPubOrg, sizeof(ccBaseData.hexPubOrg));
        ccBaseDataMap["CardType"] = ccBaseData.CardType;
        ccBaseDataMap["CardVer"] = ccBaseData.CardVer;
        ccBaseDataMap["HexCardNet"] = bufferToString(ccBaseData.hexCardNet, sizeof(ccBaseData.hexCardNet));
        ccBaseDataMap["HexCardNo"] = bufferToString(ccBaseData.hexCardNo, sizeof(ccBaseData.hexCardNo));
        ccBaseDataMap["HexStartDate"] = bufferToString(ccBaseData.hexStartDate, sizeof(ccBaseData.hexStartDate));
        ccBaseDataMap["HexStopDate"] = bufferToString(ccBaseData.hexStopDate, sizeof(ccBaseData.hexStopDate));
        ccBaseDataMap["VehPlate"] = bufferToString(ccBaseData.VehPlate, sizeof(ccBaseData.VehPlate));
        ccBaseDataMap["VehClass"] = ccBaseData.VehClass;
        ccBaseDataMap["UserType"] = ccBaseData.UserType;
        ccBaseDataMap["IsBinding"] = static_cast<int>(ccBaseData.IsBinding);
        ccBaseDataMap["IsFree"] = static_cast<int>(ccBaseData.IsFree);
        ccBaseDataMap["IsTruck"] = static_cast<int>(ccBaseData.IsTruck);
        ccBaseDataMap["Seats"] = ccBaseData.Seats;
        ccBaseDataMap["RemainMoney"] = ccBaseData.RemainMoney;
        resMap["CCBaseData"] = ccBaseDataMap;

        QVariantMap ccTollDataMap;
        ccTollDataMap["CappType"] = ccTollData.CappType;
        ccTollDataMap["RecLen"] = ccTollData.RecLen;
        ccTollDataMap["LockFlag"] = ccTollData.LockFlag;
        ccTollDataMap["EnStation_h"] = bufferToString(ccTollData.EnStation_h, sizeof(ccTollData.EnStation_h));
        ccTollDataMap["EnLane"] = ccTollData.EnLane;
        ccTollDataMap["EnTime"] = ccTollData.EnTime;
        ccTollDataMap["VehClass"] = ccTollData.VehClass;
        ccTollDataMap["CardStatus"] = ccTollData.CardStatus;
        ccTollDataMap["EnOperator"] = ccTollData.EnOperator;
        ccTollDataMap["VehPlate"] = bufferToString(ccTollData.VehPlate, sizeof(ccTollData.VehPlate));
        ccTollDataMap["PlateColor"] = ccTollData.PlateColor;
        ccTollDataMap["ExStation_h"] = bufferToString(ccTollData.ExStation_h, sizeof(ccTollData.ExStation_h));
        ccTollDataMap["ExTime"] = ccTollData.ExTime;
        ccTollDataMap["ChkPoint"] = bufferToString(ccTollData.ChkPoint, sizeof(ccTollData.ChkPoint));
        ccTollDataMap["Reserve"] = ccTollData.Reserve;
        ccTollDataMap["FlagID"] = ccTollData.FlagID;
        ccTollDataMap["FlagPassTime"] = ccTollData.FlagPassTime;
        ccTollDataMap["RoadNetNo_h"] = bufferToString(ccTollData.RoadNetNo_h, sizeof(ccTollData.RoadNetNo_h));
        ccTollDataMap["ShiftID"] = ccTollData.ShiftID;
        ccTollDataMap["Version"] = ccTollData.Version;
        ccTollDataMap["Remark"] = bufferToString(ccTollData.Remark, sizeof(ccTollData.Remark));
        ccTollDataMap["AxisCount"] = ccTollData.AxisCount;
        ccTollDataMap["TollWeight"] = ccTollData.TollWeight;
        ccTollDataMap["VehicleStatus"] = ccTollData.VehicleStatus;
        ccTollDataMap["TollFee"] = ccTollData.TollFee;
        resMap["CCTollData"] = ccTollDataMap;
    }

    const QByteArray resData = DataDealUtils::mapToJson(resMap);
    LOG_INFO().noquote() << "向远程辅助返回读卡结果:" << resData;
    response.write(resData, true);
}
