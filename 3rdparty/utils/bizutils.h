#pragma once

#include <QObject>

namespace Utils {

class BizUtils : public QObject
{
    Q_OBJECT
public:
    explicit BizUtils(QObject *parent = nullptr);

    ~BizUtils() override;

    /*******************************************************/
    /****                   项目业务相关                 ****/
    /*******************************************************/
    // 获取带颜色的车牌
    static QString getPlateWithColor(int color, const QString &plate);

    // 获取不带颜色的车牌
    static QString getPlateNoColor(const QString &fullPlate);

    // 车牌错误校核修复
    static QString correctVehplate(const QString &plate);

    // 从车牌中获取并返回颜色名称
    static QString getColorFromPlate(const QString &fullPlate);

    // 从车牌中获取并返回颜色编码
    static int getColorCodeFromPlate(const QString &fullPlate);

    // 根据颜色编码获取车牌颜色 0-蓝 1-黄 2-黑 3-白 4-绿 5-拼 6-渐 7-临 9-未知
    static QString getColorFormColorCode(int colorCode);

    // 获取按键对应的车道键盘按键名称
    static QString getKeyName(const QVariantMap &keyboard, uint keyCode);

    // 获取按键值对应的车道键盘按键描述
    static QString getKeyDescByCode(const QVariantMap &keyboard, uint keyCode);

    // 获取按键名称对应的车道键盘按键描述
    static QString getKeyDescByName(const QVariantMap &keyboard, const QString &keyName);

    // 获取车道键盘按键对应的按键数值(返回-1表示未获取到对应keyName的key)
    static int getKeyCode(const QVariantMap &keyboard, const QString &keyName);

    // 根据车型编码返回车型名称 isShort: true-获取车型短名称 false-获取长名称车型
    static QString getVehClassName(uint classCode, bool isShort = true);

    // 获取CPC卡里车辆标识值
    static QString getVehicleSignString(int UserType, int vehStatus);
    static int getVehicleSign(int UserType, int vehStatus);

    // 根据卡类型获取对应名称
    static QString getCardDesc(int cardType);

    // 字串str，在FullStr里的相符的百分比，如相符80%，值返回80
    static int getStrToFullStr(QString FullStr, QString str);

    // vehMode（vehFlow）中文描述
    static QString getVehModeName(int vehMode);

    // vehMode转换为车型
    static int getVehModeClass(int vehMode);
    /*******************************************************/
    /****                   DTP传输相关                  ****/
    /*******************************************************/
    // 依据对象属性，制作用于DTP传输的报文内容
    static QString makeDtpContentFromObj(const QObject &obj);

    // 依据QString，制作用于Dtp传输的报文内容
    static QString makeDtpContentFromStr(const QStringList &strList);

    // 依据键值对，制作用于Dtp传输的报文内容
    static QString makeDtpContentFromMap(const QVariantMap &map);

    // 生成完整的DTP传输报文
    static QString makeDtpXml(const QString &content, const QString &businessId, const QString &fromNode, const QString &toNode, int recordCount);

    /*******************************************************/
    /****                   DataDict中文描述             ****/
    /*******************************************************/
    //RSU特情告警描述
    static QString getWarnDesc(int nWarnType);

    // 车辆违规类型
    static QString getVehPeccTypeDesc(int nVehPeccType);

    // 卡类型
    static QString getCardTypeDesc(int nCardType);

    // 卡业务类型
    static QString getCardBizTypeDesc(int nCardBizType);

    // 支付方式
    static QString getPayTypeDesc(int nPayType);

    // 黑名单类型
    static QString getBlackTypeDesc(int nBlackType);

    // 变更类型
    static QString getChangeTypeDesc(int nChangeType);

    // 标识站
    static QString getChkPointDesc(int nChkPoint);

    // 出口车情
    static QString getDealStatusDesc(int nDealStatus, QString delimiter = ",");

    // 计费方式
    static QString getExitFeeTypeDesc(int nExitFeeType);

    // 免费类型
    static QString getFreeTypeDesc(int nFreeType);

    // 参数类型
    static QString getParamTypeDesc(int nParamType);

    // 代扣类型
    static QString getRebateTypeDesc(int nRebateType);

    // 记录类型
    static QString getRecordTypeDesc(int nRecordType);

    // 记录类型2
    static QString getRecordType2Desc(int nRecordType2);

    // 作废类型
    static QString getScrapTypeDesc(int nScrapType);

    // 出口班次日期（班次类型）
    static QString getShiftDateTypeDesc(int nShiftDateType);

    // 特情类型
    static QString getSpecialTypeDesc(int nSpecialType);

    //specialType:"35 | 25 | 36",oneType:"25",若25在specialType里 return true
    static bool isIncludeSpecialType(QString speiclaTypes, const QString &oneType);

    // 系统标识
    static QString getSystemIDDesc(int nSystemID);

    // 用户类型
    static QString getUserTypeDesc(int nUserType);

    // 车辆状态
    static QString getVehStatusDesc(int nVehStatus);

    // 记录类型2 (CardRobotMsgType)
    static QString getCardRobotMsgTypeDesc(int nCardRobotMsgType);
};

} // namespace Utils
