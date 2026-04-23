#include "bizhandler.h"

#include "HttpClient/src/http.h"
#include "Logger.h"
#include "NlohmannJson/nlojson.hpp"
#include "QJson/include/qobjecthelper.h"
#include "bean/t_auditpayback.h"
#include "bean/t_discardticketreview.h"
#include "bean/t_etcout.h"
#include "bean/t_freetempvehicle.h"
#include "bean/t_laneinputshift.h"
#include "bean/t_mtcout.h"
#include "bean/t_mticketuse.h"
#include "bean/t_specialcards.h"
#include "bean/t_splitout.h"
#include "bend/dtpsender.h"
#include "config/baseexception.h"
#include "config/config.h"
#include "global/globalmanager.h"
#include "utils/bizutils.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"

QMap<QString, ST_AuditInfo> BizHandler::m_auditInfos;

BizHandler::BizHandler(QObject *parent)
    : QObject{parent}
{
    // 每次有新请求到来时，清除30天前的图片文件
    QString error;
    Utils::FileUtils::autoDeleteFiles(GM_INSTANCE->m_pictureDir.toString(), ".jpg", 30 * 24, &error);
    if (!error.isEmpty())
        LOG_INFO().noquote() << error;
}

BizHandler::~BizHandler() {}

QString BizHandler::doMainDeal(int cmdType, const QVariantMap &dataMap, const QByteArray &reqBody)
{
    QString dealtData;
    switch (cmdType) {
    case 1:
        dealtData = doDealCmd01(dataMap); // 稽核登录
        break;
    case 16:
        dealtData = doDealCmd16(dataMap); // 稽核补费打票
        break;
    case 18:
        dealtData = doDealCmd18(dataMap); // 路径展示
        break;
    case 19:
        dealtData = doDealCmd19(dataMap); // 绿通查验
        break;
    case 20:
        dealtData = doDealCmd20(dataMap); // 获取图片
        break;
    case 23:
        dealtData = doDealCmd23(dataMap); // 栏杆机操作
        break;
    case 24:
        dealtData = doDealCmd24(reqBody); // 车牌识别
        break;
    case 25:
        dealtData = doDealCmd25(dataMap); // 获取状态名单
        break;
    case 26:
        dealtData = doDealCmd26(dataMap); // 交接班数据
        break;
    case 27:
        dealtData = doDealCmd27(dataMap); // 特情记录
        break;
    case 28:
        dealtData = doDealCmd28(dataMap); // 查询工号
        break;
    case 30:
        dealtData = doDealCmd30(dataMap); // 查询逃费车辆欠费信息
        break;
    case 31:
        dealtData = doDealCmd31(dataMap); // 逃漏费车辆补费上传
        break;
    case 32:
        dealtData = doDealCmd32(dataMap); // 大件运输车预约信息查询
        break;
    case 33:
        dealtData = doDealCmd33(dataMap); // 集装箱车预约信息列表查询
        break;
    case 34:
        dealtData = doDealCmd34(dataMap); // 临时免征车列表查询
        break;
    case 37:
        dealtData = doDealCmd37(dataMap); // 图片上传
        break;
    case 38:
        dealtData = doDealCmd38(dataMap); // 查询出口流水是否可补打票信息
        break;
    case 39:
        dealtData = doDealCmd39(dataMap); // 本站绿通流水查询
        break;
    case 40:
        dealtData = doDealCmd40(dataMap); // 云坐席台账
        break;
    default:
        break;
    }
    return dealtData;
}

QString BizHandler::doDealCmd01(const QVariantMap &aMap)
{
    QString stationID;
    int laneID = 0;
    QString operatorID;

    if (aMap.contains("stationID"))
        stationID = aMap["stationID"].toString();
    if (aMap.contains("laneID"))
        laneID = aMap["laneID"].toInt();
    if (aMap.contains("operatorID"))
        operatorID = aMap["operatorID"].toString();

    if (stationID.isEmpty())
        throw BaseException(1, "响应失败: 站代码为空");

    NloJson nloJson;

    QString stationName = m_ds.getStationName("3501" + stationID);

    QString msg = QString("%1(%2), 车道号: %3, 工号: %4").arg(stationName, stationID).arg(laneID).arg(operatorID);
    LOG_INFO().noquote() << QString("%1 稽核业务登录成功").arg(msg);

    QVariantMap resMap;
    resMap["loginStatus"] = 0;
    resMap["desc"] = msg;
    resMap["loginCode"] = QString("%1%2").arg(stationID, Utils::DataDealUtils::padValue(laneID, 2));

    QString dealtData = nloJson.serialize(resMap);
    return dealtData;
}

QString BizHandler::doDealCmd16(const QVariantMap &aMap)
{
    int tradeType = 0;
    QString tradeId;
    QString ticketNum;
    QString operatorId;
    QString startNum;
    QString stopNum;
    QString shiftDate;
    int shiftNum = 0;
    int laneID = 0;
    QString stationID;

    if (aMap.contains("tradeID"))
        tradeId = aMap["tradeID"].toString();
    if (aMap.contains("tradeType"))
        tradeType = aMap["tradeType"].toInt();
    if (aMap.contains("ticketNum"))
        ticketNum = aMap["ticketNum"].toString();
    if (aMap.contains("operatorID"))
        operatorId = aMap["operatorID"].toString();
    if (aMap.contains("startNum"))
        startNum = aMap["startNum"].toString();
    if (aMap.contains("stopNum"))
        stopNum = aMap["stopNum"].toString();
    if (aMap.contains("shiftDate"))
        shiftDate = aMap["shiftDate"].toString();
    if (aMap.contains("laneID"))
        laneID = aMap["laneID"].toInt();
    if (aMap.contains("stationID"))
        stationID = aMap["stationID"].toString();
    if (aMap.contains("shiftNum"))
        shiftNum = aMap["shiftNum"].toInt();

    if (tradeType == 0)
        throw BaseException(1, "响应失败: 操作类型异常");
    if (laneID == 0)
        throw BaseException(1, "响应失败: 车道号为空");
    if (shiftDate.isEmpty())
        throw BaseException(1, "响应失败: 班次日期为空");
    if (stationID.isEmpty())
        throw BaseException(1, "响应失败: 站代码为空");

    NloJson nloJson;

    QString dealtData;
    if (tradeType == 7) {
        // 补费票段设置
        if (startNum.isEmpty())
            throw BaseException(1, "响应失败: 起始票据号为空");
        if (stopNum.isEmpty())
            throw BaseException(1, "响应失败: 终止票据号为空");

// TODO 测试完成后需要取消注释
#if 0
        QString stationIP = m_ds.getStationIP(stationID);
        SINGLETON_GM->createSqlServerConnection(stationIP);
        // 获取票据信息
        QString id = QString("sqlserver_%1").arg(stationIP);
        QVariantMap resMap = m_ds.getTicketUseInfo(laneID, shiftDate, id);
        if (resMap.isEmpty()) throw BaseException(1, "响应失败: 未查询到相关票据信息");
        // 获取票据段
        int startNumFromDB = resMap["StartNum"].toInt();
        int stopNumFromDB = resMap["StopNum"].toInt();
        if (!(startNum.toInt() >= startNumFromDB && stopNum.toInt() <= stopNumFromDB &&
              startNum.toInt() <= stopNum.toInt()))
            throw BaseException(1, "响应失败: 票据段与下发票据段不符");
#endif

        QVariantMap map;
        map["status"] = 0;
        map["desc"] = "成功设置补费票段";

        dealtData = nloJson.serialize(map);
    } else if (tradeType == 8 || tradeType == 9) {
        // 8 稽核打票（每票必打） 9 车道流水补打票（按需打票）
        if (tradeId.isEmpty())
            throw BaseException(1, "响应失败: 交易号为空");
        if (ticketNum.isEmpty())
            throw BaseException(1, "响应失败: 票据号为空");
        if (operatorId.isEmpty())
            throw BaseException(1, "响应失败: 操作员编号为空");
        if (shiftNum == 0)
            throw BaseException(1, "响应失败: 班次号为空");

        QString stationIP = m_ds.getStationIP(stationID);
        QString stationServiceUrl = QString("http://%1:8082").arg(stationIP);

        // 获取票据信息
        QVariantMap resMap = m_ds.getTicketUseInfo(laneID, ticketNum, QUrl(stationServiceUrl));

        // 获取票据段
        int startNumFromDB = resMap["StartNum"].toInt();
        int stopNumFromDB = resMap["StopNum"].toInt();
        if (ticketNum.toInt() < startNumFromDB || ticketNum.toInt() > stopNumFromDB)
            throw BaseException(1, "响应失败: 票据号不在票据段内");

        // 下发票据信息报文到收费站
        T_MTicketUse ticketUse;

        if (tradeType == 8) {
            ticketUse.TradeNum = tradeId.midRef(6).toInt();
            ticketUse.TradeID = QString::number(ticketUse.TradeNum);
        } else {
            ticketUse.TradeNum = tradeId.rightRef(8).toInt();
            ticketUse.TradeID = tradeId;
        }
        ticketUse.TicketType = resMap["TicketType"].toInt();
        ticketUse.FaceType = resMap["FaceType"].toInt();
        ticketUse.TicketYear = resMap["TicketYear"].toInt();
        ticketUse.StartNum = ticketNum.toInt();
        ticketUse.EndNum = ticketNum.toInt();
        ticketUse.Amount = 1;
        ticketUse.LaneID = laneID;
        ticketUse.ShiftDate = QDateTime::fromString(shiftDate, "yyyy-MM-dd hh:mm:ss");
        ticketUse.ShiftNum = shiftNum;
        ticketUse.ShiftUser = operatorId;
        ticketUse.IsValid = 1;
        ticketUse.OperaterID = operatorId;
        ticketUse.OperateTime = QDateTime::currentDateTime();
        ticketUse.BillCode = resMap["BillCode"].toString();

        QString dtpContent = Utils::BizUtils::makeDtpContentFromStr(QStringList() << Utils::DataDealUtils::getInsertSql(&ticketUse));
        QString dtpXml = Utils::BizUtils::makeDtpXml(dtpContent, "610", "1590", stationID, 1);
        LOG_INFO().noquote() << "票据信息DTP报文: " << dtpXml;

        int res = GM_INSTANCE->m_dtpSender->sendMsgToDtp(stationIP, 13591, "TradeQ", "", dtpXml);

        if (res < 0)
            throw BaseException(1, "响应失败: 站级数据传输异常");

        // 更新LastNum
        int dataId = resMap["DataID"].toInt();
        m_ds.updateTicketUseInfo(dataId, ticketNum.toInt(), QUrl(stationServiceUrl));

        // 更新班次表
        if (m_ds.getOutShiftSettleCount(stationID, shiftDate, shiftNum, QUrl(stationServiceUrl)) <= 0) {
            QString varShiftDate = QDateTime::fromString(shiftDate, "yyyy-MM-dd hh:mm:ss").toString("yyyyMMdd");
            QString varDataId = QString("%1XX%2").arg(varShiftDate).arg(Utils::DataDealUtils::padValue(shiftNum, 2));
            QString varOperatorName = m_ds.getUserName(operatorId, 1);
            QString operatorName = varOperatorName.isEmpty() ? "稽查班" : varOperatorName;
            if (!m_ds.insertOutShiftSettle(varDataId, shiftDate, shiftNum, stationID, operatorId, operatorName, QUrl(stationServiceUrl))) {
                LOG_WARNING().noquote() << "工班信息插入失败";
            }
        }

        QVariantMap map;
        map["status"] = 0;
        map["desc"] = "成功录入票据信息";

        dealtData = nloJson.serialize(map);
    }
    return dealtData;
}

QString BizHandler::doDealCmd18(const QVariantMap &aMap)
{
    QString qrCodeMsg;
    QString plateNumber;

    if (aMap.contains("qrCodeMsg"))
        qrCodeMsg = aMap["qrCodeMsg"].toString();
    if (aMap.contains("plateNumber"))
        plateNumber = aMap["plateNumber"].toString();

    QString dealtData;
    if (!qrCodeMsg.isEmpty()) {
        dealtData = getVehicleWayFromScan(qrCodeMsg);
    } else if (!plateNumber.isEmpty()) {
        dealtData = getVehicleWayFromPlate(plateNumber);
    } else {
        throw BaseException(1, "响应失败: 车牌号或二维码为空");
    }

    return dealtData;
}

QString BizHandler::getVehicleWayFromPlate(const QString &plateNumber)
{
    T_EtcOut objEtc;
    T_MtcOut objMtc;

    QString passID;
    QString tradeID;
    QString enTime;
    QString enNetHex;
    QString enHex;
    QString enName;
    int exitFeeType = 0, mediaType = 3, vehClass = 0, userType = 0, axisCount = 0;
    int shouldPay = 0, discount = 0, factPay = 0;

    NloJson nloJson;

    ST_Node exNode;
    exNode.nodeType = 1;
    if (m_ds.getLatestOutTradeInPlate(plateNumber, &objEtc, 0)) {
        // ETC出口记录
        if (objEtc.ExShiftDate <= QDateTime::fromString("20200101", "yyyyMMdd")) {
            LOG_INFO().noquote() << QString("取消省界站前数据无效 %1").arg(objEtc.ExShiftDate.toString("yyyy-MM-dd"));
            throw BaseException(1, "响应失败: 数据无效(取消省界站前数据)");
        }
        passID = objEtc.PassID;
        tradeID = objEtc.TradeID;
        exitFeeType = objEtc.ExitFeeType;
        vehClass = objEtc.ExVehClass;
        userType = objEtc.UserType;
        axisCount = objEtc.Axises;
        shouldPay = objEtc.ShouldPay * 100;
        factPay = objEtc.FactPay * 100;
        discount = objEtc.Discount * 100;
        enTime = objEtc.EnTime.toString("yyyy-MM-dd HH:mm:ss");
        enHex = objEtc.EnStation;
        enNetHex = QString::number(objEtc.EnNetID);
        enName = objEtc.EnStationName;
        if (passID.left(2) == "01") {
            mediaType = 0;
        } else if (passID.left(2) == "02") {
            mediaType = 1;
        }

        // 出口门架节点信息
        exNode.captureId = objEtc.TradeID;
        exNode.nodeHex = objEtc.ExStation;
        exNode.nodeId = objEtc.CNLaneID.left(14);
        exNode.nodeName = objEtc.ExStationName;
        exNode.passTime = objEtc.ExTime.toString("yyyy-MM-dd hh:mm:ss");
    } else if (m_ds.getLatestOutTradeInPlate(plateNumber, &objMtc, 1)) {
        // MTC出口记录
        if (objMtc.ExShiftDate <= QDateTime::fromString("20200101", "yyyyMMdd")) {
            LOG_INFO().noquote() << QString("取消省界站前数据无效 %1").arg(objMtc.ExShiftDate.toString("yyyy-MM-dd"));
            throw BaseException(1, "响应失败: 数据无效(取消省界站前数据)");
        }
        passID = objMtc.PassID;
        tradeID = objMtc.TradeID;
        exitFeeType = objMtc.ExitFeeType;
        vehClass = objMtc.ExVehClass;
        userType = objMtc.UserType;
        axisCount = objMtc.Axises;
        shouldPay = objMtc.ShouldPay * 100;
        factPay = objMtc.FactPay * 100;
        discount = objMtc.Discount * 100;
        enTime = objMtc.EnTime.toString("yyyy-MM-dd HH:mm:ss");
        enHex = objMtc.EnStation;
        enNetHex = QString::number(objMtc.EnNetID);
        enName = objMtc.EnStationName;
        if (passID.left(2) == "01") {
            mediaType = 0;
        } else if (passID.left(2) == "02") {
            mediaType = 1;
        }

        // 出口门架节点信息
        exNode.captureId = objMtc.TradeID;
        exNode.nodeHex = objMtc.ExStation;
        exNode.nodeId = objMtc.CNLaneID.left(14);
        exNode.nodeName = objMtc.ExStationName;
        exNode.passTime = objMtc.ExTime.toString("yyyy-MM-dd hh:mm:ss");
    } else {
        throw BaseException(1, "响应失败：未查询到相关记录");
    }

    if (passID.isEmpty())
        throw BaseException(1, "响应失败: 获取出口信息失败(passid为空)");

    LOG_INFO().noquote() << QString("查询出口信息结果返回: exitFeeType %1, passID %2, name %3, tradeId %4")
                                .arg(exitFeeType)
                                .arg(passID, exNode.nodeName, tradeID);

    // 查询入口信息
    ST_Node enNode;
    enNode.nodeType = 1;
    QVariantMap enInfo = m_ds.getEnInfo(passID);
    if (!enInfo.isEmpty()) {
        enNode.captureId = enInfo["TRADEID"].toString();
        enNode.passTime = enInfo["ENTIME"].toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        enNode.nodeHex = enInfo["ENSTATION"].toString();
        enNode.nodeName = enInfo["STATIONNAME"].toString();
        enNode.nodeId = enInfo["CNLANEID"].toString().left(14);
    }

    if (enInfo.isEmpty() || enNode.captureId.isEmpty()) {
        LOG_INFO().noquote() << "查询入口信息为空，使用出口记录中的入口信息";
        enNode.captureId = "";
        enNode.passTime = enTime;
        enNode.nodeHex = enNetHex + enHex;
        enNode.nodeId = m_ds.getGantryNodeID(enNode.nodeHex);
        enNode.nodeName = enName;
    }
    LOG_INFO().noquote() << QString("查询入口信息结果返回 passID %1, name %2").arg(passID, enNode.nodeName);

    // 组装发送报文
    ST_PathFitting pathFitting;
    pathFitting.plateNumber = Utils::BizUtils::getPlateNoColor(plateNumber);
    pathFitting.plateColor = Utils::BizUtils::getColorCodeFromPlate(plateNumber);
    pathFitting.mediaType = mediaType;
    pathFitting.vehicleType = vehClass;
    pathFitting.vehicleClass = userType;
    pathFitting.receivableMoney = shouldPay;
    pathFitting.tradingMoney = factPay;
    pathFitting.discountsMoney = discount;
    pathFitting.axesCount = axisCount;
    pathFitting.enNode = packNode(enNode);
    pathFitting.exNode = packNode(exNode);
    if (exitFeeType == 1 || exitFeeType == 2 || exitFeeType == 3 || exitFeeType == 11) {
        getGantryNodes(passID, pathFitting.gantryUnits);
    } else if (exitFeeType == 4 || exitFeeType == 5) {
        getGantryNodeSplitOut(tradeID, pathFitting.tollIntervalIDs, pathFitting.tollIntervalIDsTime);
    }
    QString sendJson = packFeePath(pathFitting, exitFeeType);
    LOG_INFO().noquote() << "发送地图路径请求: " << sendJson;

    QUrl url(GM_INSTANCE->m_config->m_baseConfig.mapUrl);

    QByteArray result;
    bool ok = Http::instance().postSync(result, url, sendJson.toUtf8(), "application/json");
    if (!ok) {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }
    LOG_INFO().noquote() << "接收地图路径完成: " << result.left(1024);

    QVariantMap resMap = nloJson.parse(result).toMap();
    QVariantMap bizContent = resMap.value("bizContent", {}).toMap();
    QByteArray sendData = nloJson.serialize(bizContent);

    return sendData;
}

QString BizHandler::getVehicleWayFromScan(const QString &scan)
{
    NloJson nloJson;

    QVariantMap tempMap;
    tempMap.insert("pathFittingData", scan);
    QVariantMap aMap;
    aMap.insert("method", "getPathFittingByPathId");
    aMap.insert("sign", "");
    aMap.insert("signType", "NONE");
    aMap.insert("version", "1.0");
    aMap.insert("timestamp", Utils::DataDealUtils::curDateTimeStr());
    aMap.insert("encryptType", "NONE");
    aMap.insert("bizContent", nloJson.serialize(tempMap));

    QString sendJson = nloJson.serialize(aMap);
    LOG_INFO().noquote() << "传扫描内容: " << sendJson;

    QUrl url(GM_INSTANCE->m_config->m_baseConfig.mapUrl);
    QByteArray result;
    bool ok = Http::instance().postSync(result, url, sendJson.toUtf8(), "application/json");
    if (!ok) {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    LOG_INFO().noquote() << "返回扫描结果: " << result.left(1024);
    QVariantMap resMap = nloJson.parse(result).toMap();
    QVariant bizContent;
    if (resMap.contains("bizContent"))
        bizContent = resMap["bizContent"];

    QString dealtData = nloJson.serialize(bizContent);
    return dealtData;
}

QString BizHandler::packFeePath(const ST_PathFitting &pathFitting, int exitFeeType)
{
    NloJson nloJson;

    QVariantMap map;

    map["plateNumber"] = pathFitting.plateNumber;
    map["plateColor"] = pathFitting.plateColor;
    map["mediaType"] = pathFitting.mediaType;
    map["vehicleType"] = pathFitting.vehicleType;
    map["vehicleClass"] = pathFitting.vehicleClass;
    map["receivableMoney"] = pathFitting.receivableMoney;
    map["tradingMoney"] = pathFitting.tradingMoney;
    map["discountsMoney"] = pathFitting.discountsMoney;
    map["axesCount"] = pathFitting.axesCount;
    map["enNode"] = nloJson.parse(pathFitting.enNode.toUtf8()).toMap();
    map["exNode"] = nloJson.parse(pathFitting.exNode.toUtf8()).toMap();

    QString gantrys;
    if (exitFeeType == 3 || exitFeeType == 2 || exitFeeType == 1 || exitFeeType == 11) { // 卡内计费
        QVariantList unitsList;
        QVariantMap unitMap;
        for (const auto &unit : pathFitting.gantryUnits) {
            if (gantrys.isEmpty())
                gantrys += ",";
            gantrys += unit;

            unitMap = nloJson.parse(unit.toUtf8()).toMap();
            unitsList.append(unitMap);
        }

        if (!gantrys.isEmpty())
            gantrys = "[" + gantrys + "]";
        map.insert("gantryUnits", unitsList);

    } else if (exitFeeType == 4 || exitFeeType == 5) { // 在线计费
        if (!pathFitting.tollIntervalIDs.isEmpty())
            map["tollIntervalIDs"] = pathFitting.tollIntervalIDs;
        if (!pathFitting.tollIntervalIDsTime.isEmpty())
            map["tollIntervalIDsTime"] = pathFitting.tollIntervalIDsTime;
    }

    QString var = QString("axesCount=%1&discountsMoney=%2&enNode=%3&exNode=%4&gantryUnits=%5&mediaType=%6&"
                          "plateColor=%7&plateNumber=%8&receivableMoney=%9&tradingMoney=%10&vehicleClass=%11"
                          "&vehicleType=%12")
                      .arg(pathFitting.axesCount)
                      .arg(pathFitting.discountsMoney)
                      .arg(pathFitting.enNode)
                      .arg(pathFitting.exNode)
                      .arg(gantrys)
                      .arg(pathFitting.mediaType)
                      .arg(pathFitting.plateColor)
                      .arg(pathFitting.plateNumber)
                      .arg(pathFitting.receivableMoney)
                      .arg(pathFitting.tradingMoney)
                      .arg(pathFitting.vehicleClass)
                      .arg(pathFitting.vehicleType);
    QString signMd5 = Utils::DataDealUtils::cryptoMD5(var).toLower();

    QVariantMap sendMap;
    sendMap["method"] = "pathFitting2";
    sendMap["sign"] = signMd5;
    sendMap["signType"] = "NONE";
    sendMap["version"] = "1.0";
    sendMap["timestamp"] = Utils::DataDealUtils::curDateTimeStr();
    sendMap["encryptType"] = "NONE";
    sendMap["bizContent"] = nloJson.serialize(map);

    QByteArray sendJson = nloJson.serialize(sendMap);
    return sendJson;
}

QString BizHandler::packNode(const ST_Node &node)
{
    NloJson nloJson;

    QVariantMap temp;
    temp["captureId"] = node.captureId;
    temp["nodeHex"] = node.nodeHex;
    temp["nodeId"] = node.nodeId;
    temp["nodeName"] = node.nodeName;
    temp["nodeType"] = node.nodeType;
    temp["passTime"] = node.passTime;
    QString data = nloJson.serialize(temp);

    return data;
}

void BizHandler::getGantryNodes(const QString &passID, QStringList &nodeList)
{
    nodeList.clear();

    auto records = m_ds.getGantryInfos(passID);
    if (records.isEmpty())
        return;

    ST_Node node;
    for (const auto &record : records) {
        if (record["ERRORCODE"].toString() != "")
            continue;
        node.nodeType = 0;
        node.captureId = "";
        node.nodeHex = record["FLAGID"].toString();
        node.nodeId = record["GANTRYID"].toString();
        node.nodeName = record["FLAGNAME"].toString();
        node.passTime = record["TRADETIME"].toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        nodeList.append(packNode(node));
    }
}

void BizHandler::getGantryNodeSplitOut(const QString &sTradeID, QString &tollIntervalsGroup, QString &transTimeGroup)
{
    // 查t_SplitOut
    T_SplitOut splitOut;
    if (!m_ds.getSplitOut(sTradeID, &splitOut)) {
        LOG_INFO().noquote() << "未查询到门架信息";
        return;
    }

    tollIntervalsGroup = splitOut.TollIntervalsGroup;
    transTimeGroup = splitOut.TransTimeGroup;
    LOG_INFO().noquote() << QString("门架信息：%1, %2").arg(tollIntervalsGroup, transTimeGroup);
}

QString BizHandler::doDealCmd19(const QVariantMap &aMap)
{
    QString qrCodeMsg;
    QString plateNumber;
    QString tradeID;

    if (aMap.contains("qrCodeMsg"))
        qrCodeMsg = aMap["qrCodeMsg"].toString();
    if (aMap.contains("plateNumber"))
        plateNumber = aMap["plateNumber"].toString();
    if (aMap.contains("tradeID"))
        tradeID = aMap["tradeID"].toString();

    QString dealtData;
    if (!qrCodeMsg.isEmpty()) {
        dealtData = getGreenVehicleWayFromScan(qrCodeMsg);
    } else if (!tradeID.isEmpty()) {
        dealtData = getGreenVehicleInfoFromTradeID(tradeID);
    } else if (!plateNumber.isEmpty()) {
        dealtData = getGreenVehicleInfoFromPlate(plateNumber);
    } else {
        throw BaseException(1, "响应失败: 车牌号或二维码为空");
    }

    return dealtData;
}

QString BizHandler::getGreenVehicleInfoFromPlate(const QString &plateNumber)
{
    NloJson nloJson;

    T_EtcOut objEtc;
    T_MtcOut objMtc;

    QString passID;
    QString tradeID;
    QString enTime;
    QString enNetHex;
    QString enHex;
    QString enName;
    int exitFeeType = 0, mediaType = 3, vehClass = 0, userType = 0, axisCount = 0, shouldPay = 0, discount = 0, factPay = 0, enWeight = 0,
        exWeight = 0;

    ST_Node exNode;
    exNode.nodeType = 1;
    if (m_ds.getLatestOutTradeInPlate(plateNumber, &objEtc, 0)) {
        // ETC出口记录
        passID = objEtc.PassID;
        tradeID = objEtc.TradeID;
        exitFeeType = objEtc.ExitFeeType;
        if (objEtc.CardType == 15)
            mediaType = 1;
        else if (objEtc.CardType == 22 || objEtc.CardType == 23)
            mediaType = 0;
        vehClass = objEtc.ExVehClass;
        userType = objEtc.UserType;
        axisCount = objEtc.Axises;
        shouldPay = objEtc.ShouldPay * 100;
        factPay = objEtc.FactPay * 100;
        discount = objEtc.Discount * 100;
        enWeight = objEtc.EnTotalWeight;
        exWeight = objEtc.TotalWeight;
        enTime = objEtc.EnTime.toString("yyyy-MM-dd HH:mm:ss");
        enHex = objEtc.EnStation;
        enNetHex = QString::number(objEtc.EnNetID);
        enName = objEtc.EnStationName;

        // 出口门架节点信息
        exNode.captureId = objEtc.TradeID;
        exNode.nodeHex = objEtc.ExStation;
        exNode.nodeId = objEtc.CNLaneID.left(14);
        exNode.nodeName = objEtc.ExStationName;
        exNode.passTime = objEtc.ExTime.toString("yyyy-MM-dd hh:mm:ss");
    } else if (m_ds.getLatestOutTradeInPlate(plateNumber, &objMtc, 1)) {
        // MTC出口记录
        passID = objMtc.PassID;
        tradeID = objMtc.TradeID;
        exitFeeType = objMtc.ExitFeeType;
        if (objMtc.CardType == 15)
            mediaType = 1;
        vehClass = objMtc.ExVehClass;
        userType = objMtc.UserType;
        axisCount = objMtc.Axises;
        shouldPay = objMtc.ShouldPay * 100;
        factPay = objMtc.FactPay * 100;
        discount = objMtc.Discount * 100;
        enWeight = objMtc.EnTotalWeight;
        exWeight = objMtc.TotalWeight;
        enTime = objMtc.EnTime.toString("yyyy-MM-dd HH:mm:ss");
        enHex = objMtc.EnStation;
        enNetHex = QString::number(objMtc.EnNetID);
        enName = objMtc.EnStationName;

        // 出口门架节点信息
        exNode.captureId = objMtc.TradeID;
        exNode.nodeHex = objMtc.ExStation;
        exNode.nodeId = objMtc.CNLaneID.left(14);
        exNode.nodeName = objMtc.ExStationName;
        exNode.passTime = objMtc.ExTime.toString("yyyy-MM-dd hh:mm:ss");
    } else {
        throw BaseException(1, "响应失败：未查询到相关记录");
    }

    if (passID.isEmpty()) {
        throw BaseException(1, "响应失败: 获取出口信息失败(passid为空)");
    }

    LOG_INFO().noquote() << QString("查询出口信息结果返回: exitFeeType %1, passID %2, name %3, tradeId %4")
                                .arg(exitFeeType)
                                .arg(passID, exNode.nodeName, tradeID);

    // 查询入口信息
    ST_Node enNode;
    enNode.nodeType = 1;
    QVariantMap enInfo = m_ds.getEnInfo(passID);
    if (!enInfo.isEmpty()) {
        enNode.captureId = enInfo["TRADEID"].toString();
        enNode.passTime = enInfo["ENTIME"].toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        enNode.nodeHex = enInfo["ENSTATION"].toString();
        enNode.nodeName = enInfo["STATIONNAME"].toString();
        enNode.nodeId = enInfo["CNLANEID"].toString().left(14);
    }

    if (enInfo.isEmpty() || enNode.captureId.isEmpty()) {
        LOG_INFO().noquote() << "查询入口信息为空，使用出口记录中的入口信息";
        enNode.captureId = "";
        enNode.passTime = enTime;
        enNode.nodeHex = enNetHex + enHex;
        enNode.nodeId = m_ds.getGantryNodeID(enNode.nodeHex);
        enNode.nodeName = enName;
    }
    LOG_INFO().noquote() << QString("查询入口信息结果返回 passID %1, name %2").arg(passID, enNode.nodeName);

    // 判断是否允许绿通通行
    QString greenPassMsg;
    int banType = m_ds.getGreenPassBanType(plateNumber);
    if (banType == 1) {
        greenPassMsg = "存在多次绿通混装";
    } else if (banType == 2) {
        greenPassMsg = "存在多次假冒绿通";
    } else {
        greenPassMsg = m_ds.getGreenPassAppointment(plateNumber) ? "存在绿通预约信息" : "";
    }

    QVariantMap sendMap;
    sendMap["status"] = 0;
    sendMap["desc"] = "";
    sendMap["plateNumber"] = Utils::BizUtils::getPlateNoColor(plateNumber);
    sendMap["plateColor"] = Utils::BizUtils::getColorCodeFromPlate(plateNumber);
    sendMap["enWeight"] = enWeight;
    sendMap["exWeight"] = exWeight;
    sendMap["greenPassStatus"] = (banType > 0 ? 1 : 0);
    sendMap["greenPassMsg"] = greenPassMsg;
    sendMap["enNode"] = nloJson.parse(packNode(enNode).toUtf8());
    sendMap["exNode"] = nloJson.parse(packNode(exNode).toUtf8());

    QStringList nodesList;
    getGantryNodes(passID, nodesList);
    QVariantList units;
    foreach (auto node, nodesList) {
        QVariantMap var = nloJson.parse(node.toUtf8()).toMap();
        units.append(var);
    }
    sendMap["units"] = units;

    QByteArray data = nloJson.serialize(sendMap);
    return data;
}

QString BizHandler::getGreenVehicleInfoFromTradeID(const QString &tradeId)
{
    NloJson nloJson;

    T_EtcOut objEtc;
    T_MtcOut objMtc;

    QString passID;
    QString enTime;
    QString enNetHex;
    QString enHex;
    QString enName;
    int exitFeeType = 0, mediaType = 3, vehClass = 0, userType = 0, axisCount = 0, shouldPay = 0, discount = 0, factPay = 0, enWeight = 0,
        exWeight = 0;
    QString vehPlate;

    ST_Node exNode;
    exNode.nodeType = 1;
    if (m_ds.getLatestOutTradeInTradeID(tradeId, &objEtc, 0)) {
        // ETC出口记录
        passID = objEtc.PassID;
        exitFeeType = objEtc.ExitFeeType;
        if (objEtc.CardType == 15)
            mediaType = 1;
        else if (objEtc.CardType == 22 || objEtc.CardType == 23)
            mediaType = 0;
        vehClass = objEtc.ExVehClass;
        userType = objEtc.UserType;
        axisCount = objEtc.Axises;
        shouldPay = objEtc.ShouldPay * 100;
        factPay = objEtc.FactPay * 100;
        discount = objEtc.Discount * 100;
        enWeight = objEtc.EnTotalWeight;
        exWeight = objEtc.TotalWeight;
        enTime = objEtc.EnTime.toString("yyyy-MM-dd HH:mm:ss");
        enHex = objEtc.EnStation;
        enNetHex = QString::number(objEtc.EnNetID);
        enName = objEtc.EnStationName;
        vehPlate = objEtc.ExVehPlate;

        // 出口门架节点信息
        exNode.captureId = objEtc.TradeID;
        exNode.nodeHex = objEtc.ExStation;
        exNode.nodeId = objEtc.CNLaneID.left(14);
        exNode.nodeName = objEtc.ExStationName;
        exNode.passTime = objEtc.ExTime.toString("yyyy-MM-dd hh:mm:ss");
    } else if (m_ds.getLatestOutTradeInTradeID(tradeId, &objMtc, 1)) {
        // MTC出口记录
        passID = objMtc.PassID;
        exitFeeType = objMtc.ExitFeeType;
        if (objMtc.CardType == 15)
            mediaType = 1;
        vehClass = objMtc.ExVehClass;
        userType = objMtc.UserType;
        axisCount = objMtc.Axises;
        shouldPay = objMtc.ShouldPay * 100;
        factPay = objMtc.FactPay * 100;
        discount = objMtc.Discount * 100;
        enWeight = objMtc.EnTotalWeight;
        exWeight = objMtc.TotalWeight;
        enTime = objMtc.EnTime.toString("yyyy-MM-dd HH:mm:ss");
        enHex = objMtc.EnStation;
        enNetHex = QString::number(objMtc.EnNetID);
        enName = objMtc.EnStationName;
        vehPlate = objMtc.ExVehPlate;

        // 出口门架节点信息
        exNode.captureId = objMtc.TradeID;
        exNode.nodeHex = objMtc.ExStation;
        exNode.nodeId = objMtc.CNLaneID.left(14);
        exNode.nodeName = objMtc.ExStationName;
        exNode.passTime = objMtc.ExTime.toString("yyyy-MM-dd hh:mm:ss");
    } else {
        throw BaseException(1, "响应失败：未查询到相关记录");
    }

    if (passID.isEmpty()) {
        throw BaseException(1, "响应失败: 获取出口信息失败(passid为空)");
    }

    LOG_INFO().noquote() << QString("查询出口信息结果返回: exitFeeType %1, passID %2, name %3, tradeId %4")
                                .arg(exitFeeType)
                                .arg(passID, exNode.nodeName, tradeId);

    // 查询入口信息
    ST_Node enNode;
    enNode.nodeType = 1;
    QVariantMap enInfo = m_ds.getEnInfo(passID);
    if (!enInfo.isEmpty()) {
        enNode.captureId = enInfo["TRADEID"].toString();
        enNode.passTime = enInfo["ENTIME"].toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        enNode.nodeHex = enInfo["ENSTATION"].toString();
        enNode.nodeName = enInfo["STATIONNAME"].toString();
        enNode.nodeId = enInfo["CNLANEID"].toString().left(14);
    }

    if (enInfo.isEmpty() || enNode.captureId.isEmpty()) {
        LOG_INFO().noquote() << "查询入口信息为空，使用出口记录中的入口信息";
        enNode.captureId = "";
        enNode.passTime = enTime;
        enNode.nodeHex = enNetHex + enHex;
        enNode.nodeId = m_ds.getGantryNodeID(enNode.nodeHex);
        enNode.nodeName = enName;
    }
    LOG_INFO().noquote() << QString("查询入口信息结果返回 passID %1, name %2").arg(passID, enNode.nodeName);

    // 判断是否允许绿通通行
    QString greenPassMsg;
    int banType = m_ds.getGreenPassBanType(vehPlate);
    if (banType == 1) {
        greenPassMsg = "存在多次绿通混装";
    } else if (banType == 2) {
        greenPassMsg = "存在多次假冒绿通";
    } else {
        greenPassMsg = m_ds.getGreenPassAppointment(vehPlate) ? "存在绿通预约信息" : "";
    }

    QVariantMap sendMap;
    sendMap["status"] = 0;
    sendMap["desc"] = "";
    sendMap["plateNumber"] = Utils::BizUtils::getPlateNoColor(vehPlate);
    sendMap["plateColor"] = Utils::BizUtils::getColorCodeFromPlate(vehPlate);
    sendMap["enWeight"] = enWeight;
    sendMap["exWeight"] = exWeight;
    sendMap["greenPassStatus"] = (banType > 0 ? 1 : 0);
    sendMap["greenPassMsg"] = greenPassMsg;
    sendMap["enNode"] = nloJson.parse(packNode(enNode).toUtf8());
    sendMap["exNode"] = nloJson.parse(packNode(exNode).toUtf8());

    QStringList nodesList;
    getGantryNodes(passID, nodesList);
    QVariantList units;
    foreach (auto node, nodesList) {
        QVariantMap var = nloJson.parse(node.toUtf8()).toMap();
        units.append(var);
    }
    sendMap["units"] = units;

    QByteArray data = nloJson.serialize(sendMap);
    return data;
}

QString BizHandler::getGreenVehicleWayFromScan(const QString &scan)
{
    NloJson nloJson;

    ST_GreenScanInfo scanInfo = getGreenPassScanInfo(scan);

    QString passID;
    QString tradeID;
    int exitFeeType = 0, mediaType = 3, vehClass = 0, userType = 0, axisCount = 0;
    int shouldPay = 0, discount = 0, factPay = 0;

    // 获取出口信息
    passID = scanInfo.passId;
    if (passID.left(2) == "01") {
        mediaType = 0;
    } else if (passID.left(2) == "02") {
        mediaType = 1;
    }
    shouldPay = scanInfo.payFee;
    factPay = scanInfo.fee;
    discount = shouldPay - factPay;

    ST_Node exNode;
    exNode.nodeType = 1;
    exNode.captureId = "";
    exNode.nodeHex = m_ds.getGantryHexNode(scanInfo.exStationId);
    exNode.nodeId = scanInfo.exStationId;
    exNode.nodeName = m_ds.getGantryNodeName(scanInfo.exStationId);
    exNode.passTime = scanInfo.exTime;

    if (passID.isEmpty()) {
        throw BaseException(1, "响应失败: 解析二维码信息失败(passid为空)");
    }

    // 查询入口信息
    ST_Node enNode;
    enNode.nodeType = 1;
    QVariantMap enInfo = m_ds.getEnInfo(passID);
    if (!enInfo.isEmpty()) {
        enNode.captureId = enInfo["TRADEID"].toString();
        enNode.passTime = enInfo["ENTIME"].toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        enNode.nodeHex = enInfo["ENSTATION"].toString();
        enNode.nodeName = enInfo["STATIONNAME"].toString();
        enNode.nodeId = enInfo["CNLANEID"].toString().left(14);
    }

    if (enInfo.isEmpty() || enNode.captureId.isEmpty()) {
        LOG_INFO().noquote() << "查询入口信息为空，使用出口记录中的入口信息";
        enNode.captureId = "";
        enNode.passTime = QDateTime::fromString(scanInfo.passId.right(14), "yyyyMMddhhmmss").toString("yyyy-MM-dd hh:mm:ss");
        enNode.nodeHex = m_ds.getGantryHexNode(scanInfo.enStationId);
        enNode.nodeId = m_ds.getGantryNodeID(enNode.nodeHex);
        enNode.nodeName = m_ds.getGantryNodeName(scanInfo.enStationId);
    }
    LOG_INFO().noquote() << QString("查询入口信息结果返回 passID %1, name %2").arg(passID, enNode.nodeName);

    // 组装发送报文
    ST_PathFitting pathFitting;
    pathFitting.plateNumber = scanInfo.plateNumber;
    pathFitting.plateColor = scanInfo.plateColor;
    pathFitting.mediaType = mediaType;
    pathFitting.vehicleType = vehClass;
    pathFitting.vehicleClass = userType;
    pathFitting.receivableMoney = shouldPay;
    pathFitting.tradingMoney = factPay;
    pathFitting.discountsMoney = discount;
    pathFitting.axesCount = axisCount;

    pathFitting.enNode = packNode(enNode);
    pathFitting.exNode = packNode(exNode);
    getGantryNodes(passID, pathFitting.gantryUnits);
    QString sendJson = packFeePath(pathFitting, exitFeeType);

    LOG_INFO().noquote() << "发送地图路径请求: " << sendJson;
    QUrl url(GM_INSTANCE->m_config->m_baseConfig.mapUrl);
    QByteArray result;
    bool ok = Http::instance().postSync(result, url, sendJson.toUtf8(), "application/json");
    if (!ok) {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    LOG_INFO().noquote() << "接收地图路径完成: " << result.left(1024);
    QVariantMap resMap = nloJson.parse(result).toMap();
    QVariantMap bizMap = resMap.value("bizContent", {}).toMap();
    QByteArray sendData = nloJson.serialize(bizMap);

    return sendData;
}

ST_GreenScanInfo BizHandler::getGreenPassScanInfo(const QString &scan)
{
    ST_GreenScanInfo var;
    QStringList fields = scan.split('|');

    QString fullVehplate = fields[0];
    var.plateNumber = fullVehplate.section('_', 0, 0);
    var.plateColor = fullVehplate.section('_', 1, 1).toInt();

    var.enStationId = fields[1];
    var.exStationId = fields[2];
    var.enWeight = fields[3].toInt();
    var.exWeight = fields[4].toInt();
    var.mediaType = fields[5].toInt();
    var.transactionId = fields[6];
    var.passId = fields[7];

    var.exTime = fields[8].replace("T", " ");
    var.transPayType = fields[9].toInt();
    var.fee = fields[10].toLong();
    var.payFee = fields[11].toLong();
    var.vehicleSign = fields[12];
    var.provinceNum = fields[13].toInt();

    return var;
}

QString BizHandler::doDealCmd20(const QVariantMap &aMap)
{
    NloJson nloJson;

    QString captureId;
    int nodeType = -1;
    QString plateNumber;
    QString gantryId;
    QString tradeTime;
    QString picId;

    if (aMap.contains("captureId"))
        captureId = aMap["captureId"].toString();
    if (aMap.contains("nodeType"))
        nodeType = aMap["nodeType"].toInt();
    if (aMap.contains("plateNumber"))
        plateNumber = aMap["plateNumber"].toString();
    if (aMap.contains("gantryId"))
        gantryId = aMap["gantryId"].toString();
    if (aMap.contains("tradeTime"))
        tradeTime = aMap["tradeTime"].toString();
    if (aMap.contains("picId"))
        picId = aMap["picId"].toString();

    QString imageData;
    if (nodeType == 0) { // 获取门架图片
        imageData = getGantryImage(plateNumber, gantryId, tradeTime);
        int pos = imageData.indexOf(",");
        if (pos > 0)
            imageData = imageData.left(pos);
    } else if (nodeType == 1) { // 获取车道图片
        imageData = getLaneImage(captureId);
    } else if (nodeType == 2) { // 获取集装箱图片
        imageData = getContainerImage(picId);
    } else if (nodeType == 3) { // 获取稽核审核图片
        QString stationId;
        if (aMap.contains("stationID"))
            stationId = aMap["stationID"].toString();
        imageData = getDiscardTicketReviewImage(picId, stationId);
    } else {
        throw BaseException(1, QString("响应失败：节点类型错误(nodeType %1)").arg(nodeType));
    }

    QVariantMap map;
    map["status"] = 0;
    map["desc"] = "成功获取到图片";
    map["picData"] = imageData;
    QString dealtData = nloJson.serialize(map);

    return dealtData;
}

QString BizHandler::getGantryImage(const QString &plateNumber, const QString &gantryId, const QString &tradeTime)
{
    NloJson nloJson;

    if (plateNumber.isEmpty())
        throw BaseException(1, "响应失败: 车牌号为空");
    if (gantryId.isEmpty())
        throw BaseException(1, "响应失败: 门架全网编号为空");
    if (tradeTime.isEmpty())
        throw BaseException(1, "响应失败: 交易时间为空");

    QVariantMap sendMap;
    sendMap["gantryId"] = gantryId;
    sendMap["tradeTime"] = tradeTime;
    sendMap["vehPlate"] = plateNumber;
    QString sendData = nloJson.serialize(sendMap);
    LOG_INFO().noquote() << "获取门架图片请求: " << sendData;

    QUrl url(GM_INSTANCE->m_config->m_baseConfig.gantryPicUrl);
    QByteArray result;
    int defaultTimeOut = Http::instance().getReadTimeout();
    Http::instance().setReadTimeout(30 * 1000);
    bool ok = Http::instance().postSync(result, url, sendData.toUtf8(), "application/json");
    Http::instance().setReadTimeout(defaultTimeOut);
    if (!ok) {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    LOG_INFO().noquote() << "返回门架图片数据: " << result.left(1024);
    return result;
}

QString BizHandler::getLaneImage(const QString &captureId)
{
    NloJson nloJson;

    if (captureId.isEmpty())
        throw BaseException(1, "响应失败: 牌识Id为空");

    QVariantMap sendMap;
    sendMap["tradeId"] = captureId;
    sendMap["orgId"] = captureId.left(4);
    QString sendData = nloJson.serialize(sendMap);
    LOG_INFO().noquote() << "获取车道图片请求: " << sendData;

    QUrl url(GM_INSTANCE->m_config->m_baseConfig.lanePicUrl);
    QByteArray result;
    bool ok = Http::instance().postSync(result, url, sendData.toUtf8(), "application/json");
    if (!ok) {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    LOG_INFO().noquote() << "返回车道图片数据: " << result.left(1024);
    return result;
}

QString BizHandler::getContainerImage(const QString &picName)
{
    if (picName.isEmpty())
        throw BaseException(1, "响应失败: 图片ID为空");

    Utils::FileName picturePath = GM_INSTANCE->m_pictureDir + QString("/%1").arg(picName);

    if (!picturePath.exists()) {
        LOG_WARNING().noquote() << QString("图片文件不存在 %1.jgp").arg(picName);
        throw BaseException(1, "响应失败: 图片文件不存在");
    }

    Utils::FileReader fileReader;
    bool ok = fileReader.fetch(picturePath.toString());
    if (!ok) {
        LOG_ERROR().noquote() << QString("读取图片文件 %1.jgp 失败").arg(picName);
        throw BaseException(1, "响应失败: 读取图片文件失败");
    }

    QByteArray imageData = fileReader.data();

    QString result = imageData.toBase64();
    LOG_INFO().noquote() << "返回本地图片数据: " << result.left(1024);
    return result;
}

QString BizHandler::doDealCmd23(const QVariantMap &aMap)
{
    NloJson nloJson;

    int operateType = 0;
    QVariantMap operateData;
    QString stationID;
    int laneID = 0;
    QString deviceNumber;

    if (aMap.contains("operateType"))
        operateType = aMap["operateType"].toInt();
    if (aMap.contains("operateData"))
        operateData = aMap["operateData"].toMap();
    if (aMap.contains("stationID"))
        stationID = aMap["stationID"].toString();
    if (aMap.contains("laneID"))
        laneID = aMap["laneID"].toInt();
    if (aMap.contains("deviceNumber"))
        deviceNumber = aMap["deviceNumber"].toString();

    if (operateType != 1 && operateType != 2)
        throw BaseException(1, "响应失败: 操作类型异常");
    if (operateData.isEmpty())
        throw BaseException(1, "响应失败: 操作数据为空");
    if (stationID.isEmpty())
        throw BaseException(1, "响应失败: 站代码为空");
    if (laneID == 0)
        throw BaseException(1, "响应失败：车道号为空");
    if (deviceNumber.isEmpty())
        throw BaseException(1, "响应失败: 设备编号为空");

    QString reqBody = nloJson.serialize(aMap);
    LOG_INFO().noquote() << "车道设备控制请求: " << reqBody;

    QString laneIP = m_ds.getLaneIP(stationID, laneID);
    QString devCtrlurl = QString("http://%1:13592/devCtrl").arg(laneIP);
    QUrl url(devCtrlurl);
    QByteArray result;
    bool okNet = Http::instance().postSync(result, url, reqBody.toUtf8(), "application/json");
    if (!okNet) {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    bool ok = false;
    QVariantMap resMap = nloJson.parse(result, &ok).toMap();
    if (!ok)
        throw BaseException(1, "响应失败: 解析车道设备控制返回内容异常");

    int status = resMap["status"].toInt();
    if (status != 0) {
        resMap["status"] = 1;
    }
    QString dealtData = nloJson.serialize(resMap);
    return dealtData;
}

QString BizHandler::getDiscardTicketReviewImage(const QString &picName, const QString &stationId)
{
    if (picName.isEmpty())
        throw BaseException(1, "响应失败: 图片ID为空");
    if (stationId.isEmpty())
        throw BaseException(1, "响应失败: 站代码为空");

    QRegularExpression re(R"(^ImageQ_(.+)$)");
    QRegularExpressionMatch match = re.match(picName);

    QString tradeId;
    if (match.hasMatch()) {
        tradeId = match.captured(1);
    } else {
        throw BaseException(1, "响应失败: 图片命名错误");
    }

    QString stationIP = m_ds.getStationIP(stationId);
    QString stationServiceUrl = QString("http://%1:8082").arg(stationIP);

    QString result;
    QVariantList resList = m_ds.getTicketReviewPic(stationId, tradeId, QUrl(stationServiceUrl));
    if (!resList.isEmpty()) {
        QVariantMap resMap = resList.first().toMap();
        result = resMap["picdata"].toString();
    } else {
        LOG_WARNING().noquote() << "未查询到相关稽核审核图片";
    }
    LOG_INFO().noquote() << "返回稽核审核图片数据: " << result.left(1024);
    return result;
}

QString BizHandler::doDealCmd24(const QByteArray &reqBody)
{
    LOG_INFO().noquote() << "车牌识别获取请求: " << QString::fromUtf8(reqBody.left(1024));

    QUrl url(GM_INSTANCE->m_config->m_baseConfig.plateOcrUrl);
    QByteArray result;
    bool okNet = Http::instance().postSync(result, url, reqBody, "application/json");
    if (!okNet) {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    NloJson nloJson;

    bool ok = false;
    QVariantMap resMap = nloJson.parse(result, &ok).toMap();
    if (!ok)
        throw BaseException(1, "响应失败: 解析车牌识别返回内容异常");
    QString data = nloJson.serialize(resMap["data"]);

    LOG_INFO().noquote() << "车牌识别内容返回: " << data;
    return data;
}

QString BizHandler::doDealCmd25(const QVariantMap &aMap)
{
    QString vehicleId;
    QString cardId;
    if (aMap.contains("vehicleId"))
        vehicleId = aMap["vehicleId"].toString();
    if (aMap.contains("cardId"))
        cardId = aMap["cardId"].toString();

    if (vehicleId.isEmpty() && cardId.isEmpty())
        throw BaseException(1, "响应失败: 车牌号与卡号为空");

    NloJson nloJson;

    QVariantMap map;
    map["vehicleId"] = vehicleId;
    map["cardNum"] = cardId;
    QString sendData = nloJson.serialize(map);
    LOG_INFO().noquote() << "状态名单信息查询请求: " << sendData;

    QString url(GM_INSTANCE->m_config->m_baseConfig.blackStatusUrl);
    QByteArray result;
    bool ok = Http::instance().postSync(result, url, sendData.toUtf8(), "application/json");
    if (!ok) {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    LOG_INFO().noquote() << "返回状态名单信息查询结果: " << result;
    QVariantMap resMap = nloJson.parse(result).toMap();
    int subCode = resMap["subCode"].toInt();  // 返回码
    QString info = resMap["info"].toString(); // 信息

    resMap["status"] = subCode == 200 ? 0 : 1;
    resMap["desc"] = info;
    QString dealtData = nloJson.serialize(resMap);
    return dealtData;
}

QString BizHandler::doDealCmd26(const QVariantMap &aMap)
{
    QString stationId;
    QString shiftDate;
    int shiftId = 0;
    int laneId = 0;
    QString userId;
    if (aMap.contains("stationId"))
        stationId = aMap["stationId"].toString();
    if (aMap.contains("shiftDate"))
        shiftDate = aMap["shiftDate"].toString();
    if (aMap.contains("shiftID"))
        shiftId = aMap["shiftID"].toInt();
    if (aMap.contains("laneID"))
        laneId = aMap["laneID"].toInt();
    if (aMap.contains("operator"))
        userId = aMap["operator"].toString();

    if (stationId.isEmpty())
        throw BaseException(1, "响应失败: 站代码为空");
    if (shiftDate.isEmpty())
        throw BaseException(1, "响应失败: 班次日期为空");
    if (shiftId == 0)
        throw BaseException(1, "响应失败: 班次号为空");
    if (laneId == 0)
        throw BaseException(1, "响应失败：车道号为空");
    if (userId.isEmpty())
        throw BaseException(1, "响应失败: 员工号为空");

    NloJson nloJson;

    T_LaneInputShift laneInputShift;
    QJson::QObjectHelper::qvariant2qobject(aMap, &laneInputShift);
    laneInputShift.UpdateTime = QDateTime::currentDateTime();
    LOG_INFO().noquote() << "交接班数据: " << laneInputShift.toString();

    QString dtpContent = Utils::BizUtils::makeDtpContentFromObj(laneInputShift);
    QString fromNode = stationId.rightJustified(4, QLatin1Char('0')) + QString::number(laneId).rightJustified(2, QLatin1Char('0'));
    QString dtpXml = Utils::BizUtils::makeDtpXml(dtpContent, "531", fromNode, stationId, 1);
    LOG_INFO().noquote() << "交接班数据DTP报文: " << dtpXml;

    QString stationIP = m_ds.getStationIP(stationId);
    int res = GM_INSTANCE->m_dtpSender->sendMsgToDtp(stationIP, 13591, "TradeQ", "", dtpXml);

    if (res < 0)
        throw BaseException(1, "响应失败: 站级数据传输异常");

    QVariantMap map;
    map["status"] = 0;
    map["desc"] = "成功录入交接班数据";
    QString dealtData = nloJson.serialize(map);
    return dealtData;
}

QString BizHandler::doDealCmd27(const QVariantMap &aMap)
{
    QString stationID;
    QString shiftDate;
    int shiftID = -1;
    int laneID = -1;
    QString userID;
    int specialType = -1;
    int specialSubType = -1;
    QString specialMedium;
    QString desc;

    if (aMap.contains("stationId"))
        stationID = aMap["stationId"].toString();
    if (aMap.contains("shiftDate"))
        shiftDate = aMap["shiftDate"].toString();
    if (aMap.contains("shiftID"))
        shiftID = aMap["shiftID"].toInt();
    if (aMap.contains("laneID"))
        laneID = aMap["laneID"].toInt();
    if (aMap.contains("operator"))
        userID = aMap["operator"].toString();
    if (aMap.contains("specialType"))
        specialType = aMap["specialType"].toInt();
    if (aMap.contains("specialSubType"))
        specialSubType = aMap["specialSubType"].toInt();
    if (aMap.contains("specialMedium"))
        specialMedium = aMap["specialMedium"].toString();
    if (aMap.contains("desc"))
        desc = aMap["desc"].toString();

    if (stationID.isEmpty())
        throw BaseException(1, "响应失败: 收费站代码为空");
    if (shiftDate.isEmpty())
        throw BaseException(1, "响应失败: 班次日期为空");
    if (shiftID == -1)
        throw BaseException(1, "响应失败: 班次号为空");
    if (laneID == -1)
        throw BaseException(1, "响应失败: 车道号为空");
    if (userID.isEmpty())
        throw BaseException(1, "响应失败: 登记员工号为空");
    if (specialType == -1)
        throw BaseException(1, "响应失败: 特情类型为空");
    if (specialSubType == -1)
        throw BaseException(1, "响应失败: 特情子类型为空");
    if (specialMedium.isEmpty())
        throw BaseException(1, "响应失败: 特情介质号为空");

    NloJson nloJson;

    // map对齐表结构
    QVariantMap resMap = aMap;
    resMap.insert("updateTime", QDateTime::currentDateTime());
    resMap.insert("dataID", stationID + QString("%1").arg(laneID, 2, 10, QChar('0')) + QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    resMap.insert("cardID", specialMedium);

    QString operatorName = m_ds.getUserName(userID, 1);
    resMap.insert("inputOperator", userID);
    resMap.insert("operatorName", operatorName);
    resMap.insert("inputOperatorName", operatorName);

    resMap["specialType"] = specialSubType; // NOTE 特别处理

    T_SpecialCards specialCard;
    QJson::QObjectHelper::qvariant2qobject(resMap, &specialCard);
    LOG_INFO().noquote() << "特情卡记录: " << specialCard.toString();

    QString stationIP = m_ds.getStationIP(stationID);
    QString stationServiceUrl = QString("http://%1:8082").arg(stationIP);

    if (m_ds.getSpecialCardExist(specialCard, QUrl(stationServiceUrl)))
        throw BaseException(1, "响应失败: 该特情卡记录已存在");

    if (!m_ds.insertSpecialCard(specialCard, QUrl(stationServiceUrl)))
        throw BaseException(1, "响应失败: 录入特情卡数据失败");

    QVariantMap map;
    map["status"] = 0;
    map["desc"] = "成功录入特情卡数据";
    QString dealtData = nloJson.serialize(map);
    return dealtData;
}

QString BizHandler::doDealCmd28(const QVariantMap &aMap)
{
    NloJson nloJson;

    QString cardId;
    if (aMap.contains("cardId"))
        cardId = aMap["cardId"].toString();
    if (cardId.isEmpty())
        throw BaseException(1, "响应失败: 身份卡号为空");

    QString userId = m_ds.getUserID(cardId);
    QString userName = m_ds.getUserName(cardId, 0);

    if (userId.isEmpty() && userName.isEmpty())
        throw BaseException(1, "响应失败: 未查询到相关用户信息");

    LOG_INFO().noquote() << QString("返回用户信息: cardId %1, userName %2, userId %3").arg(cardId, userName, userId);
    QVariantMap map;
    map["status"] = 0;
    map["desc"] = "";
    map["operator"] = userId;
    map["operatorName"] = userName;
    QString dealtData = nloJson.serialize(map);
    return dealtData;
}

QString BizHandler::doDealCmd30(const QVariantMap &aMap)
{
    NloJson nloJson;

    QString vehicleId;
    if (aMap.contains("vehicleId"))
        vehicleId = aMap["vehicleId"].toString();

    if (vehicleId.isEmpty())
        throw BaseException(1, "响应失败: 车牌号为空");

    int colorIndex = Utils::BizUtils::getColorCodeFromPlate(vehicleId);
    QString plateNoColor = Utils::BizUtils::getPlateNoColor(vehicleId);

    LOG_INFO().noquote() << QString("查询逃漏费车辆 %1 欠费列表").arg(vehicleId);
    QString encodedVehicleId = QUrl::toPercentEncoding(QString("%1_%2").arg(plateNoColor).arg(colorIndex));
    QUrl url(GM_INSTANCE->m_config->m_baseConfig.arrearsUrl + "/" + encodedVehicleId);
    // 跳过证书校验
    Http().instance().setSkipVerify(true);

    QByteArray result;
    bool ok = Http::instance().getSync(result, url);
    Http().instance().setSkipVerify(false);
    if (!ok) {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    LOG_INFO().noquote() << "返回逃漏费车辆欠费列表: " << result.left(1024);

    QVariantMap resMap = nloJson.parse(result).toMap();
    bool success = resMap["success"].toBool();
    if (!success)
        throw BaseException(1, "响应失败: 欠费查询接口返回失败");

    QVariantList audits = resMap["data"].toList();
    if (audits.isEmpty())
        throw BaseException(1, "响应失败: 未查询到欠费信息");
    for (auto &var : audits) {
        QVariantMap auditMap = var.toMap();

        QString id = auditMap["id"].toString();
        QString enStationId = auditMap["realEnStationId"].toString();
        QString exStationId = auditMap["realExStationId"].toString();
        QString passId = auditMap["passId"].toString();
        QString escapeType = auditMap["confirmedEscapeType"].toString();
        QString escapeTypeDesc = auditMap["confirmedEscapeTypeName"].toString();
        QString remark = auditMap["remark"].toString();
        bool isPreBlack = auditMap["isPreBlack"].toBool();

        // 缓存稽核对象，补费上传时使用
        ST_AuditInfo auditInfo;
        auditInfo.isPreBlack = isPreBlack;
        auditInfo.enStationId = enStationId;
        auditInfo.exStationId = exStationId;
        auditInfo.passId = passId;
        auditInfo.escapeType = escapeType;
        auditInfo.escapeTypeDesc = escapeTypeDesc;
        auditInfo.remark = remark;
        m_auditInfos.insert(id, auditInfo);

        // 提高返回数据可读性
        auditMap["realEnStationName"] = enStationId.isEmpty() ? "" : m_ds.getStationName(enStationId);
        auditMap["realExStationName"] = exStationId.isEmpty() ? "" : m_ds.getStationName(exStationId);
        auditMap["isPreBlack"] = isPreBlack ? 1 : 0;

        var = auditMap;
    }

    QVariantMap map;
    map["status"] = 0;
    map["desc"] = "";
    map["audits"] = audits;

    QString dealtData = nloJson.serialize(map);
    return dealtData;
}

QString BizHandler::doDealCmd31(QVariantMap aMap)
{
    int scanType = -1;
    QString stationId;
    QString tradeNum;
    if (aMap.contains("scanType"))
        scanType = aMap.take("scanType").toInt();
    if (aMap.contains("operateStation"))
        stationId = aMap["operateStation"].toString();
    if (aMap.contains("tradeNum"))
        tradeNum = aMap["tradeNum"].toString();

    if (scanType != 0 && scanType != 1 && scanType != 2)
        throw BaseException(1, "响应失败: 操作类型异常");
    if (stationId.isEmpty())
        throw BaseException(1, "响应失败: 站代码为空");

    NloJson nloJson;

    if (tradeNum.isEmpty()) {
        // 第一次请求时，该字段手持机传空。此时由后端自行组装
        int tradeNumSuffix = getUniqueTradeNum(stationId) + 1000000000;
        // 更新唯一交易号
        m_ds.updateEmgcSeqNum(stationId);
        tradeNum = QString("%1AD%2").arg(stationId).arg(tradeNumSuffix);
        aMap["tradeNum"] = tradeNum;
    }
    // 支付失败，重新支付或核单时，对于同一笔交易，手持机填上次返回的TradeID

    QString payId = aMap["payId"].toString();
    QByteArray sendData;
    if (scanType == 0) {
        int payMeans = aMap["payMeans"].toInt();

        QString auditId;
        QString passId;
        QString escapeType;
        QString escapeTypeDesc;
        QString enStation;
        QString exStation;
        QString remark;
        int isPreBlack = -1;
        QVariantList resultIds = aMap["resultIds"].toList();
        int auditCount = resultIds.size();

        if (auditCount == 1) {
            auditId = resultIds.first().toString();
            passId = m_auditInfos[auditId].passId;
            escapeType = m_auditInfos[auditId].escapeType;
            escapeTypeDesc = m_auditInfos[auditId].escapeTypeDesc;
            enStation = m_auditInfos[auditId].enStationId;
            exStation = m_auditInfos[auditId].exStationId;
            isPreBlack = m_auditInfos[auditId].isPreBlack;
            remark = m_auditInfos[auditId].remark;
        } else {
            QStringList idList;
            QStringList passIdList;
            QStringList escapeTypeList;
            QStringList escapeTypeDescList;
            QStringList remarkList;
            for (const auto &resultId : resultIds) {
                QString var = resultId.toString();
                idList << var;
                passIdList << m_auditInfos[var].passId;
                escapeTypeList << m_auditInfos[var].escapeType;
                escapeTypeDescList << m_auditInfos[var].escapeTypeDesc;
                remarkList << m_auditInfos[var].remark;
            }
            auditId = idList.join(";");
            passId = passIdList.join(";");
            escapeType = escapeTypeList.join(";");
            escapeTypeDesc = escapeTypeDescList.join(";");
            remark = remarkList.join(";");
            enStation = "";
            exStation = "";
            isPreBlack = 0;
        }

        if (payMeans != 0) {
            // 发起第三方支付，支付系统LaneTradeId生成格式：LPDA{stationid}AD{yyyyMMddHHmmss}{seqNum}
            if (payId.isEmpty()) {
                int randNum = Utils::DataDealUtils::getRandomNum(1000);
                QString randNumStr = QString("%1").arg(randNum, 3, 10, QChar('0'));
                payId = "LPDA" + stationId + "AD" + Utils::DataDealUtils::curDateTimeStr("yyyyMMddhhmmss") + randNumStr;
                aMap["payId"] = payId;
            }

            QVariantMap resMap = cloudPay(payId, aMap);

            int payState = resMap["paystate"].toInt();
            int errorCode = resMap["errorcode"].toInt();
            QString message = resMap["message"].toString();

            // 支付失败，返回TradeId用于再次核单 NOTE 错误码300001，表示订单已支付
            if (payState != 0 && errorCode != 300001) {
                QVariantMap map;
                if (errorCode == 100001) {
                    map["status"] = 2;
                } else {
                    map["status"] = 1;
                }
                map["desc"] = QString("响应失败: %1(%2,%3)").arg(message).arg(errorCode).arg(getErrInfo(errorCode));
                map["tradeId"] = tradeNum;
                map["payId"] = payId; // 用于后续核单
                map["paySuccess"] = 1;
                sendData = nloJson.serialize(map);
                return sendData;
            }
        }

        // 支付成功，将支付结果上传补费接口
        QString scanCode = aMap.take("scanCode").toString();
        int laneId = aMap.take("laneId").toInt();
        QString shiftDate = aMap.take("shiftDate").toString();
        int shiftId = aMap.take("shiftId").toInt();
        int vehClass = aMap.take("vehClass").toInt();
        QString vehicleId = aMap["vehicleId"].toString();

        // DTP下发站级的T_AuditPayBack表（报文提前打印）
        T_AuditPayBack auditPayBack;
        auditPayBack.TradeNum = tradeNum.mid(6).toInt();
        auditPayBack.StationID = stationId;
        auditPayBack.ShiftDate = QDateTime::fromString(shiftDate, "yyyy-MM-dd hh:mm:ss");
        auditPayBack.ShiftID = shiftId;
        QStringList parts = vehicleId.split('_');
        auditPayBack.VehPlate = parts.value(0);
        auditPayBack.VehPlateColor = parts.value(1).toInt();
        auditPayBack.PayFee = aMap["paybackFee"].toInt() / 100.0; // 以分为单位
        auditPayBack.VehClass = vehClass;
        auditPayBack.DataType = 2; // 手持机稽核
        auditPayBack.Remark = remark;
        auditPayBack.Operator = aMap["operator"].toString();
        auditPayBack.OperatorName = m_ds.getUserName(auditPayBack.Operator, 1);
        auditPayBack.OperateTime = Utils::DataDealUtils::curDateTime();
        auditPayBack.Status = 2; // 已处理
        auditPayBack.PaybackUser = aMap["paybackUser"].toString();
        auditPayBack.PaybackUserPhone = aMap["paybackUserPhone"].toString();
        auditPayBack.TransactionType = aMap["transactionType"].toInt();
        auditPayBack.PaybackWay = aMap["paybackWay"].toInt();
        auditPayBack.EtcCardId = aMap["etcCardId"].toString();
        auditPayBack.AuditId = auditId;
        if (auditPayBack.TransactionType == 1) { // 电子支付
            auditPayBack.PayType = 1;
        } else if (auditPayBack.TransactionType == 2) { // 现金交易
            auditPayBack.PayType = 0;
        } else { // 第三方支付
            auditPayBack.PayType = 2;
        }
        if (auditPayBack.PayType == 2) {
            auditPayBack.PayMeans = payMeans;
            auditPayBack.PayCode = scanCode;
            auditPayBack.OrderNum = payId;
            auditPayBack.ReqPayTime = aMap["paybackTime"].toDateTime();
        } else {
            auditPayBack.PayMeans = 0;
            auditPayBack.PayCode = "";
            auditPayBack.OrderNum = "";
            auditPayBack.ReqPayTime = Utils::DataDealUtils::curDateTime();
        }
        auditPayBack.SyncFlag = 1;
        auditPayBack.EnStation = enStation;
        auditPayBack.ExStation = exStation;
        auditPayBack.IsPreBlack = isPreBlack;
        auditPayBack.PassID = passId;
        auditPayBack.EscapeType = escapeType;
        auditPayBack.EscapeTypeDesc = escapeTypeDesc;

        QString dtpContent = Utils::BizUtils::makeDtpContentFromStr(QStringList() << Utils::DataDealUtils::getInsertSql(&auditPayBack));
        QString dtpXml = Utils::BizUtils::makeDtpXml(dtpContent, "610", "1590", stationId, 1);
        LOG_INFO().noquote() << "稽核补费DTP报文: " << dtpXml;

        sendData = nloJson.serialize(aMap);
        LOG_INFO().noquote() << "请求稽核补费结果上传: " << QString::fromUtf8(sendData);

        QUrl url(GM_INSTANCE->m_config->m_baseConfig.payBackUrl + "/" + vehicleId);
        QByteArray result;
        Http::instance().setSkipVerify(true);
        bool okNet = Http::instance().postSync(result, url, sendData, "application/json");
        Http::instance().setSkipVerify(false);
        if (!okNet) {
            QVariantMap map;
            map["status"] = 1;
            map["desc"] = QString("响应失败: %1").arg(QString::fromUtf8(result));
            map["tradeId"] = tradeNum;
            map["payId"] = payId;
            map["paySuccess"] = 0;
            sendData = nloJson.serialize(map);
            return sendData;
        }

        LOG_INFO().noquote() << "返回稽核补费上传结果: " << result.left(1024);
        QVariantMap tempMap = nloJson.parse(result).toMap();
        bool ok = tempMap["success"].toBool();

        if (ok) {
            // 清理补缴对象缓存
            for (const auto &resultId : resultIds)
                m_auditInfos.remove(resultId.toString());

            QString stationIP = m_ds.getStationIP(stationId);
            int res = GM_INSTANCE->m_dtpSender->sendMsgToDtp(stationIP, 13591, "TradeQ", "", dtpXml);
            QVariantMap map;
            if (res < 0) {
                map["status"] = 1;
                map["desc"] = "响应失败: 站级数据传输失败(请联系运维人员处理)";
            } else {
                map["status"] = 0;
                map["desc"] = "稽核补费成功";
            }
            map["tradeId"] = tradeNum;
            map["payId"] = payId;
            map["paySuccess"] = 0;
            sendData = nloJson.serialize(map);
            return sendData;
        } else {
            QVariantMap map;
            map["status"] = 1;
            map["desc"] = "响应失败: 稽核补费接口上传失败";
            map["tradeId"] = tradeNum;
            map["payId"] = payId;
            map["paySuccess"] = 0;
            sendData = nloJson.serialize(map);
            return sendData;
        }
    } else if (scanType == 1) {
        // 核单
        QString scanCode = aMap["scanCode"].toString();

        QVariantMap billMap;
        if (!payId.isEmpty()) {
            billMap = getBillState(payId);
        } else if (!scanCode.isEmpty()) {
            billMap = getBillState(scanCode);
        } else {
            throw BaseException(1, "响应失败: 支付核验单号未知");
        }

        int billState = billMap["BillState"].toInt();
        QString billMessage = billMap["Message"].toString();

        QVariantMap resMap;
        if (billState == 0) {
            resMap["status"] = 0;
            QString payedTime = billMap["PayedTime"].toDateTime().toString("yyyy-MM-dd hh:mm:ss");
            resMap["desc"] = QString("订单已支付(%1,%2)").arg(payedTime).arg(billMessage);
        } else {
            resMap["status"] = 1;
            resMap["desc"] = QString("订单状态异常(%1,%2)").arg(billState).arg(billMessage);
        }
        resMap["tradeId"] = tradeNum;
        resMap["payId"] = payId;
        sendData = nloJson.serialize(resMap);
        return sendData;
    } else {
        // 退款
        if (payId.isEmpty())
            throw BaseException(1, "响应失败：支付订单号为空");

        QVariantMap resMap = refund(payId, aMap);

        int errorCode = resMap["errorcode"].toInt();
        QString message = resMap["message"].toString();

        QVariantMap map;
        if (errorCode == 0) {
            map["status"] = 0;
            map["desc"] = "第三方支付退款成功";
        } else {
            map["status"] = 1;
            map["desc"] = QString("响应失败: 第三方支付退款失败(%1,%2)").arg(errorCode).arg(message);
        }
        map["tradeId"] = tradeNum;
        map["payId"] = payId;
        QByteArray sendData = nloJson.serialize(map);
        return sendData;
    }
}

QVariantMap BizHandler::cloudPay(const QString &tradeNum, const QVariantMap &aMap)
{
    QString authCode;
    int factPay = 0;
    QString vehicleId;

    if (aMap.contains("scanCode"))
        authCode = aMap["scanCode"].toString();
    if (aMap.contains("paybackFee"))
        factPay = aMap["paybackFee"].toInt();
    if (aMap.contains("vehicleId"))
        vehicleId = aMap["vehicleId"].toString();

    if (authCode.isEmpty())
        throw BaseException(1, "响应失败: 付款码为空");
    if (factPay == 0)
        throw BaseException(1, "响应失败: 补费金额异常");
    if (vehicleId.isEmpty())
        throw BaseException(1, "响应失败: 车牌号为空");

    NloJson nloJson;

    QString exStationId = aMap["operateStation"].toString();
    int laneId = 249;
    QString vehPlate = vehicleId;
    QString timeStamp = Utils::DataDealUtils::curDateTimeStr();
    QString enStationId = "-";
    QString enTimeStamp = Utils::DataDealUtils::curDateTimeStr();
    int randNum = Utils::DataDealUtils::getRandomNum(1000);
    QString cloudPayKey = GM_INSTANCE->m_config->m_baseConfig.cloudPayKey;

    QString var = QString("%1&%2&%3&%4&%5&%6&%7&%8&%9&%10&%11")
                      .arg(exStationId)
                      .arg(laneId)
                      .arg(authCode)
                      .arg(tradeNum)
                      .arg(vehPlate)
                      .arg(factPay)
                      .arg(timeStamp)
                      .arg(enStationId)
                      .arg(enTimeStamp)
                      .arg(randNum)
                      .arg(cloudPayKey);
    QString sigMD5 = Utils::DataDealUtils::cryptoMD5(var);

    QVariantMap sendMap;
    sendMap["stationid"] = exStationId;
    sendMap["laneid"] = QString::number(laneId);
    sendMap["authcode"] = authCode;
    sendMap["tradeid"] = tradeNum;
    sendMap["vehplate"] = vehPlate;
    sendMap["factpay"] = QString::number(factPay);
    sendMap["timestamp"] = timeStamp;
    sendMap["enstationid"] = enStationId;
    sendMap["entime"] = enTimeStamp;
    sendMap["randnum"] = randNum;
    sendMap["signature"] = sigMD5;
    sendMap["shiftdate"] = Utils::DataDealUtils::curDateStr();
    QByteArray sendData = nloJson.serialize(sendMap);
    LOG_INFO().noquote() << "第三方支付请求: " << QString::fromUtf8(sendData);

    QString url(GM_INSTANCE->m_config->m_baseConfig.cloudPayUrl);
    QByteArray result;
    bool ok = Http::instance().postSync(result, url, sendData, "application/json");
    if (!ok) {
        QVariantMap resMap;
        resMap["paystate"] = 1;
        resMap["errorcode"] = 100001;
        resMap["message"] = "支付超时";
        resMap["lanetradeid"] = tradeNum;
        return resMap;
    }

    LOG_INFO().noquote() << "返回第三方支付结果: " << result;
    QVariant resObj = nloJson.parse(result);
    return resObj.toMap();
}

QVariantMap BizHandler::getBillState(const QString &tradeNum)
{
    NloJson nloJson;

    // 获取订单状态
    LOG_INFO().noquote() << "发起核单请求: 订单号 " << tradeNum;

    QUrl url(GM_INSTANCE->m_config->m_baseConfig.billQueryUrl + "/" + tradeNum);
    QByteArray result;
    bool ok = Http::instance().getSync(result, url);
    if (!ok) {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    LOG_INFO().noquote() << "返回核单结果: " << result;
    QVariant resObj = nloJson.parse(result);
    return resObj.toMap();
}

QVariantMap BizHandler::refund(const QString &tradeNum, const QVariantMap &aMap)
{
    NloJson nloJson;

    LOG_INFO().noquote() << "发起退款请求: 订单号 " << tradeNum;

    int refundFee = aMap["paybackFee"].toInt();
    QString operatorId = aMap["operator"].toString();
    QString operatorName = m_ds.getUserName(operatorId, 1);
    QString refundDesc = "";
    int randNum = Utils::DataDealUtils::getRandomNum(1000);
    QString cloudPayKey = GM_INSTANCE->m_config->m_baseConfig.cloudPayKey;

    QString var
        = QString("%1&%2&%3&%4&%5&%6&%7").arg(tradeNum).arg(refundFee).arg(operatorId).arg(operatorName).arg(refundDesc).arg(randNum).arg(cloudPayKey);
    QString sigMd5 = Utils::DataDealUtils::cryptoMD5(var);

    QVariantMap sendMap;
    sendMap["lanetradeid"] = tradeNum;
    sendMap["refundfee"] = refundFee;
    sendMap["operatorid"] = operatorId;
    sendMap["operatorname"] = operatorName;
    sendMap["refunddesc"] = refundDesc;
    sendMap["randnum"] = randNum;
    sendMap["signature"] = sigMd5;
    sendMap["shiftdate"] = aMap["shiftDate"].toString();

    QByteArray sendData = nloJson.serialize(sendMap);
    LOG_INFO().noquote() << "第三方支付请求退款: " << QString::fromUtf8(sendData);

    QString url(GM_INSTANCE->m_config->m_baseConfig.refundUrl);
    QByteArray result;
    bool ok = Http::instance().postSync(result, url, sendData, "application/json");
    if (!ok) {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    LOG_INFO().noquote() << "第三方支付退款结果: " << result;
    QVariant resObj = nloJson.parse(result);
    return resObj.toMap();
}

int BizHandler::getUniqueTradeNum(const QString &stationId)
{
    QString seqNum = m_ds.getEmgcSeqNum(stationId);
    if (seqNum == "0") {
        // 没有站级对应的记录时，默认插入一条，并返回seqNum=1
        m_ds.insertEmgcSeqNum(stationId);
        seqNum = "1";
    } else if (seqNum == "") {
        throw BaseException(1, "响应失败: 获取唯一交易号失败");
    }

    return seqNum.toInt();
}

QString BizHandler::getErrInfo(int errorCode)
{
    QString errInfo;
    if (errorCode == 100001) {
        errInfo = "系统超时";
    } else if (errorCode == 100002) {
        errInfo = "银行系统异常";
    } else if (errorCode == 200001) {
        errInfo = "订单已关闭";
    } else if (errorCode == 200002) {
        errInfo = "订单已撤销";
    } else if (errorCode == 400001) {
        errInfo = "二维码已过期";
    } else if (errorCode == 400002) {
        errInfo = "余额不足";
    } else if (errorCode == 400003) {
        errInfo = "不支持卡类型";
    } else if (errorCode == 400004) {
        errInfo = "用户未输入密码";
    } else if (errorCode == 400005) {
        errInfo = "授权校验码错误";
    } else if (errorCode == -1) {
        errInfo = "其他内部错误";
    }
    return errInfo;
}

QString BizHandler::doDealCmd32(const QVariantMap &aMap)
{
    QString vehicleId;
    if (aMap.contains("vehicleId"))
        vehicleId = aMap["vehicleId"].toString();

    if (vehicleId.isEmpty())
        throw BaseException(1, "响应失败: 车牌号为空");

    NloJson nloJson;

    QVariantMap map;
    map["vehplate"] = vehicleId;
    map["stationid"] = "";
    map["flag"] = 0;
    QByteArray sendData = nloJson.serialize(map);
    LOG_INFO().noquote() << "获取大件运输车信息(多条)请求: " << sendData.constData();

    QUrl url(GM_INSTANCE->m_config->m_baseConfig.multiBulkUrl);
    QByteArray result;
    bool ok = Http::instance().postSync(result, url, sendData, "application/json");
    if (!ok) {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    LOG_INFO().noquote() << "返回大件运输车信息(多条): " << result.left(1024);
    QVariantList bulkInfos = nloJson.parse(result).toList();
    if (bulkInfos.isEmpty())
        throw BaseException(1, "响应失败: 未查询到相关信息");

    QVariantMap tempMap;
    tempMap["bulkInfos"] = bulkInfos;
    tempMap["status"] = 0;
    tempMap["desc"] = "";

    QString dealtData = nloJson.serialize(tempMap);
    return dealtData;
}

QString BizHandler::doDealCmd33(const QVariantMap &aMap)
{
    QString vehicleId;
    if (aMap.contains("vehicleId"))
        vehicleId = aMap["vehicleId"].toString();

    if (vehicleId.isEmpty())
        throw BaseException(1, "响应失败: 车牌号为空");

    NloJson nloJson;

    QVariantMap map;
    map["vehicleid"] = vehicleId;
    map["lanetype"] = 1;
    map["entime"] = Utils::DataDealUtils::curDateTimeStr();
    QByteArray sendData = nloJson.serialize(map);
    LOG_INFO().noquote() << "获取集装箱信息请求: " << QString::fromUtf8(sendData);

    QUrl url(GM_INSTANCE->m_config->m_baseConfig.containerConfirmUrl);
    QByteArray result;
    bool ok = Http::instance().postSync(result, url, sendData, "application/json");
    if (!ok) {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    LOG_INFO().noquote() << "返回集装箱信息: " << result.left(1024);
    // 保存集装箱图片到本地
    QVariantMap infoMap = nloJson.parse(result).toMap();
    int containerConfirm = infoMap["containerconfirm"].toInt();
    if (containerConfirm == 1) {
        for (auto it = infoMap.begin(); it != infoMap.end(); ++it) {
            if (it.value().isNull())
                it.value() = QString("");
        }

        QString startHeadOfTruckPicBase64 = infoMap.value("startheadoftruckpic", "").toString();
        QString startBodyOfTruckPicBase64 = infoMap.value("startbodyoftruckpic1", "").toString();
        QString startTailOfTruckPicBase64 = infoMap.value("starttailoftruckpic", "").toString();
        QString tailTruckPicOfTailBase64 = infoMap.value("tailtruckpicoftail", "").toString();
        QString drivingLicensePicBase64 = infoMap.value("drivinglicensepic", "").toString();
        QString wayBillPicBase64 = infoMap.value("waybillpic", "").toString();
        QString tailTruckDrivingLicensePicBase64 = infoMap.value("tailtruckdrivinglicensepic", "").toString();
        QString tailTruckTransportationTripPicBase64 = infoMap.value("tailtrucktransportationtrippic", "").toString();
        QString businessLicensePicBase64 = infoMap.value("businesslicensepic", "").toString();

        // 保存图片到本地，返回图片Id给请求方
        saveAndReplaceContainerPic(startHeadOfTruckPicBase64, infoMap, "startheadoftruckpic");
        saveAndReplaceContainerPic(startBodyOfTruckPicBase64, infoMap, "startbodyoftruckpic1");
        saveAndReplaceContainerPic(startTailOfTruckPicBase64, infoMap, "starttailoftruckpic");
        saveAndReplaceContainerPic(tailTruckPicOfTailBase64, infoMap, "tailtruckpicoftail");
        saveAndReplaceContainerPic(drivingLicensePicBase64, infoMap, "drivinglicensepic");
        saveAndReplaceContainerPic(wayBillPicBase64, infoMap, "waybillpic");
        saveAndReplaceContainerPic(tailTruckDrivingLicensePicBase64, infoMap, "tailtruckdrivinglicensepic");
        saveAndReplaceContainerPic(tailTruckTransportationTripPicBase64, infoMap, "tailtrucktransportationtrippic");
        saveAndReplaceContainerPic(businessLicensePicBase64, infoMap, "businesslicensepic");
    } else {
        throw BaseException(1, "响应失败: 未查询到预约信息");
    }

    infoMap["status"] = 0;
    infoMap["desc"] = "";
    QString dealtData = nloJson.serialize(infoMap);
    return dealtData;
}

void BizHandler::saveAndReplaceContainerPic(const QString &base64Data, QVariantMap &infoMap, const QString &key)
{
    if (base64Data.isEmpty())
        infoMap[key] = "";
#if QT_VERSION <= QT_VERSION_CHECK(5, 11, 0)
    QString picUUID = QUuid::createUuid().toString();
    picUUID = picUUID.mid(1, picUUID.length() - 2);
#else
    QString picUUID = QUuid::createUuid().toString(QUuid::WithoutBraces);
#endif
    QString picName = QString("/%1.jpg").arg(picUUID);
    QString targetPath = (GM_INSTANCE->m_pictureDir + picName).toString();

    Utils::FileSaver saver(targetPath);
    bool ok = saver.write(QByteArray::fromBase64(base64Data.toUtf8()));
    if (ok) {
        infoMap[key] = picName;
    } else {
        infoMap[key] = "";
    }
    saver.finalize();
}

QString BizHandler::doDealCmd34(const QVariantMap &aMap)
{
    NloJson nloJson;

    QString vehicleId;
    if (aMap.contains("vehicleId"))
        vehicleId = aMap["vehicleId"].toString();

    if (vehicleId.isEmpty())
        throw BaseException(1, "响应失败: 车牌号为空");

    auto tempFreeTempVehicles = m_ds.getFreeTempVehicles(vehicleId);

    if (tempFreeTempVehicles.isEmpty())
        throw BaseException(1, "响应失败: 未查询到相关数据");

    // 从数据库中直接取到的记录，键和数据库中的一致，需要做下转换
    QVariantList freeTempVehicles;
    foreach (auto item, tempFreeTempVehicles) {
        T_FreeTempVehicle freeTempVehicle;
        QJson::QObjectHelper::qvariant2qobject(item.toMap(), &freeTempVehicle);
        QVariantMap map = QJson::QObjectHelper::qobject2qvariant(&freeTempVehicle);
        freeTempVehicles.append(map);
    }

    QVariantMap resMap;
    resMap["freeTempVehicles"] = freeTempVehicles;
    resMap["status"] = 0;
    resMap["desc"] = "";

    QString dealtData = nloJson.serialize(resMap);
    return dealtData;
}

QString BizHandler::doDealCmd37(const QVariantMap &aMap)
{
    NloJson nloJson;

    QString stationID;
    int picType = 0;
    QString picName;
    QString imageBase64;

    if (aMap.contains("stationID"))
        stationID = aMap["stationID"].toString();
    if (aMap.contains("picName"))
        picName = aMap["picName"].toString();
    if (aMap.contains("picType"))
        picType = aMap["picType"].toInt();
    if (aMap.contains("imageBase64"))
        imageBase64 = aMap["imageBase64"].toString();

    if (stationID.isEmpty())
        throw BaseException(1, "响应失败: 站代码为空");
    if (picName.isEmpty())
        throw BaseException(1, "响应失败: 图片名称为空");
    if (picType <= 0)
        throw BaseException(1, "响应失败: 图片类型错误");
    if (imageBase64.isEmpty())
        throw BaseException(1, "响应失败: 图片内容为空");

    QString stationIP = m_ds.getStationIP(stationID);
    QString stationServiceUrl = QString("http://%1:8082").arg(stationIP);

    QString dealtData;
    if (picType == 1) {
        // TODO
    } else if (picType == 2) {
        // TODO
    } else if (picType == 3) {
        // TODO
    } else if (picType == 4) {
        QRegularExpression re(R"(^ImageQ_(.+)$)");
        QRegularExpressionMatch match = re.match(picName);

        QString tradeId;
        if (match.hasMatch()) {
            tradeId = match.captured(1);
        } else {
            throw BaseException(1, "响应失败: 图片命名错误");
        }

        T_DiscardTicketReview discardTicketReview;
        discardTicketReview.StationID = stationID;
        discardTicketReview.TradeID = tradeId;
        discardTicketReview.Flag = 0;
        discardTicketReview.PicData = imageBase64;
        discardTicketReview.OperateTime = Utils::DataDealUtils::curDateTime();

        // 图片上传
        if (m_ds.insertTicketReviewPic(discardTicketReview, stationServiceUrl)) {
            LOG_INFO().noquote() << "图片上传成功";
        }

        QVariantMap resMap;
        resMap["status"] = 0;
        resMap["desc"] = "";
        dealtData = nloJson.serialize(resMap);
    } else {
        // 预留
        throw BaseException(1, "响应失败: 图片类型错误");
    }

    return dealtData;
}

QString BizHandler::doDealCmd38(const QVariantMap &aMap)
{
    NloJson nloJson;

    QString vehPlate;
    QString cardId;
    QString startTime;
    QString stopTime;
    QString stationId;

    if (aMap.contains("vehPlate"))
        vehPlate = aMap["vehPlate"].toString();
    if (aMap.contains("cardId"))
        cardId = aMap["cardId"].toString();
    if (aMap.contains("startTime"))
        startTime = aMap["startTime"].toString();
    if (aMap.contains("stopTime"))
        stopTime = aMap["stopTime"].toString();
    if (aMap.contains("stationId"))
        stationId = aMap["stationId"].toString();

    if (vehPlate.isEmpty() && cardId.isEmpty())
        throw BaseException(1, "响应失败: 站代码与车牌号为空");
    if (startTime.isEmpty() || stopTime.isEmpty())
        throw BaseException(1, "响应失败：开始或结束时间为空");
    if (stationId.isEmpty())
        throw BaseException(1, "响应失败：站代码为空");

    QString stationIP = m_ds.getStationIP(stationId);
    QString stationServiceUrl = QString("http://%1:8082").arg(stationIP);

    QMap<QString, QVariantMap> outTradeList;
    QVariantList tradeRecords = m_ds.getDurationOutTrades(vehPlate, cardId, startTime, stopTime, QUrl(stationServiceUrl));
    for (const auto &item : tradeRecords) {
        QVariantMap oneTrade = item.toMap();
        QString tradeId = oneTrade["tradeid"].toString();

        QVariantMap tradeMap;
        tradeMap["tradeId"] = tradeId;
        tradeMap["passId"] = oneTrade["passid"].toString();
        tradeMap["vehPlate"] = oneTrade["exvehplate"].toString();
        int vehClass = oneTrade["exvehclass"].toInt();
        tradeMap["vehClass"] = Utils::BizUtils::getVehClassName(vehClass);
        tradeMap["cardId"] = oneTrade["cardid"].toString();
        tradeMap["enStation"] = oneTrade["enstationname"].toString();
        tradeMap["exStation"] = oneTrade["exstationname"].toString();
        qreal factPay = oneTrade["factpay"].toDouble();
        tradeMap["factFee"] = QString::number(factPay * 100);
        tradeMap["tradeTime"] = oneTrade["extime"].toString();
        tradeMap["remark"] = "";
        tradeMap["payType"] = oneTrade["paytype"].toInt();
        tradeMap["flag"] = 0;    // 默认不审核，最终是否审核以弃票表中flag字段为准
        tradeMap["isPrint"] = 1; // 默认打票，最终是否打票以业务逻辑判断为准

        bool ok = false;
        tradeMap["ticketNum"] = oneTrade["ticketnum"].toInt(&ok);
        if (!ok)
            tradeMap["ticketNum"] = 0;

        outTradeList.insert(tradeId, tradeMap);
    }

    // 是否审核
    QVariantList discardTicketRecords = m_ds.getDiscardTickets(outTradeList.keys(), QUrl(stationServiceUrl));
    QStringList dtIds; // 后续判断补打票使用
    for (const auto &dtRec : discardTicketRecords) {
        QVariantMap discardTicket = dtRec.toMap();

        QString dtTradeId = discardTicket.value("tradeid").toString();
        int flag = discardTicket.value("flag").toInt();
        dtIds << dtTradeId;
        if (outTradeList.contains(dtTradeId)) {
            QVariantMap tradeMap = outTradeList.value(dtTradeId);
            tradeMap["flag"] = flag;
            // 将修改结果赋值回去
            outTradeList[dtTradeId] = tradeMap;
        }
    }

    // 是否补打票
    QVariantList ticketUseScrapRecords = m_ds.getTicketUseScrapInfos(outTradeList.keys(), QUrl(stationServiceUrl));
    QStringList tusIds;
    for (const auto &tusRec : ticketUseScrapRecords) {
        QVariantMap ticketUseScrap = tusRec.toMap();

        tusIds << ticketUseScrap.value("tradeid").toString();
    }

    for (auto &tradeMap : outTradeList.values()) {
        QString tradeId = tradeMap.value("tradeId").toString();
        if (tusIds.contains(tradeId)) {
            // 检查废票表中有该流水的作废信息
            for (const auto &tusRec : ticketUseScrapRecords) {
                QVariantMap ticketUseScrap = tusRec.toMap();

                QString tusTradeId = ticketUseScrap.value("tradeid").toString();
                if (tusTradeId == tradeId && ticketUseScrap.value("isscrapticket").toInt() == 0) {
                    // 说明该流水已经有补打票了(在mticketuse表中有数据，而scrapticket中查不到对应记录)
                    tradeMap["isPrint"] = 0;
                    break;
                }
            }
        } else {
            // 检查弃票表中有该流水的弃票信息
            if (dtIds.contains(tradeId)) {
                tradeMap["isPrint"] = 1;
            } else {
                int ticketNum = tradeMap["ticketNum"].toInt();
                tradeMap["isPrint"] = ticketNum == 0 ? 1 : 0;
            }
        }
        // 将修改结果赋值回去
        outTradeList[tradeId] = tradeMap;
    }

    QVariantMap resMap;
    resMap["status"] = 0;
    resMap["desc"] = "";
    QVariantList tradeList;
    for (const auto &map : outTradeList.values()) {
        tradeList.append(map);
    }
    resMap["tradeList"] = tradeList;

    QString dealtData = nloJson.serialize(resMap);

    return dealtData;
}

QString BizHandler::doDealCmd39(const QVariantMap &aMap)
{
    NloJson nloJson;

    QString stationID;
    QString startTime;
    QString stopTime;

    if (aMap.contains("stationID"))
        stationID = aMap["stationID"].toString();
    if (aMap.contains("startTime"))
        startTime = aMap["startTime"].toString();
    if (aMap.contains("stopTime"))
        stopTime = aMap["stopTime"].toString();

    if (stationID.isEmpty())
        throw BaseException(1, "响应失败: 站代码为空");
    if (startTime.isEmpty())
        throw BaseException(1, "响应失败: 预约开始时间为空");
    if (stopTime.isEmpty())
        throw BaseException(1, "响应失败: 预约结束时间为空");

    QString stationIP = m_ds.getStationIP(stationID);
    QString stationServiceUrl = QString("http://%1:8082").arg(stationIP);

    QVariantList greenTradeList;
    QVariantList records = m_ds.getExGreenPassTrades(startTime, stopTime, QUrl(stationServiceUrl));
    for (const auto &record : records) {
        QVariantMap recordMap = record.toMap();

        QVariantMap greenTrade;
        greenTrade["tradeId"] = recordMap["tradeid"].toString();
        greenTrade["passId"] = recordMap["passid"].toString();
        greenTrade["vehPlate"] = recordMap["exvehplate"].toString();
        QString enHexNode = recordMap["ennetid"].toString() + recordMap["enstation"].toString();
        greenTrade["enStation"] = m_ds.getGantryNodeID(enHexNode);
        QString exHexNode = recordMap["exnetid"].toString() + recordMap["exstation"].toString();
        greenTrade["exStation"] = m_ds.getGantryNodeID(exHexNode);
        greenTrade["exTime"] = QDateTime::fromString(recordMap["extime"].toString(), "yyyy-MM-dd hh:mm:ss").toString("yyyy-MM-ddThh:mm:ss");
        greenTrade["enWeight"] = recordMap["entotalweight"].toString();
        greenTrade["exWeight"] = recordMap["totalweight"].toString();

        int cardType = recordMap["cardtype"].toInt();
        if (cardType == 22 || cardType == 23) {
            greenTrade["mediaType"] = 1;
        } else if (cardType == 15) {
            greenTrade["mediaType"] = 2;
        } else if (cardType == 3) {
            greenTrade["mediaType"] = 3;
        } else {
            greenTrade["mediaType"] = 9;
        }

        int cardBizType = recordMap["cardbiztype"].toInt();
        if (cardBizType == 33 || cardBizType == 34 || cardBizType == 35) {
            greenTrade["transPayType"] = 2;
        } else if (cardType == 22 || cardType == 23) {
            greenTrade["transPayType"] = 1;
        } else {
            greenTrade["transPayType"] = -1;
        }
        greenTrade["fee"] = recordMap["factpay"].toInt();
        greenTrade["payFee"] = recordMap["shouldpay"].toInt();

        int userType = recordMap["usertype"].toInt();
        QString reserve = recordMap["reserve"].toString();
        if (reserve.contains("(Appointment:0)")) {
            greenTrade["vehicleSign"] = "0xFF";
        } else if (userType == 21) {
            greenTrade["vehicleSign"] = "0x02";
        } else if (userType == 22) {
            greenTrade["vehicleSign"] = "0x03";
        }

        int provinceNum = recordMap["provincenum"].toInt();
        if (provinceNum == 0) {
            provinceNum = 1;
            QRegularExpression re(R"(\(ProvinceNum:(\d+)\))"); // 捕获数字
            QRegularExpressionMatch match = re.match(reserve);
            if (match.hasMatch())
                provinceNum = match.captured(1).toInt(); // 第1个括号里的内容
        }

        greenTrade["proviceCount"] = provinceNum;
        greenTrade["obuPlate"] = recordMap["obuplate"].toString();

        greenTradeList.append(greenTrade);
    }

    QVariantMap map;
    map["status"] = 0;
    map["desc"] = "查询成功";
    map["greenTradeList"] = greenTradeList;
    QString dealtData = nloJson.serialize(map);
    return dealtData;
}

QString BizHandler::doDealCmd40(const QVariantMap &aMap)
{
    int type = 0;
    QString token;
    QString data;
    QString id;

    bool ok = false;
    if (aMap.contains("type"))
        type = aMap["type"].toInt(&ok);
    if (aMap.contains("token"))
        token = aMap["token"].toString();
    if (aMap.contains("data"))
        data = aMap["data"].toString();
    if (aMap.contains("id"))
        id = aMap["id"].toString();

    if (type == 0 || !ok)
        throw BaseException(1, "响应失败: 请求指令类型错误");
    if (!GM_INSTANCE->m_remoteURIs.contains(type))
        throw BaseException(1, QString("响应失败: 未知请求指令类型 %1").arg(type));
    // 所有接口（除授权获取接口11）必须携带授权码
    if (type != 11 && token.isEmpty())
        throw BaseException(1, "响应失败: 授权码为空");
    if (data.isEmpty())
        throw BaseException(1, "响应失败: 请求体为空");

    LOG_INFO().noquote() << "云坐席台账请求: " << data;
    QString retData = requestRemoteAPI(type, id, data, token);
    LOG_INFO().noquote() << "返回云坐席台账处理结果: " << retData.left(1024);

    NloJson nloJson;

    QVariantMap resMap;
    resMap["status"] = 0;
    resMap["desc"] = "指令处理成功";
    if (type == 11) {
        resMap["data"] = retData;
    } else {
        QVariant var = nloJson.parse(retData.toUtf8());
        if (var.isValid() && var.canConvert<QVariantMap>()) {
            resMap["data"] = var.toMap();
        } else if (var.isValid() && var.canConvert<QVariantList>()) {
            resMap["data"] = var.toList();
        }
    }

    QString dealtData = nloJson.serialize(resMap);
    return dealtData;
}

QString BizHandler::requestRemoteAPI(int type, const QString &id, const QString &data, const QString &token)
{
    QString uri = GM_INSTANCE->m_remoteURIs.value(type);
    if (type == 23 || type == 33 || type == 43 || type == 53 || type == 63 || type == 73 || type == 83 || type == 103) {
        if (id.isEmpty())
            throw BaseException(1, "响应失败: 查询ID为空");
        uri = uri.arg(id);
    }

    QString url(GM_INSTANCE->m_config->m_baseConfig.remoteAPIUrl + uri);

    Http httpClient;
    QByteArray result;
    LOG_INFO().noquote() << "测试日志<1>: " << httpClient.getRequestHeaders();
    httpClient.addRequestHeader("UserToken", QString("%1").arg(token).toUtf8());
    LOG_INFO().noquote() << "测试日志<2>: " << httpClient.getRequestHeaders();
    bool ok = httpClient.postSync(result, url, data.toUtf8(), "application/json");
    if (!ok) {
        throw BaseException(1, QString("响应失败: %1").arg(QString(result)));
    }

    return result;
}
