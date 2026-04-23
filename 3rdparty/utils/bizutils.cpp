#include "bizutils.h"
#include "enums.h"
#include "utils/datadealutils.h"

#include <QDate>
#include <QDebug>
#include <QMetaProperty>
#include <QPair>

using namespace Utils;

BizUtils::BizUtils(QObject *parent)
    : QObject{parent}
{}

Utils::BizUtils::~BizUtils() {}

QString BizUtils::getPlateNoColor(const QString &fullPlate)
{
    int pos = -1;
    QString checkData = fullPlate.mid(0, 1);

    if (DataDealUtils::getChineseCountFromString(fullPlate, 4) <= 1) {
        if ((pos = checkData.indexOf("白")) >= 0)
            return fullPlate.mid(pos + 1, fullPlate.length() - pos - 1);
        if ((pos = checkData.indexOf("黑")) >= 0)
            return fullPlate;
    }

    if ((pos = checkData.indexOf("蓝")) < 0 && (pos = checkData.indexOf("黄")) < 0 && (pos = checkData.indexOf("白")) < 0
        && (pos = checkData.indexOf("绿")) < 0 && (pos = checkData.indexOf("拼")) < 0 && (pos = checkData.indexOf("渐")) < 0
        && (pos = checkData.indexOf("黑")) < 0 && (pos = checkData.indexOf("临")) < 0) {
        // 所有都没匹配到
        return fullPlate;
    }

    // 匹配到字符
    return fullPlate.mid(pos + 1, fullPlate.length() - pos - 1);
}

QString BizUtils::correctVehplate(const QString &plate)
{
    if (plate.isEmpty() || plate.contains("无车牌") || plate.contains("整牌拒识"))
        return QString("整牌拒识");

    return plate;
}

int BizUtils::getColorCodeFromPlate(const QString &fullPlate)
{
    if (fullPlate.isEmpty())
        return EM_PlateColor::Unknown;
    if (DataDealUtils::getChineseCountFromString(fullPlate, 4) <= 1) {
        QString prefix = fullPlate.left(2);
        if (prefix.contains(QStringLiteral("白")))
            return EM_PlateColor::White;
        else
            return EM_PlateColor::Unknown;
    }
    QString checkData = fullPlate.mid(0, 1);
    if (checkData.contains(QStringLiteral("蓝")))
        return EM_PlateColor::Blue;
    else if (checkData.contains(QStringLiteral("黄")))
        return EM_PlateColor::Yellow;
    else if (checkData.contains(QStringLiteral("黑")))
        return EM_PlateColor::Black;
    else if (checkData.contains(QStringLiteral("白")))
        return EM_PlateColor::White;
    else if (checkData.contains(QStringLiteral("绿")))
        return EM_PlateColor::Green;
    else if (checkData.contains(QStringLiteral("拼")))
        return EM_PlateColor::Plain;
    else if (checkData.contains(QStringLiteral("渐")))
        return EM_PlateColor::Gradient;
    else if (checkData.contains(QStringLiteral("临")))
        return EM_PlateColor::Temporty;
    else
        return EM_PlateColor::Unknown;
}

QString BizUtils::getColorFormColorCode(int colorCode)
{
    QStringList colors;
    colors << QStringLiteral("蓝") << QStringLiteral("黄") << QStringLiteral("黑") << QStringLiteral("白") << QStringLiteral("绿")
           << QStringLiteral("拼") << QStringLiteral("渐") << QStringLiteral("临");

    if (colorCode >= 8)
        return "";

    return colors.at(colorCode);
}

QString BizUtils::getKeyName(const QVariantMap &keyboard, uint keyCode)
{
    if (keyboard.isEmpty())
        return "";
    QMetaEnum metaEnum = QMetaEnum::fromType<Qt::Key>();
    QString keyStr = metaEnum.valueToKey(keyCode);
    QVariantMap keyInfo = keyboard[keyStr].toMap();
    return keyInfo.value("Name", "").toString();
}

QString BizUtils::getKeyDescByName(const QVariantMap &keyboard, const QString &keyName)
{
    if (keyboard.isEmpty())
        return "";
    for (auto itor = keyboard.constBegin(); itor != keyboard.constEnd(); itor++) {
        if (itor->toMap().value("Name") == keyName)
            return itor->toMap().value("Desc").toString();
    }
    return "";
}

QString BizUtils::getKeyDescByCode(const QVariantMap &keyboard, uint keyCode)
{
    if (keyboard.isEmpty())
        return "";
    QMetaEnum metaEnum = QMetaEnum::fromType<Qt::Key>();
    QString keyStr = metaEnum.valueToKey(keyCode);
    QVariantMap keyInfo = keyboard[keyStr].toMap();
    return keyInfo.value("Desc", "无效按键").toString();
}

int BizUtils::getKeyCode(const QVariantMap &keyboard, const QString &keyName)
{
    if (keyboard.isEmpty())
        return -1;
    QString enumName = keyboard.key(keyName);
    if (enumName.isEmpty())
        return -1;
    QMetaEnum metaEnum = QMetaEnum::fromType<Qt::Key>();
    return metaEnum.keyToValue(enumName.toLocal8Bit().data());
}

QString BizUtils::getVehClassName(uint classCode, bool isShort)
{
    uint var1 = classCode / 10;
    uint var2 = classCode % 10;
    if (var1 >= 3 || var2 >= 7 || var2 <= 0)
        return "车型不明";

    QString vehClassName;
    QStringList strList1;
    QStringList strList2;
    if (isShort) {
        strList1 = QStringList({"客", "货", "专"});
        strList2 = QStringList({"", "一", "二", "三", "四", "五", "六"});

        vehClassName = strList1.value(var1) + strList2.value(var2);
    } else {
        strList1 = QStringList({"客车", "货车", "专项作业车"});
        strList2 = QStringList({"", "一类", "二类", "三类", "四类", "五类", "六类"});

        vehClassName = strList2.value(var2) + strList1.value(var1);
    }

    return vehClassName;
}

QString BizUtils::getVehicleSignString(int UserType, int vehStatus)
{
    QString vehicleSign = "0xff";
    if (UserType == EM_UserType::BULK)
        vehicleSign = "0x00";
    else if (vehStatus == EM_VehStatus::VEH_FRUIT)
        vehicleSign = "0x02";
    else if (vehStatus == EM_VehStatus::VEH_HARVESTER)
        vehicleSign = "0x03";
    else if (UserType == EM_UserType::CONTAINER_J1 || UserType == EM_UserType::CONTAINER_J2)
        vehicleSign = "0x04";
    else if (UserType == 0x05)
        vehicleSign = "0x05";
    else if (UserType == 0x06)
        vehicleSign = "0x06";
    return vehicleSign;
}

int BizUtils::getVehicleSign(int UserType, int vehStatus)
{
    int vehicleSign = 0xff;
    if (UserType == EM_UserType::BULK)
        vehicleSign = 0x00;
    else if (vehStatus == EM_VehStatus::VEH_FRUIT)
        vehicleSign = 0x02;
    else if (vehStatus == EM_VehStatus::VEH_HARVESTER)
        vehicleSign = 0x03;
    else if (UserType == EM_UserType::CONTAINER_J1 || UserType == EM_UserType::CONTAINER_J2)
        vehicleSign = 0x04;
    else if (UserType == 0x05)
        vehicleSign = 0x05;
    else if (UserType == 0x06)
        vehicleSign = 0x06;
    return vehicleSign;
}

int BizUtils::getStrToFullStr(QString FullStr, QString str)
{
    if (FullStr.indexOf(str) > 0)
        return 100;
    int fullLen = FullStr.toStdString().length();
    int nLen = str.toStdString().length();
    int nTemp = 0, nMaxTemp = 0;
    char full[20] = {0};
    char subStr[20] = {0};
    unsigned char cmp1 = 0, cmp2 = 0;
    if (fullLen < 20)
        memcpy(full, FullStr.toStdString().c_str(), fullLen);
    if (nLen < 20)
        memcpy(subStr, str.toStdString().c_str(), nLen);
    for (int n = 0; n <= (fullLen - nLen); n++) {
        nTemp = 0;
        for (int i = 0; i < nLen; i++) {
            cmp1 = full[i + n];
            cmp2 = subStr[i];
            if (cmp1 == cmp2)
                nTemp++;
        }
        if (nTemp > nMaxTemp)
            nMaxTemp = nTemp;
    }
    return (nMaxTemp * 100.00 / nLen);
}

QString BizUtils::getVehModeName(int vehMode)
{
    if (vehMode == EM_VehMode::CAR) {
        return "客车流程";
    } else if (vehMode == EM_VehMode::TRUCK) {
        return "货车流程";
    } else if (vehMode == EM_VehMode::OPERATION) {
        return "专项作业车流程";
    } else if (vehMode == EM_VehMode::CAR_LOCK) {
        return "客车锁定";
    } else if (vehMode == EM_VehMode::TRUCK_LOCK) {
        return "货车锁定";
    } else {
        return "";
    }
}

int BizUtils::getVehModeClass(int vehMode)
{
    if (vehMode == EM_VehMode::TRUCK || vehMode == EM_VehMode::TRUCK_LOCK) {
        return 10;
    } else if (vehMode == EM_VehMode::OPERATION) {
        return 20;
    } else {
        return 0;
    }
}

QString BizUtils::getCardDesc(int cardType)
{
    if (cardType == EM_CardType::CPC)
        return "CPC卡";
    if (cardType == EM_CardType::PREPAID)
        return "储值卡";
    if (cardType == EM_CardType::CREDIT)
        return "记账卡";
    if (cardType == EM_CardType::PAPER)
        return "纸性券";
    return "";
}
QString BizUtils::makeDtpContentFromObj(const QObject &obj)
{
    QString content = "<record>";
    // 遍历 QObject 的属性，拼 content
    const QMetaObject *mo = obj.metaObject();
    int propCount = mo->propertyCount();
    for (int i = 0; i < propCount; ++i) {
        QMetaProperty prop = mo->property(i);
        const char *cname = prop.name();
        QString name = QString::fromLatin1(cname).toLower();

        // 跳过 Qt 自带的 objectName 属性
        if (name == QLatin1String("objectname") || name == QLatin1String("tbl_pk") || name == QLatin1String("auto_inc"))
            continue;
        // 只要可读就取
        if (!prop.isReadable())
            continue;
        QVariant v = obj.property(cname);

        QString sValue;
        // 按类型格式化
        if (v.type() == QVariant::Date) {
            sValue = v.toDate().toString("yyyy-MM-dd");
        } else if (v.type() == QVariant::DateTime) {
            sValue = v.toDateTime().toString("yyyy-MM-dd HH:mm:ss");
        } else {
            sValue = v.toString();
        }
        // 对日期/时间类型加花括号
        if (v.type() == QVariant::Date || v.type() == QVariant::DateTime) {
            sValue = "{" + sValue + "}";
        }

        // 拼成 <name>value</name>
        content += "<" + name + ">" + sValue + "</" + name + ">";
    }
    content += "</record>";

    return content;
}

QString BizUtils::makeDtpContentFromStr(const QStringList &strs)
{
    QString content;

    foreach (auto str, strs) {
        QString record = "<record>";
        QString val = QString("<![CDATA[%1]]>").arg(str);
        record += val;
        record += "</record>";

        content += record;
    }

    return content;
}

QString BizUtils::makeDtpContentFromMap(const QVariantMap &map)
{
    QString content;

    for (auto it = map.begin(); it != map.end(); ++it) {
        const QString &field = it.key();
        QString value = it.value().toString();

        // 空值
        if (value.isEmpty()) {
            content += QString("<%1/>").arg(field);
        } else {
            content += QString("<%1>%2</%1>").arg(field, value);
        }
    }

    return QString("<record>%1</record>").arg(content);
}

QString BizUtils::makeDtpXml(const QString &content, const QString &businessId, const QString &fromNode, const QString &toNode, int recordCount)
{
    QString result;

    result += QString(R"(<?xml version="1.0" encoding="gbk" ?><transfer><head>)");
    result += "<businessid>" + businessId + "</businessid>";
    result += "<fromnode>" + fromNode + "</fromnode>";
    result += "<tonode>" + toNode + "</tonode>";
    result += "<recordcount>" + QString::number(recordCount) + "</recordcount>";
    result += "</head><content>";
    result += content;
    result += "</content></transfer>";

    return result;
}

//RSU特情告警描述
QString BizUtils::getWarnDesc(int nWarnType)
{
    QString sDesc = "";
    switch (nWarnType) {
    case EM_WarnType::WARN_BALANCE_LOW:
        sDesc = "卡余额不足";
        break;
    case EM_WarnType::WARN_OBU_TAMPER_FAIL:
        sDesc = "OBU防拆失效";
        break;
    case EM_WarnType::WARN_OBU_DISABLED:
        sDesc = "OBU禁用";
        break;
    case EM_WarnType::WARN_IC_DISABLED:
        sDesc = "IC卡禁用";
        break;
    case EM_WarnType::WARN_OBU_EXPIRED:
        sDesc = "OBU已过期";
        break;
    case EM_WarnType::WARN_IC_EXPIRED:
        sDesc = "卡已过期";
        break;
    case EM_WarnType::WARN_OBU_NOT_ENABLED:
        sDesc = "电子标签未启用";
        break;
    case EM_WarnType::WARN_IC_NOT_ENABLED:
        sDesc = "ETC卡未启用";
        break;
    case EM_WarnType::WARN_TIMEOUT:
        sDesc = "超时车";
        break;
    case EM_WarnType::WARN_BREAKTHROUGH:
        sDesc = "冲关车";
        break;
    case EM_WarnType::WARN_NO_ENTRY_INFO:
        sDesc = "无入口信息";
        break;
    case EM_WarnType::WARN_NO_CARD:
        sDesc = "标签无卡";
        break;
    case EM_WarnType::WARN_IC_INVALID_ISSUER:
        sDesc = "卡发行方无效";
        break;
    case EM_WarnType::WARN_OBU_INVALID_ISSUER:
        sDesc = "OBU发行方无效";
        break;
    case EM_WarnType::WARN_NO_FEE:
        sDesc = "入口不明";
        break;
    case EM_WarnType::WARN_FEE_ERR:
        sDesc = "费率计算错误";
        break;
    case EM_WarnType::WARN_IC_REPORTED_LOST:
        sDesc = "IC卡挂失";
        break;
    case EM_WarnType::WARN_OBU_REPORTED_LOST:
        sDesc = "OBU挂失";
        break;
    case EM_WarnType::WARN_LANE_INTERFERENCE:
        sDesc = "邻道干扰";
        break;
    case EM_WarnType::WARN_WRITE_CARD_FAIL:
        sDesc = "写卡故障";
        break;
    case EM_WarnType::WARN_DEVICE_FAULT:
        sDesc = "设备故障";
        break;
    case EM_WarnType::WARN_BLACKLIST_VEHICLE:
        sDesc = "欠费车辆";
        break;
    case EM_WarnType::WARN_BLACKLIST_CARD:
        sDesc = "ETC卡账户异常";
        break;
    case EM_WarnType::WARN_BLACKLIST_TAG:
        sDesc = "OBU状态名单";
        break;
    case EM_WarnType::WARN_NO_OBU:
        sDesc = "无电子标签";
        break;
    case EM_WarnType::WARN_U_TURN:
        sDesc = "U转车";
        break;
    case EM_WarnType::WARN_ENTRY_EXIT_FLAG_ERROR:
        sDesc = "进出站标志错";
        break;
    case EM_WarnType::WARN_UNKNOWN_VEHCLASS:
        sDesc = "车型不明";
        break;
    case EM_WarnType::WARN_OBU_CARD_NOTSAME:
        sDesc = "标卡信息不符";
        break;
    case EM_WarnType::WARN_PLATE_MISMATCH:
        sDesc = "车牌不符车";
        break;
    case EM_WarnType::WARN_FREE_VEHICLE_EXPIRED:
        sDesc = "免费车到期";
        break;
    case EM_WarnType::WARN_FREE_CARD_EXPIRED:
        sDesc = "免费卡到期";
        break;
    case EM_WarnType::WARN_PROHIBITED_VEHICLE:
        sDesc = "禁止通行车辆";
        break;
    case EM_WarnType::WARN_OBU_DEVICE_FAULT:
        sDesc = "OBU设备故障";
        break;
    case EM_WarnType::WARN_OBU_ANTENNA_FAIL:
        sDesc = "通讯故障";
        break;
    case EM_WarnType::WARN_OTHER:
        sDesc = "其它";
        break;
    case EM_WarnType::WARN_B5_FRAME_ERROR:
        sDesc = "B5帧错误";
        break;
    case EM_WarnType::WARN_LOCATION_ILLEGAL:
        sDesc = "卡标归属地不一致";
        break;
    case EM_WarnType::WARN_ETC_CARD_INVALID:
        sDesc = "ETC卡无效";
        break;
    case EM_WarnType::WARN_TRADE_ERROR:
        sDesc = "交易暂时失败";
        break;
    case EM_WarnType::WARN_FREE_TIMEOUT:
        sDesc = "超出免征期限";
        break;
    case EM_WarnType::WARN_FREE_SCOPE:
        sDesc = "超出免费区域";
        break;
    case EM_WarnType::WARN_CMD_ERROR:
        sDesc = "RSU指令帧错";
        break;
    case EM_WarnType::WARN_SYS_ERROR:
        sDesc = "系统错";
        break;
    case EM_WarnType::WARN_NETID_EREROR:
        sDesc = "入网编号不明";
        break;
    case EM_WarnType::WARN_LAW_BALCK:
        sDesc = "路政黑名单";
        break;
    case EM_WarnType::WARN_LAW_TRAILER:
        sDesc = "牵引拖挂车";
        break;
    case EM_WarnType::WARN_LAW_WEIGHT:
        sDesc = "无称重数据";
        break;
    case EM_WarnType::WARN_STOP_WEIGHT:
        sDesc = "超限称重数据";
        break;
    case EM_WarnType::WARN_USERTYPE_LTC:
        sDesc = "绿通车";
        break;
    case EM_WarnType::WARN_USERTYPE_LHSGJ:
        sDesc = "联合收割机";
        break;
    case EM_WarnType::WARN_LAW_JZX:
        sDesc = "集装箱J1";
        break;
    case EM_WarnType::WARN_LAW_JZX2:
        sDesc = "集装箱J2";
        break;
    case EM_WarnType::WARN_OBU_FEE:
        sDesc = "OBU计费信息异常";
        break;
    case EM_WarnType::WARN_OBU_EN:
        sDesc = "入口信息异常";
        break;
    case EM_WarnType::WARN_OBU_PROV:
        sDesc = "通行省份个数异常";
        break;
    case EM_WarnType::WARN_OBU_FEE0:
        sDesc = "OBU计费信息异常";
        break;
    case EM_WarnType::WARN_ERR_FEE:
        sDesc = "实收金额超大";
        break;
    case EM_WarnType::WARN_ERR_WEIGHT:
        sDesc = "入口称重信息异常";
        break;
    case EM_WarnType::WARN_ERR_AXIS:
        sDesc = "超6轴";
        break;
    case EM_WarnType::WARN_ERR_KSJZX:
        sDesc = "跨省集装箱";
        break;
    case EM_WarnType::WARN_ERR_RETRADE:
        sDesc = "重复交易";
        break;
    case EM_WarnType::WARN_ERR_NOAXIS:
        sDesc = "罕见轴型";
        break;
    case EM_WarnType::WARN_ERR_NOVEHPLATE:
        sDesc = "与抓拍车牌不符";
        break;
    case EM_WarnType::WARN_AXLE_MODEL_MISMATCH:
        sDesc = "车轴型不符";
        break;
    case EM_WarnType::WARN_OVERSIZED_VEHICLE:
        sDesc = "大件运输车";
        break;
    default:
        break;
    }
    return sDesc;
}

QString BizUtils::getVehPeccTypeDesc(int nVehPeccType)
{
    QString sDesc = "";
    switch (nVehPeccType) {
    case EM_VehPeccType::NORMAL:
        sDesc = "正常";
        break;
    case EM_VehPeccType::BREAKTHROUGH:
        sDesc = "冲关";
        break;
    case EM_VehPeccType::UNPAID:
        sDesc = "未付";
        break;
    case EM_VehPeccType::EVADE_FEE:
        sDesc = "逃费";
        break;
    case EM_VehPeccType::SPECIAL_SEARCH:
        sDesc = "特殊情况配合查找";
        break;
    case EM_VehPeccType::LAW_BLACKLIST:
        sDesc = "路政状态名单车";
        break;
    case EM_VehPeccType::MALICIOUS_OWE:
        sDesc = "电子收费恶意欠费车辆";
        break;
    case EM_VehPeccType::ETC_ARREARS_LIST:
        sDesc = "ETC欠费状态名单";
        break;
    case EM_VehPeccType::BANK_ADVANCE_LIST:
        sDesc = "银行垫付状态名单";
        break;
    default:
        break;
    }
    return sDesc;
}

// 卡类型
QString BizUtils::getCardTypeDesc(int nCardType)
{
    QString sDesc = "";
    switch (nCardType) {
    case EM_CardType::NONE:
        sDesc = "空白卡或未知";
        break;
    case EM_CardType::PAPER:
        sDesc = "纸性券";
        break;
    case EM_CardType::CPC:
        sDesc = "CPC";
        break;
    case EM_CardType::OBU:
        sDesc = "单片式OBU（暂未用过）";
        break;
    case EM_CardType::PREPAID:
        sDesc = "储值卡";
        break;
    case EM_CardType::CREDIT:
        sDesc = "记账卡";
        break;
    default:
        break;
    }
    return sDesc;
}

// 卡业务类型
QString BizUtils::getCardBizTypeDesc(int nCardBizType)
{
    QString sDesc = "";
    switch (nCardBizType) {
    case EM_CardBizType::MANUAL:
        sDesc = "人工发卡模式";
        break;
    case EM_CardBizType::ROBOT:
        sDesc = "自助发卡机";
        break;
    case EM_CardBizType::OPENING:
        sDesc = "开放式车道";
        break;
    case EM_CardBizType::HOLIDAY:
        sDesc = "节假日免征直行";
        break;
    case EM_CardBizType::ECNY:
        sDesc = "数字人民币";
        break;
    case EM_CardBizType::WECHAT:
        sDesc = "微信支付";
        break;
    case EM_CardBizType::ALIPAY:
        sDesc = "支付宝支付";
        break;
    case EM_CardBizType::CPC_ETC_PAY:
        sDesc = "CPC卡使用ETC卡支付";
        break;
    case EM_CardBizType::PAPER_ETC_PAY:
        sDesc = "纸券使用ETC卡支付";
        break;
    case EM_CardBizType::NO_CARD_ETC_PAY:
        sDesc = "无卡使用ETC卡支付";
        break;
    case EM_CardBizType::ERR_ENEX_FLAG:
        sDesc = "入出口站标志错误";
        break;
    case EM_CardBizType::ENTRY_UNKOWN:
        sDesc = "入口不明";
        break;
    default:
        break;
    }
    return sDesc;
}

// 支付方式
QString BizUtils::getPayTypeDesc(int nPayType)
{
    QString sDesc = "";
    switch (nPayType) {
    case EM_PayType::CASH:
        sDesc = "现金支付";
        break;
    case EM_PayType::ETC_CARD:
        sDesc = "电子支付";
        break;
    case EM_PayType::SCAN:
        sDesc = "第三方支付";
        break;
    case EM_PayType::UNPAID:
        sDesc = "闯关未付";
        break;
    default:
        break;
    }
    return sDesc;
}

// 黑名单类型
QString BizUtils::getBlackTypeDesc(int nBlackType)
{
    QString sDesc = "";
    switch (nBlackType) {
    case EM_BlackType::VEHICLE:
        sDesc = "状态名单车";
        break;
    case EM_BlackType::CPC_CARD:
        sDesc = "状态名单通行卡";
        break;
    case EM_BlackType::ID_CARD:
        sDesc = "状态名单身份卡";
        break;
    case EM_BlackType::OBU:
        sDesc = "状态名单OBU";
        break;
    case EM_BlackType::ETC_CARD:
        sDesc = "状态名单ETC卡";
        break;
    case EM_BlackType::TERMINAL:
        sDesc = "状态名单终端";
        break;
    case EM_BlackType::LAW_VEHICLE:
        sDesc = "路政状态名单车";
        break;
    default:
        break;
    }
    return sDesc;
}

// 变更类型
QString BizUtils::getChangeTypeDesc(int nChangeType)
{
    QString sDesc = "";
    switch (nChangeType) {
    case EM_ChangeType::NONE:
        sDesc = "无";
        break;
    case EM_ChangeType::CAR_TO_TRUCK:
        sDesc = "客车变货车";
        break;
    case EM_ChangeType::TRUCK_TO_CARD:
        sDesc = "货车变客车";
        break;
    case EM_ChangeType::HIGH_TO_LOW:
        sDesc = "高变低";
        break;
    case EM_ChangeType::LOW_TO_HIGH:
        sDesc = "低变高";
        break;
    case EM_ChangeType::CAR_TO_OPERATION:
        sDesc = "客变专";
        break;
    case EM_ChangeType::OPERATION_TO_CAR:
        sDesc = "专变客";
        break;
    case EM_ChangeType::TRUCK_TO_OPERATION:
        sDesc = "货变专";
        break;
    case EM_ChangeType::OPERATION_TO_TRUCK:
        sDesc = "专变货";
        break;
    default:
        break;
    }
    return sDesc;
}

// 标识站
QString BizUtils::getChkPointDesc(int nChkPoint)
{
    QString sDesc = "";
    switch (nChkPoint) {
    case EM_ChkPoint::QINGZHOU_BRIDGE:
        sDesc = "青州大桥";
        break;
    case EM_ChkPoint::XIAXIANG_BRIDGE:
        sDesc = "厦漳跨海大桥";
        break;
    case EM_ChkPoint::PINGTAN_BRIDGE:
        sDesc = "平潭大桥";
        break;
    case EM_ChkPoint::QUANZHOUWAN_BRIDGE:
        sDesc = "泉州湾跨海大桥";
        break;
    default:
        break;
    }
    return sDesc;
}

// 出口车情
QString BizUtils::getDealStatusDesc(int nDealStatus, QString delimiter)
{
    QString sDesc = "";
    // 定义所有状态与描述的映射（包含全部枚举值）
    using StatusPair = QPair<EM_DealStatus::DealStatus, QString>;
    static const QList<StatusPair> statusList = {qMakePair(EM_DealStatus::NO_CARD, QString("无卡")),
                                                 qMakePair(EM_DealStatus::BAD_CARD, QString("坏卡")),
                                                 qMakePair(EM_DealStatus::PLATE_DIFF, QString("车牌不符")),
                                                 qMakePair(EM_DealStatus::U_TURN, QString("U转")),
                                                 qMakePair(EM_DealStatus::TIMEOUT, QString("超时")),
                                                 qMakePair(EM_DealStatus::CHANGED, QString("变更")),
                                                 qMakePair(EM_DealStatus::UNPAID, QString("未付")),
                                                 qMakePair(EM_DealStatus::NO_PLATE_DIFF, QString("非车牌不符")),
                                                 qMakePair(EM_DealStatus::U_TURN_NORMAL, QString("正常U转车")),
                                                 qMakePair(EM_DealStatus::TIMEOUT_NORMAL, QString("超时车")),
                                                 qMakePair(EM_DealStatus::U_TURN_CAL, QString("正常计费U转")),
                                                 qMakePair(EM_DealStatus::GUIAN_CAR, QString("贵安专用车")),
                                                 qMakePair(EM_DealStatus::SCAN_PAY, QString("二维码支付")),
                                                 qMakePair(EM_DealStatus::BINHAI_REBATE, QString("滨海新城代扣")),
                                                 qMakePair(EM_DealStatus::MAWEI_REBATE, QString("马尾大桥代扣")),
                                                 qMakePair(EM_DealStatus::WUYISHAN_REBATE, QString("武夷山代扣")),
                                                 qMakePair(EM_DealStatus::SANMING_SHAXIAN_REBATE, QString("三明沙县代扣")),
                                                 qMakePair(EM_DealStatus::JINJIANGNAN_LONGHU_REBATE, QString("晋江南龙湖代扣")),
                                                 qMakePair(EM_DealStatus::NANPING_JIANYANG_REBATE, QString("南平建阳代扣")),
                                                 qMakePair(EM_DealStatus::WUPINGSHIFANG_REBATE, QString("武平十方代扣")),
                                                 qMakePair(EM_DealStatus::SHAOWU_WUJIATANG_REBATE, QString("邵武吴家塘代扣")),
                                                 qMakePair(EM_DealStatus::GUANGZE_JINLING_REBATE, QString("光泽金岭代扣")),
                                                 qMakePair(EM_DealStatus::PUCHENG_REBATE, QString("浦城代扣")),
                                                 qMakePair(EM_DealStatus::YANPING_REBATE, QString("延平代扣")),
                                                 qMakePair(EM_DealStatus::YANPINGBEI_REBATE, QString("延平北代扣"))};

    // 遍历所有状态，通过位与判断是否包含该状态
    for (const auto &pair : statusList) {
        // 将枚举值转为int进行位与运算（假设枚举是位掩码定义）
        if (nDealStatus & static_cast<int>(pair.first)) {
            if (!sDesc.isEmpty()) {
                sDesc += delimiter; // 多状态用逗号分隔
            }
            sDesc += pair.second;
        }
    }

    return sDesc;
}

// 计费方式
QString BizUtils::getExitFeeTypeDesc(int nExitFeeType)
{
    QString sDesc = "";
    switch (nExitFeeType) {
    case EM_ExitFeeType::OBU_NO_DISCOUNT:
        sDesc = "按OBU内累计优惠后金额计费";
        break;
    case EM_ExitFeeType::OBU_BEFORE_DISCOUNT:
        sDesc = "按OBU内累计优惠前金额计费";
        break;
    case EM_ExitFeeType::CPC_CARD:
        sDesc = "按CPC卡内累计金额计费";
        break;
    case EM_ExitFeeType::PROVINCE_CLOUD_FEE:
        sDesc = "省中心在线服务计费";
        break;
    case EM_ExitFeeType::MOT_CLOUD_FEE:
        sDesc = "部中心在线服务计费";
        break;
    case EM_ExitFeeType::SHORT:
        sDesc = "按最小费额计费";
        break;
    default:
        break;
    }
    return sDesc;
}

// 免费类型
QString BizUtils::getFreeTypeDesc(int nFreeType)
{
    QString sDesc = "";
    switch (nFreeType) {
    case EM_FreeType::PROVINCE_FREE:
        sDesc = "全省免征";
        break;
    case EM_FreeType::REGION_FREE:
        sDesc = "区域免征";
        break;
    default:
        break;
    }
    return sDesc;
}

// 参数类型
QString BizUtils::getParamTypeDesc(int nParamType)
{
    QString sDesc = "";
    switch (nParamType) {
    case EM_ParamType::FEE_RATE_PARAM:
        sDesc = "费率参数";
        break;
    case EM_ParamType::AXLE_TYPE_PARAM:
        sDesc = "轴型参数";
        break;
    case EM_ParamType::VEHICLE_SEAT_PARAM:
        sDesc = "车型坐位参数";
        break;
    case EM_ParamType::WEIGHT_CORRECTION_PARAM:
        sDesc = "计重较正参数";
        break;
    case EM_ParamType::SHIFT_PARAM:
        sDesc = "班次参数";
        break;
    case EM_ParamType::TRAFFIC_CALIBRATION_PARAM:
        sDesc = "交调口径折算系数";
        break;
    case EM_ParamType::HOLIDAY_PARAM:
        sDesc = "节假日免征参数";
        break;
    case EM_ParamType::DISCOUNT_USER_PARAM:
        sDesc = "折扣用户参数";
        break;
    case EM_ParamType::DISCOUNT_FEE_PARAM:
        sDesc = "DiscountFee折扣参数";
        break;
    case EM_ParamType::PARAM_DICT_PARAM:
        sDesc = "T_ParamDict参数";
        break;
    case EM_ParamType::SEGMENT_FEE_PARAM:
        sDesc = "分段计费参数";
        break;
    case EM_ParamType::REDUCE_EXEMPT_PARAM:
        sDesc = "减免征车参数";
        break;
    case EM_ParamType::LAW_TRAILER_PARAM:
        sDesc = "路政拖车参数";
        break;
    case EM_ParamType::ETC_BLACKLIST_PARAM:
        sDesc = "ETC状态名单参数";
        break;
    case EM_ParamType::VEHICLE_BLACKLIST_PARAM:
        sDesc = "状态名单车参数";
        break;
    case EM_ParamType::PASSCARD_BLACKLIST_PARAM:
        sDesc = "状态名单通行卡参数";
        break;
    case EM_ParamType::LAW_BLACKLIST_PARAM:
        sDesc = "路政状态名单参数";
        break;
    case EM_ParamType::OTHER_BLACKLIST_PARAM:
        sDesc = "其它状态名单";
        break;
    case EM_ParamType::OTHER_FEE_PARAM:
        sDesc = "其它收费参数";
        break;
    default:
        break;
    }
    return sDesc;
}

// 代扣类型
QString BizUtils::getRebateTypeDesc(int nRebateType)
{
    QString sDesc = "";
    switch (nRebateType) {
    case EM_RebateType::GUIAN_REBATE:
        sDesc = "贵安商学院代扣";
        break;
    case EM_RebateType::BINHAI_REBATE:
        sDesc = "滨海新城代扣";
        break;
    case EM_RebateType::MAWEI_REBATE:
        sDesc = "马尾大桥(福州城区)代扣";
        break;
    case EM_RebateType::SHAOWU_JINTANG_REBATE:
        sDesc = "邵武金塘代扣";
        break;
    case EM_RebateType::YANPING_LIST_REBATE:
        sDesc = "延平代扣(名单)";
        break;
    case EM_RebateType::WUYISHAN_REBATE:
        sDesc = "武夷山代扣";
        break;
    case EM_RebateType::SANMING_SHAXIAN_REBATE:
        sDesc = "三明沙县代扣";
        break;
    case EM_RebateType::JINJIANGNAN_LONGHU_REBATE:
        sDesc = "晋江南龙湖代扣";
        break;
    case EM_RebateType::NANPING_JIANYANG_REBATE:
        sDesc = "南平建阳代扣";
        break;
    case EM_RebateType::JIANYANG_NANPINGNAN_REBATE:
        sDesc = "建阳南平南代扣";
        break;
    case EM_RebateType::WUPING_DUANWU_2024_REBATE:
        sDesc = "武平十方代扣2024端午";
        break;
    case EM_RebateType::WUPING_MID_AUTUMN_2024_REBATE:
        sDesc = "武平十方代扣2024中秋";
        break;
    case EM_RebateType::WUPING_NATIONAL_2024_REBATE:
        sDesc = "武平十方代扣2024国庆";
        break;
    case EM_RebateType::WUPING_NEWYEAR_2025_REBATE:
        sDesc = "武平十方代扣2025元旦";
        break;
    case EM_RebateType::WUPING_SPRING_2025_REBATE:
        sDesc = "武平十方代扣2025春运";
        break;
    case EM_RebateType::WUPING_QINGMING_2025_REBATE:
        sDesc = "武平十方代扣2025清明";
        break;
    case EM_RebateType::WUPING_LABOR_2025_REBATE:
        sDesc = "武平十方代扣2025劳动";
        break;
    case EM_RebateType::WUPING_DUANWU_2025_REBATE:
        sDesc = "武平十方代扣2025端午";
        break;
    case EM_RebateType::WUPING_NATIONAL_2025_REBATE:
        sDesc = "武平十方代扣2025国庆";
        break;
    case EM_RebateType::GUANGZE_JINLING_REBATE:
        sDesc = "光泽金岭代扣";
        break;
    case EM_RebateType::PUCHENG_REBATE:
        sDesc = "浦城代扣";
        break;
    case EM_RebateType::YANPING_REBATE:
        sDesc = "延平代扣";
        break;
    case EM_RebateType::YANPINGBEI_REBATE:
        sDesc = "延平北代扣";
        break;
    case EM_RebateType::FUZHOUDONG_JICHANG_REBATE:
        sDesc = "机场代扣(福州东-机场)";
        break;
    case EM_RebateType::MAWEI_JICHANG_REBATE:
        sDesc = "机场代扣(马尾-机场)";
        break;
    default:
        break;
    }
    return sDesc;
}

// 记录类型
QString BizUtils::getRecordTypeDesc(int nRecordType)
{
    QString sDesc = "";
    switch (nRecordType) {
    case EM_RecordType::WEIGHT_LOW:
        sDesc = "称重降级数据";
        break;
    case EM_RecordType::MANUAL_WEIGHT:
        sDesc = "手工插入称重";
        break;
    case EM_RecordType::CHANGE_AXIS_TYPE:
        sDesc = "人工改轴型";
        break;
    case EM_RecordType::CHANGE_AXISES:
        sDesc = "人工改轴数";
        break;
    case EM_RecordType::CHANGE_WEIGHT:
        sDesc = "轴型+轴数更改";
        break;
    case EM_RecordType::TRAILER:
        sDesc = "拖车";
        break;
    case EM_RecordType::ACCIDENT:
        sDesc = "事故车";
        break;
    case EM_RecordType::LONG_CAR:
        sDesc = "超长车";
        break;
    case EM_RecordType::FAKE_AXIS:
        sDesc = "假轴车";
        break;
    case EM_RecordType::NO_ENOUGH_MONEY:
        sDesc = "ETC卡余额不足";
        break;
    case EM_RecordType::GREEN_TO_NORMAL:
        sDesc = "绿通变普通";
        break;
    case EM_RecordType::ENTRY_ERROR:
        sDesc = "入口信息校验出错";
        break;
    case EM_RecordType::SPT:
        sDesc = "自助缴费终端";
        break;
    case EM_RecordType::CROSS_PROVINCE:
        sDesc = "跨省";
        break;
    case EM_RecordType::REMOTE_ASSIST:
        sDesc = "远程辅助";
        break;
    case EM_RecordType::ENTRY_EXIT_FLAG_ERROR:
        sDesc = "入出口异常";
        break;
    default:
        break;
    }
    return sDesc;
}

// 记录类型2
QString BizUtils::getRecordType2Desc(int nRecordType2)
{
    QString sDesc = "";
    switch (nRecordType2) {
    case EM_RecordType2::CPC_CARD_CAL:
        sDesc = "根据CPC卡内数据计费";
        break;
    case EM_RecordType2::SHORT_CAL:
        sDesc = "最短可达路径计费";
        break;
    case EM_RecordType2::SHORT_ERROR_CAL:
        sDesc = "路径不可达（省内最远站点计费）";
        break;
    case EM_RecordType2::BLACK:
        sDesc = "状态名单车或状态名单卡";
        break;
    case EM_RecordType2::NO_DISCOUNT_ETC:
        sDesc = "单ETC卡无折扣";
        break;
    case EM_RecordType2::FORBIDDEN_PASSENGER:
        sDesc = "不允许通行的两客一危车辆";
        break;
    case EM_RecordType2::PLATE_DIFFER:
        sDesc = "卡车牌与抓拍车牌不符";
        break;
    case EM_RecordType2::ETC_CAL_ERROR:
        sDesc = "出口ETC累计金额异常";
        break;
    case EM_RecordType2::OBU_CARD_DIFF:
        sDesc = "卡签入口信息不符";
        break;
    case EM_RecordType2::TEST_VEHICLE:
        sDesc = "测试车";
        break;
    case EM_RecordType2::SHORT_80_PERCENT:
        sDesc = "兜底80%";
        break;
    default:
        break;
    }
    return sDesc;
}

// 作废类型
QString BizUtils::getScrapTypeDesc(int nScrapType)
{
    QString sDesc = "";
    switch (nScrapType) {
    case EM_ScrapType::NORMAL_TICKET:
        sDesc = "正常票";
        break;
    case EM_ScrapType::REPRINT_TICKET:
        sDesc = "重打票";
        break;
    case EM_ScrapType::ADJUSTED_TICKET:
        sDesc = "调整票";
        break;
    case EM_ScrapType::TRADE_SCRAP:
        sDesc = "交易废票";
        break;
    case EM_ScrapType::NON_TRADE_SCRAP:
        sDesc = "非交易废票";
        break;
    case EM_ScrapType::BACKEND_TRADE_SCRAP:
        sDesc = "后台交易废票";
        break;
    default:
        break;
    }
    return sDesc;
}

// 出口班次日期（班次类型）
QString BizUtils::getShiftDateTypeDesc(int nShiftDateType)
{
    QString sDesc = "";
    switch (nShiftDateType) {
    case EM_ShiftDateType::MORNING:
        sDesc = "早班";
        break;
    case EM_ShiftDateType::AFTERNOON:
        sDesc = "中班";
        break;
    case EM_ShiftDateType::NIGHT:
        sDesc = "晚班";
        break;
    default:
        break;
    }
    return sDesc;
}

// 特情类型
QString BizUtils::getSpecialTypeDesc(int nSpecialType)
{
    QString sDesc = "";
    switch (nSpecialType) {
    case EM_SpecialType::OBU_REMOVED:
        sDesc = "OBU拆卸";
        break;
    case EM_SpecialType::OBU_EXPIRED:
        sDesc = "OBU过期";
        break;
    case EM_SpecialType::OBU_NOT_ENABLED:
        sDesc = "OBU未启用";
        break;
    case EM_SpecialType::OBU_NO_CARD:
        sDesc = "OBU无卡";
        break;
    case EM_SpecialType::OBU_IN_BLACKLIST:
        sDesc = "OBU在状态名单里";
        break;
    case EM_SpecialType::OBU_EF04_PREFIX_ERR:
        sDesc = "OBU-EF04内前缀异常(第320-322字节非AA2900)";
        break;
    case EM_SpecialType::OBU_EF04_NO_CARD_CNT_GT0:
        sDesc = "OBU-EF04内标签无卡次数大于0";
        break;
    case EM_SpecialType::OBU_EF04_ENTRY_INVALID:
        sDesc = "OBU-EF04内入口无效";
        break;
    case EM_SpecialType::OBU_EF04_NO_PROV:
        sDesc = "OBU-EF04内通行省份数量为0";
        break;
    case EM_SpecialType::OBU_AMT_GT_MINFEE_1_5X:
        sDesc = "OBU累计金额大于最小费额1.5倍";
        break;
    case EM_SpecialType::OBU_AMT_LT_MINFEE:
        sDesc = "OBU累计金额小于最小费额";
        break;
    case EM_SpecialType::OBU_CROSS_PROV_NO_AMT:
        sDesc = "跨省交易出口省OBU本省累计金额为0";
        break;
    case EM_SpecialType::FEE_MILE_ABNORMAL:
        sDesc = "累计计费里程异常";
        break;
    case EM_SpecialType::ETC_CARD_EXPIRED:
        sDesc = "ETC卡过期";
        break;
    case EM_SpecialType::ETC_CARD_NOT_ENABLED:
        sDesc = "ETC卡未启用";
        break;
    case EM_SpecialType::ETC_CARD_IN_BLACKLIST:
        sDesc = "ETC卡在状态名单内";
        break;
    case EM_SpecialType::CARD_ENTRY_INVALID:
        sDesc = "ETC/CPC卡入口无效";
        break;
    case EM_SpecialType::PREPAID_INSUFFICIENT_BAL:
        sDesc = "储值卡余额不足";
        break;
    case EM_SpecialType::ETC_CARD_ZERO_BALANCE:
        sDesc = "ETC卡余额为0";
        break;
    case EM_SpecialType::CARD_AMT_GT_MINFEE_1_5X:
        sDesc = "卡累计金额大于最小费额1.5倍";
        break;
    case EM_SpecialType::CARD_AMT_LT_MINFEE:
        sDesc = "卡累计金额小于最小费额";
        break;
    case EM_SpecialType::CPC_DAMAGED:
        sDesc = "CPC卡损坏";
        break;
    case EM_SpecialType::CPC_MISSING:
        sDesc = "无CPC卡";
        break;
    case EM_SpecialType::ETC_OBU_PLATE_MISMATCH:
        sDesc = "ETC卡与OBU车牌（含颜色）不符";
        break;
    case EM_SpecialType::ETC_OBU_EF04_ENTRY_MISMATCH:
        sDesc = "ETC卡与OBU EF04内入口信息不一致";
        break;
    case EM_SpecialType::VEHICLE_TYPE_MISMATCH:
        sDesc = "出入口车辆车型不符";
        break;
    case EM_SpecialType::PLATE_MISMATCH:
        sDesc = "出入口车牌（含颜色）不符";
        break;
    case EM_SpecialType::FITTING_FAILED:
        sDesc = "拟合失败";
        break;
    case EM_SpecialType::PLATE_IN_PENALTY_LIST:
        sDesc = "车牌在追缴状态名单";
        break;
    case EM_SpecialType::NO_TAG_VEHICLE:
        sDesc = "无标签车辆";
        break;
    case EM_SpecialType::FITTING_SUCCESS:
        sDesc = "拟合成功";
        break;
    case EM_SpecialType::TEMP_PASS_BY_BLACKLIST:
        sDesc = "状态名单临时放行";
        break;
    case EM_SpecialType::MANUAL_AXLE_LIMIT_INPUT:
        sDesc = "手工录入轴限";
        break;
    case EM_SpecialType::NON_TRANSPORT_CONTAINER:
        sDesc = "非运输集装箱车";
        break;
    case EM_SpecialType::CONTAINER_20FT:
        sDesc = "20英尺集装箱";
        break;
    case EM_SpecialType::CONTAINER_2x20FT:
        sDesc = "2×20英尺集装箱";
        break;
    case EM_SpecialType::CONTAINER_40_OR_45FT:
        sDesc = "40或45英尺集装箱";
        break;
    case EM_SpecialType::VEHICLE_TYPE_FROM_ONLINE:
        sDesc = "使用在线稽核车型";
        break;
    case EM_SpecialType::VEHICLE_TYPE_FROM_INPUT:
        sDesc = "使用现有输入车型";
        break;
    case EM_SpecialType::DOUBLE_PERSON_REVIEW:
        sDesc = "双人复核";
        break;
    case EM_SpecialType::CPC_WRITE_FAIL_PASS:
        sDesc = "CPC写卡失败过车";
        break;
    default:
        break;
    }
    return sDesc;
}

bool BizUtils::isIncludeSpecialType(QString speiclaTypes, const QString &oneType)
{
    QStringList sl = speiclaTypes.split('|');
    for (int i = 0; i < sl.count(); i++) {
        if (sl.at(i) == oneType)
            return true;
    }
    return false;
}

// 系统标识
QString BizUtils::getSystemIDDesc(int nSystemID)
{
    QString sDesc = "";
    switch (nSystemID) {
    case EM_SystemID::SYS_NONE:
        sDesc = "系统无关";
        break;
    case EM_SystemID::SYS_LANE:
        sDesc = "车道";
        break;
    case EM_SystemID::SYS_STATION:
        sDesc = "收费站";
        break;
    case EM_SystemID::SYS_SUBCENTER:
        sDesc = "分中心系统";
        break;
    case EM_SystemID::SYS_PROV:
        sDesc = "省中心系统";
        break;
    default:
        break;
    }
    return sDesc;
}

// 用户类型
QString BizUtils::getUserTypeDesc(int nUserType)
{
    QString sDesc = "";
    switch (nUserType) {
    case EM_UserType::NORMAL_VEHICLE:
        sDesc = "普通车";
        break;
    case EM_UserType::MILITARY_POLICE:
        sDesc = "军警车";
        break;
    case EM_UserType::EMERGENCY:
        sDesc = "紧急车";
        break;
    case EM_UserType::FLEET:
        sDesc = "车队";
        break;
    case EM_UserType::GREEN:
        sDesc = "绿通车";
        break;
    case EM_UserType::HARVESTER:
        sDesc = "联合收割机";
        break;
    case EM_UserType::RESCUE:
        sDesc = "抢险救灾车";
        break;
    case EM_UserType::CONTAINER_J1:
        sDesc = "集装箱车(J1类)";
        break;
    case EM_UserType::BULK:
        sDesc = "大件运输车";
        break;
    case EM_UserType::EMERGENCY_RESCUE:
        sDesc = "应急车";
        break;
    case EM_UserType::TRAILER:
        sDesc = "货车列车或半挂汽车列车";
        break;
    case EM_UserType::CONTAINER_J2:
        sDesc = "集装箱车(J2类)";
        break;
    default:
        break;
    }
    return sDesc;
}

// 车辆状态
QString BizUtils::getVehStatusDesc(int nVehStatus)
{
    QString sDesc = "";
    switch (nVehStatus) {
    case EM_VehStatus::VEH_NORMAL:
        sDesc = "普通车";
        break;
    case EM_VehStatus::VEH_OFFICIAL:
        sDesc = "公务车";
        break;
    case EM_VehStatus::VEH_MILITARY:
        sDesc = "军警车";
        break;
    case EM_VehStatus::VEH_FRUIT:
        sDesc = "鲜果车";
        break;
    case EM_VehStatus::VEH_EMERGENCY:
        sDesc = "紧急车";
        break;
    case EM_VehStatus::VEH_SPECIAL:
        sDesc = "特殊车";
        break;
    case EM_VehStatus::VEH_FLEET:
        sDesc = "车队车";
        break;
    case EM_VehStatus::VEH_RESCUE:
        sDesc = "应急救援车辆";
        break;
    case EM_VehStatus::VEH_HARVESTER:
        sDesc = "联合收割机";
        break;
    case EM_VehStatus::VEH_SUPPLIES:
        sDesc = "运送救灾物资车辆";
        break;
    case EM_VehStatus::VEH_HOLIDAY_FREE:
        sDesc = "节假日免征车";
        break;
    case EM_VehStatus::VEH_VACCINE:
        sDesc = "疫苗车";
        break;
    case EM_VehStatus::VEH_URGENT:
        sDesc = "应急车";
        break;
    case EM_VehStatus::VEH_TEMP_FREE:
        sDesc = "临时免征车";
        break;
    default:
        break;
    }
    return sDesc;
}

QString BizUtils::getCardRobotMsgTypeDesc(int nCardRobotMsgType)
{
    QString sDesc = "";
    switch (nCardRobotMsgType) {
    case EM_CardRobotMsgType::MSG_CMD_TYPE_0:
        sDesc = "正回应";
        break;
    case EM_CardRobotMsgType::MSG_CMD_TYPE_1:
        sDesc = "负回应";
        break;
    case EM_CardRobotMsgType::MSG_CMD_TYPE_A:
        sDesc = "上电信息";
        break;
    case EM_CardRobotMsgType::MSG_CMD_TYPE_B:
        sDesc = "卡机状态信息";
        break;
    case EM_CardRobotMsgType::MSG_CMD_TYPE_D:
        sDesc = "按键取卡";
        break;
    case EM_CardRobotMsgType::MSG_CMD_TYPE_C:
        sDesc = "已出卡";
        break;
    case EM_CardRobotMsgType::MSG_CMD_TYPE_E:
        sDesc = "卡被取走";
        break;
    case EM_CardRobotMsgType::MSG_CMD_TYPE_Q:
        sDesc = "卡夹信息";
        break;
    case EM_CardRobotMsgType::MSG_CMD_TYPE_K:
        sDesc = "卡夹属性回应";
        break;
    case EM_CardRobotMsgType::MSG_CMD_TYPE_R:
        sDesc = "自动备卡回应";
        break;
    case EM_CardRobotMsgType::MSG_CMD_TYPE_G:
        sDesc = "卡回收完成";
        break;
    case EM_CardRobotMsgType::MSG_CMD_TYPE_H:
        sDesc = "按键忽略、延时";
        break;
    case EM_CardRobotMsgType::MSG_CMD_TYPE_I:
        sDesc = "纸券取走及状态信息";
        break;
    case EM_CardRobotMsgType::MSG_CMD_TYPE_Z:
        sDesc = "LPARAM整条命令";
        break;
    default:
        break;
    }
    return sDesc;
}
