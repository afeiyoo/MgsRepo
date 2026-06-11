#pragma once

#include <QDateTime>
#include <QFlags>
#include <QMap>
#include <QObject>
#include <QVariant>

//枚举类导出  兼容QT5低版本
#define M_BEGIN_ENUM_CREATE(CLASS) \
    class CLASS : public QObject \
    { \
        Q_OBJECT \
    public:

#define M_END_ENUM_CREATE(CLASS) \
private: \
    Q_DISABLE_COPY(CLASS) \
    } \
    ;

#define M_ENUM_CREATE(CLASS) Q_ENUM(CLASS)

//==============================================================================
// 通用枚举定义
//==============================================================================

// 数据库类型枚举
M_BEGIN_ENUM_CREATE(EM_DataBaseType)
enum DataBaseType { MYSQL = 1, DAMENG };
M_ENUM_CREATE(DataBaseType)
M_END_ENUM_CREATE(EM_DataBaseType)

// 服务的socket类型
M_BEGIN_ENUM_CREATE(EM_SocketType)
enum SocketType { TCP = 0, WEBSOCKET };
M_ENUM_CREATE(SocketType)
M_END_ENUM_CREATE(EM_SocketType)

// 后端服务调用方式
M_BEGIN_ENUM_CREATE(EM_ServiceMode)
enum ServiceMode { LOCAL = 0, ONLINE };
M_ENUM_CREATE(ServiceMode)
M_END_ENUM_CREATE(EM_ServiceMode)

// 车道系统类型
M_BEGIN_ENUM_CREATE(EM_LaneType)
enum LaneType { MTC_IN = 1, MTC_OUT, ETC_IN, ETC_OUT, RAMP };
M_ENUM_CREATE(LaneType)
M_END_ENUM_CREATE(EM_LaneType)

// 卡类型
M_BEGIN_ENUM_CREATE(EM_CardType)
enum CardType {
    NONE = 0,     // 空白卡或未知
    IC_CARD = 10, // IC卡
    PAPER = 12,   // 纸性券
    CPC = 15,     // CPC
    ID_CARD = 20, // 身份卡
    OBU = 21,     // 单片式OBU（暂未用过）
    PREPAID = 22, // 储值卡
    CREDIT = 23   // 记账卡
};
M_ENUM_CREATE(CardType)
M_END_ENUM_CREATE(EM_CardType)

// 卡业务类型
M_BEGIN_ENUM_CREATE(EM_CardBizType)
enum CardBizType {
    MANUAL = 10,         // 人工发卡模式
    ROBOT = 13,          // 自助发卡机
    OPENING = 14,        // 开放式车道
    HOLIDAY = 16,        // 节假日免征直行
    ERR_ENEX_FLAG = 17,  // 进出站标志错
    ENTRY_UNKOWN = 19,   // 入口不明
    ECNY = 30,           // 数字人民币
    WECHAT = 31,         // 微信支付
    ALIPAY = 32,         // 支付宝支付
    CPC_ETC_PAY = 33,    // CPC卡使用ETC卡支付
    PAPER_ETC_PAY = 34,  // 纸券使用ETC卡支付
    NO_CARD_ETC_PAY = 35 // 无卡使用ETC卡支付
};
M_ENUM_CREATE(CardBizType)
M_END_ENUM_CREATE(EM_CardBizType)

// 车牌颜色
M_BEGIN_ENUM_CREATE(EM_PlateColor)
enum PlateColor {
    Blue = 0x0000,
    Yellow = 0x0001,
    Black = 0x0002,
    White = 0x0003,
    Green = 0x0004,
    Plain = 0x0005,    // 拼
    Gradient = 0x0006, // 渐
    Temporty = 0x0007, // 临
    Unknown = 0x0009
};
M_ENUM_CREATE(PlateColor)
M_END_ENUM_CREATE(EM_PlateColor)

// 支付方式
M_BEGIN_ENUM_CREATE(EM_PayType)
enum PayType {
    CASH = 0,     // 现金支付
    ETC_CARD = 1, // 电子支付
    SCAN = 2,     // 第三方支付
    UNPAID = 3    // 闯关未付
};
M_ENUM_CREATE(PayType)
M_END_ENUM_CREATE(EM_PayType)

// 黑名单类型
M_BEGIN_ENUM_CREATE(EM_BlackType)
enum BlackType {
    VEHICLE = 0,    // 状态名单车
    CPC_CARD = 1,   // 状态名单通行卡
    ID_CARD = 2,    // 状态名单身份卡
    OBU = 3,        // 状态名单OBU
    ETC_CARD = 4,   // 状态名单ETC卡
    TERMINAL = 5,   // 状态名单终端
    LAW_VEHICLE = 6 // 路政状态名单车
};
M_ENUM_CREATE(BlackType)
M_END_ENUM_CREATE(EM_BlackType)

//变更类型
M_BEGIN_ENUM_CREATE(EM_ChangeType)
enum ChangeType {
    NONE = 0,                // 无
    CAR_TO_TRUCK = 5,        // 客车变货车
    TRUCK_TO_CARD = 6,       // 货车变客车
    HIGH_TO_LOW = 7,         // 高变低
    LOW_TO_HIGH = 8,         // 低变高
    CAR_TO_OPERATION = 9,    // 客变专
    OPERATION_TO_CAR = 10,   // 专变客
    TRUCK_TO_OPERATION = 11, // 货变专
    OPERATION_TO_TRUCK = 12  // 专变货
};
M_ENUM_CREATE(ChangeType)
M_END_ENUM_CREATE(EM_ChangeType)

// 标识站
M_BEGIN_ENUM_CREATE(EM_ChkPoint)
enum ChkPoint {
    QINGZHOU_BRIDGE = 1,   // 青州大桥
    XIAXIANG_BRIDGE = 2,   // 厦漳跨海大桥
    PINGTAN_BRIDGE = 4,    // 平潭大桥
    QUANZHOUWAN_BRIDGE = 8 // 泉州湾跨海大桥
};
M_ENUM_CREATE(ChkPoint)
M_END_ENUM_CREATE(EM_ChkPoint)

// 出口车情
M_BEGIN_ENUM_CREATE(EM_DealStatus)
enum DealStatus {
    NONE = 0,                           // 无
    NO_CARD = 1,                        // 无卡
    BAD_CARD = 2,                       // 坏卡
    PLATE_DIFF = 4,                     // 车牌不符
    U_TURN = 8,                         // U转
    TIMEOUT = 16,                       // 超时
    CHANGED = 32,                       // 变更
    UNPAID = 64,                        // 未付
    NO_PLATE_DIFF = 128,                // 非车牌不符
    U_TURN_NORMAL = 256,                // 正常U转车
    TIMEOUT_NORMAL = 512,               // 正常超时
    U_TURN_CAL = 1024,                  // 正常计费U转
    GUIAN_CAR = 2048,                   // 贵安专用车
    SCAN_PAY = 4096,                    // 二维码支付
    BINHAI_REBATE = 8192,               // 滨海新城代扣
    MAWEI_REBATE = 16384,               // 马尾大桥代扣
    WUYISHAN_REBATE = 32768,            // 武夷山代扣
    SANMING_SHAXIAN_REBATE = 65536,     // 三明沙县代扣
    JINJIANGNAN_LONGHU_REBATE = 131072, // 晋江南龙湖代扣
    NANPING_JIANYANG_REBATE = 262144,   // 南平建阳代扣
    WUPINGSHIFANG_REBATE = 1048576,     // 武平十方代扣
    SHAOWU_WUJIATANG_REBATE = 2097152,  // 邵武吴家塘代扣
    GUANGZE_JINLING_REBATE = 4194304,   // 光泽金岭代扣
    PUCHENG_REBATE = 8388608,           // 浦城代扣
    YANPING_REBATE = 16777216,          // 延平代扣
    YANPINGBEI_REBATE = 33554432        // 延平北代扣
};
M_ENUM_CREATE(DealStatus)
M_END_ENUM_CREATE(EM_DealStatus)

// 计费方式
M_BEGIN_ENUM_CREATE(EM_ExitFeeType)
enum ExitFeeType {
    OBU_NO_DISCOUNT = 1,     // 按OBU内累计优惠后金额计费
    OBU_BEFORE_DISCOUNT = 2, // 按OBU内累计优惠前金额计费
    CPC_CARD = 3,            // 按CPC卡内累计金额计费
    PROVINCE_CLOUD_FEE = 4,  // 省中心在线服务计费
    MOT_CLOUD_FEE = 5,       // 部中心在线服务计费
    SHORT = 6                // 按最小费额计费
};
M_ENUM_CREATE(ExitFeeType)
M_END_ENUM_CREATE(EM_ExitFeeType)

// 免费类型
M_BEGIN_ENUM_CREATE(EM_FreeType)
enum FreeType {
    PROVINCE_FREE = 0, // 全省免征
    REGION_FREE = 1    // 区域免征
};
M_ENUM_CREATE(FreeType)
M_END_ENUM_CREATE(EM_FreeType)

// 车道用途类型
M_BEGIN_ENUM_CREATE(EM_LaneUseType)
enum LaneUseType {
    NORMAL_LANE = 0,             // 普通车道
    PORTABLE_LANE = 1,           // 便携车道
    UNMANNED_CARD_LANE = 2,      // 无人发卡车道
    ETC_LANE = 3,                // ETC车道
    OPEN_LANE = 4,               // 开放式车道
    PASSENGER_ETC_ONLY = 5,      // 客车ETC专用道
    TRUCK_ETC_ONLY = 6,          // 货车ETC专用道
    SELF_SERVICE_LANE = 8,       // 自助缴费终端车道
    EMERGENCY_TERMINAL_LANE = 9, // 应急终端车道
    PAPER_TICKET_LANE = 10       // 自助纸券车道
};
M_ENUM_CREATE(LaneUseType)
M_END_ENUM_CREATE(EM_LaneUseType)

// 参数类型
M_BEGIN_ENUM_CREATE(EM_ParamType)
enum ParamType {
    FEE_RATE_PARAM = 10,            // 费率参数
    AXLE_TYPE_PARAM = 11,           // 轴型参数
    VEHICLE_SEAT_PARAM = 12,        // 车型坐位参数
    WEIGHT_CORRECTION_PARAM = 13,   // 计重较正参数
    SHIFT_PARAM = 14,               // 班次参数
    TRAFFIC_CALIBRATION_PARAM = 15, // 交调口径折算系数
    HOLIDAY_PARAM = 16,             // 节假日免征参数
    DISCOUNT_USER_PARAM = 17,       // 折扣用户参数
    DISCOUNT_FEE_PARAM = 18,        // DiscountFee折扣参数
    PARAM_DICT_PARAM = 19,          // T_ParamDict参数
    SEGMENT_FEE_PARAM = 20,         // 分段计费参数

    REDUCE_EXEMPT_PARAM = 30, // 减免征车参数
    LAW_TRAILER_PARAM = 31,   // 路政拖车参数

    ETC_BLACKLIST_PARAM = 40,      // ETC状态名单参数
    VEHICLE_BLACKLIST_PARAM = 41,  // 状态名单车参数
    PASSCARD_BLACKLIST_PARAM = 42, // 状态名单通行卡参数
    LAW_BLACKLIST_PARAM = 43,      // 路政状态名单参数
    OTHER_BLACKLIST_PARAM = 44,    // 其它状态名单

    OTHER_FEE_PARAM = 99 // 其它收费参数
};
M_ENUM_CREATE(ParamType)
M_END_ENUM_CREATE(EM_ParamType)

// 代扣类型
M_BEGIN_ENUM_CREATE(EM_RebateType)
enum RebateType {
    GUIAN_REBATE = 1061,          // 贵安商学院代扣
    BINHAI_REBATE = 1063,         // 滨海新城代扣
    MAWEI_REBATE = 1065,          // 马尾大桥(福州城区)代扣
    SHAOWU_JINTANG_REBATE = 1084, // 邵武金塘代扣
    YANPING_LIST_REBATE = 1094,   // 延平代扣(名单)

    WUYISHAN_REBATE = 100501,        // 武夷山代扣
    SANMING_SHAXIAN_REBATE = 100601, // 三明沙县代扣
    // JINJIANGNAN_LONGHU__REBATE  = 100701,   // 晋江南龙湖代扣(折上折)
    JINJIANGNAN_LONGHU_REBATE = 100702,  // 晋江南龙湖代扣
    NANPING_JIANYANG_REBATE = 100801,    // 南平建阳代扣
    JIANYANG_NANPINGNAN_REBATE = 100901, // 建阳南平南代扣

    WUPING_DUANWU_2024_REBATE = 101001,     // 武平十方代扣2024端午
    WUPING_MID_AUTUMN_2024_REBATE = 101002, // 武平十方代扣2024中秋
    WUPING_NATIONAL_2024_REBATE = 101003,   // 武平十方代扣2024国庆
    WUPING_NEWYEAR_2025_REBATE = 101011,    // 武平十方代扣2025元旦
    WUPING_SPRING_2025_REBATE = 101012,     // 武平十方代扣2025春运
    WUPING_QINGMING_2025_REBATE = 101013,   // 武平十方代扣2025清明
    WUPING_LABOR_2025_REBATE = 101014,      // 武平十方代扣2025劳动
    WUPING_DUANWU_2025_REBATE = 101015,     // 武平十方代扣2025端午
    WUPING_NATIONAL_2025_REBATE = 101016,   // 武平十方代扣2025国庆

    GUANGZE_JINLING_REBATE = 101201, // 光泽金岭代扣
    PUCHENG_REBATE = 101301,         // 浦城代扣
    YANPING_REBATE = 101401,         // 延平代扣
    YANPINGBEI_REBATE = 101501,      // 延平北代扣

    FUZHOUDONG_JICHANG_REBATE = 25159101, // 机场代扣(福州东-机场)
    MAWEI_JICHANG_REBATE = 25159102       // 机场代扣(马尾-机场)
};
M_ENUM_CREATE(RebateType)
M_END_ENUM_CREATE(EM_RebateType)

// 记录类型2
M_BEGIN_ENUM_CREATE(EM_RecordType)
enum RecordType {
    WEIGHT_LOW = 1,                //称重降级数据
    MANUAL_WEIGHT = 2,             //手工插入称重
    CHANGE_AXIS_TYPE = 4,          //人工改轴型
    CHANGE_AXISES = 8,             //人工改轴数
    CHANGE_WEIGHT = 12,            //轴型+轴数更改
    TRAILER = 16,                  //拖车
    ACCIDENT = 32,                 //事故车
    LONG_CAR = 64,                 //超长车
    FAKE_AXIS = 128,               //假轴车
    NO_ENOUGH_MONEY = 512,         //ETC卡余额不足
    GREEN_TO_NORMAL = 1024,        //绿通变普通
    ENTRY_ERROR = 2048,            //入口信息校验出错
    SPT = 4096,                    //自助缴费终端
    CROSS_PROVINCE = 8192,         //跨省
    REMOTE_ASSIST = 16384,         //远程辅助
    ENTRY_EXIT_FLAG_ERROR = 32768, //入出口异常
};
M_ENUM_CREATE(RecordType)
M_END_ENUM_CREATE(EM_RecordType)

// 记录类型2
M_BEGIN_ENUM_CREATE(EM_RecordType2)
enum RecordType2 {
    CPC_CARD_CAL = 1,         // 根据CPC卡内数据计费
    SHORT_CAL = 2,            // 最短可达路径计费
    SHORT_ERROR_CAL = 4,      // 路径不可达（省内最远站点计费）
    BLACK = 8,                // 状态名单车或状态名单卡
    NO_DISCOUNT_ETC = 16,     // 单ETC卡无折扣
    FORBIDDEN_PASSENGER = 32, // 不允许通行的两客一危车辆
    PLATE_DIFFER = 64,        // 卡车牌与抓拍车牌不符
    ETC_CAL_ERROR = 128,      // 出口ETC累计金额异常
    OBU_CARD_DIFF = 256,      // 卡签入口信息不符
    TEST_VEHICLE = 512,       // 测试车
    RT2_SA_UTURN = 1024,      //服务区U转车 SA:Service Area
    SHORT_80_PERCENT = 2048   // 兜底80%
};
M_ENUM_CREATE(RecordType2)
M_END_ENUM_CREATE(EM_RecordType2)

// 作废类型
M_BEGIN_ENUM_CREATE(EM_ScrapType)
enum ScrapType {
    NORMAL_TICKET = 0,      // 正常票
    REPRINT_TICKET = 1,     // 重打票
    ADJUSTED_TICKET = 2,    // 调整票
    TRADE_SCRAP = 3,        // 交易废票
    NON_TRADE_SCRAP = 4,    // 非交易废票
    BACKEND_TRADE_SCRAP = 5 // 后台交易废票
};
M_ENUM_CREATE(ScrapType)
M_END_ENUM_CREATE(EM_ScrapType)

// 班次日期（班次类型）
M_BEGIN_ENUM_CREATE(EM_ShiftDateType)
enum ShiftDateType {
    MORNING = 1,   // 早班
    AFTERNOON = 2, // 中班
    NIGHT = 3      // 晚班
};
M_ENUM_CREATE(ShiftDateType)
M_END_ENUM_CREATE(EM_ShiftDateType)

// 特情类型
M_BEGIN_ENUM_CREATE(EM_SpecialType)
enum SpecialType {
    OBU_REMOVED = 2,                  // OBU拆卸
    OBU_EXPIRED = 3,                  // OBU过期
    OBU_NOT_ENABLED = 4,              // OBU未启用
    OBU_NO_CARD = 5,                  // OBU无卡
    OBU_IN_BLACKLIST = 6,             // OBU在状态名单里
    OBU_EF04_PREFIX_ERR = 10,         // OBU-EF04内前缀异常(第320-322字节非AA2900)
    OBU_EF04_NO_CARD_CNT_GT0 = 11,    // OBU-EF04内标签无卡次数大于0
    OBU_EF04_ENTRY_INVALID = 12,      // OBU-EF04内入口无效
    OBU_EF04_NO_PROV = 13,            // OBU-EF04内通行省份数量为0
    OBU_AMT_GT_MINFEE_1_5X = 14,      // OBU累计金额大于最小费额1.5倍
    OBU_AMT_LT_MINFEE = 15,           // OBU累计金额小于最小费额
    OBU_CROSS_PROV_NO_AMT = 16,       // 跨省交易出口省OBU本省累计金额为0
    FEE_MILE_ABNORMAL = 18,           // 累计计费里程异常
    ETC_CARD_EXPIRED = 21,            // ETC卡过期
    ETC_CARD_NOT_ENABLED = 22,        // ETC卡未启用
    ETC_CARD_IN_BLACKLIST = 24,       // ETC卡在状态名单内
    CARD_ENTRY_INVALID = 26,          // ETC/CPC卡入口无效
    PREPAID_INSUFFICIENT_BAL = 27,    // 储值卡余额不足
    ETC_CARD_ZERO_BALANCE = 28,       // ETC卡余额为0
    CARD_AMT_GT_MINFEE_1_5X = 29,     // 卡累计金额大于最小费额1.5倍
    CARD_AMT_LT_MINFEE = 30,          // 卡累计金额小于最小费额
    CPC_DAMAGED = 35,                 // CPC卡损坏
    CPC_MISSING = 36,                 // 无CPC卡
    ETC_OBU_PLATE_MISMATCH = 41,      // ETC卡与OBU车牌（含颜色）不符
    ETC_OBU_EF04_ENTRY_MISMATCH = 45, // ETC卡与OBU EF04内入口信息不一致
    VEHICLE_TYPE_MISMATCH = 51,       // 出入口车辆车型不符
    PLATE_MISMATCH = 52,              // 出入口车牌（含颜色）不符
    FITTING_FAILED = 68,              // 拟合失败
    PLATE_IN_PENALTY_LIST = 118,      // 车牌在追缴状态名单
    NO_TAG_VEHICLE = 123,             // 无标签车辆
    FITTING_SUCCESS = 500,            // 拟合成功
    TEMP_PASS_BY_BLACKLIST = 501,     // 状态名单临时放行
    MANUAL_AXLE_LIMIT_INPUT = 502,    // 手工录入轴限
    NON_TRANSPORT_CONTAINER = 511,    // 非运输集装箱车
    CONTAINER_20FT = 512,             // 20英尺集装箱
    CONTAINER_2x20FT = 513,           // 2×20英尺集装箱
    CONTAINER_40_OR_45FT = 514,       // 40或45英尺集装箱
    VEHICLE_TYPE_FROM_ONLINE = 520,   // 使用在线稽核车型
    VEHICLE_TYPE_FROM_INPUT = 521,    // 使用现有输入车型
    DOUBLE_PERSON_REVIEW = 522,       // 双人复核
    CPC_WRITE_FAIL_PASS = 523         // CPC写卡失败过车
};
M_ENUM_CREATE(SpecialType)
M_END_ENUM_CREATE(EM_SpecialType)

// 系统标识
M_BEGIN_ENUM_CREATE(EM_SystemID)
enum SystemID {
    SYS_NONE = 0,      // 系统无关
    SYS_LANE = 1,      // 车道
    SYS_STATION = 2,   // 收费站
    SYS_SUBCENTER = 3, // 分中心系统
    SYS_PROV = 4       // 省中心系统
};
M_ENUM_CREATE(SystemID)
M_END_ENUM_CREATE(EM_SystemID)

// 用户类型
M_BEGIN_ENUM_CREATE(EM_UserType)
enum UserType {
    NORMAL_VEHICLE = 0,    // 普通车
    MILITARY_POLICE = 8,   // 军警车
    EMERGENCY = 10,        // 紧急车
    FLEET = 14,            // 车队
    SUPPORT = 15,          // 保障事务车
    GREEN = 21,            // 绿通车
    HARVESTER = 22,        // 联合收割机
    RESCUE = 23,           // 抢险救灾车
    CONTAINER_J1 = 24,     // 集装箱车(J1类)
    BULK = 25,             // 大件运输车
    EMERGENCY_RESCUE = 26, // 应急车
    TRAILER = 27,          // 货车列车或半挂汽车列车
    CONTAINER_J2 = 28      // 集装箱车(J2类)
};
M_ENUM_CREATE(UserType)
M_END_ENUM_CREATE(EM_UserType)

// 车牌颜色
M_BEGIN_ENUM_CREATE(EM_VehColor)
enum VehColor {
    BLUE = 1,   // 蓝
    YELLOW = 2, // 黄
    WHITE = 3,  // 白
    BLACK = 4   // 黑
};
M_ENUM_CREATE(VehColor)
M_END_ENUM_CREATE(EM_VehColor)

// 车辆违规类型
M_BEGIN_ENUM_CREATE(EM_VehPeccType)
enum VehPeccType {
    NORMAL = 0,             // 正常
    BREAKTHROUGH = 1,       // 冲关
    UNPAID = 2,             // 未付
    EVADE_FEE = 3,          // 逃费
    SPECIAL_SEARCH = 4,     // 特殊情况配合查找
    LAW_BLACKLIST = 5,      // 路政状态名单车
    MALICIOUS_OWE = 110,    // 电子收费恶意欠费车辆
    ETC_ARREARS_LIST = 111, // ETC欠费状态名单
    BANK_ADVANCE_LIST = 112 // 银行垫付状态名单
};
M_ENUM_CREATE(VehPeccType)
M_END_ENUM_CREATE(EM_VehPeccType)

// 车辆状态
M_BEGIN_ENUM_CREATE(EM_VehStatus)
enum VehStatus {
    VEH_NORMAL = 0,         // 普通车
    VEH_OFFICIAL = 1,       // 公务车
    VEH_MILITARY = 2,       // 军警车
    VEH_FRUIT = 3,          // 鲜果车
    VEH_EMERGENCY = 4,      // 紧急车
    VEH_SPECIAL = 5,        // 特殊车
    VEH_FLEET = 11,         // 车队车
    VEH_RESCUE = 26,        // 应急救援车辆
    VEH_HARVESTER = 101,    // 联合收割机
    VEH_SUPPLIES = 102,     // 运送救灾物资车辆
    VEH_HOLIDAY_FREE = 103, // 节假日免征车
    VEH_VACCINE = 105,      // 疫苗车
    VEH_URGENT = 106,       // 应急车
    VEH_TEMP_FREE = 107     // 临时免征车
};
M_ENUM_CREATE(VehStatus)
M_END_ENUM_CREATE(EM_VehStatus)

// 告警类型
M_BEGIN_ENUM_CREATE(EM_WarnType)
enum WarnType {
    WARN_BALANCE_LOW = 1,            // 余额不足
    WARN_OBU_TAMPER_FAIL = 2,        // OBU防拆失效
    WARN_OBU_DISABLED = 4,           // OBU禁用
    WARN_IC_DISABLED = 5,            // IC卡禁用
    WARN_OBU_EXPIRED = 6,            // OBU有效期过期
    WARN_IC_EXPIRED = 7,             // IC卡有效期过期
    WARN_OBU_NOT_ENABLED = 8,        // OBU尚未启用
    WARN_IC_NOT_ENABLED = 9,         // IC卡尚未启用
    WARN_TIMEOUT = 10,               // 超时
    WARN_BREAKTHROUGH = 11,          // 闯关
    WARN_NO_ENTRY_INFO = 12,         // 无入口信息
    WARN_NO_CARD = 13,               // 标签无卡
    WARN_IC_INVALID_ISSUER = 14,     // IC卡发行方无效
    WARN_OBU_INVALID_ISSUER = 15,    // OBU发行方无效
    WARN_NO_FEE = 16,                // 查询不到费率
    WARN_FEE_ERR = 17,               //费率计算错误
    WARN_IC_REPORTED_LOST = 18,      // IC卡挂失
    WARN_OBU_REPORTED_LOST = 19,     // OBU挂失
    WARN_LANE_INTERFERENCE = 20,     // 邻道干扰
    WARN_WRITE_CARD_FAIL = 40,       // 写卡故障无法交易，B4帧写卡错误
    WARN_DEVICE_FAULT = 41,          // 设备故障导致无法交易
    WARN_BLACKLIST_VEHICLE = 50,     // 状态名单车
    WARN_BLACKLIST_CARD = 51,        // 状态名单卡
    WARN_BLACKLIST_TAG = 52,         // 状态名单标签
    WARN_NO_OBU = 53,                // 无电子标签
    WARN_U_TURN = 54,                // U转车
    WARN_ENTRY_EXIT_FLAG_ERROR = 55, // 进出站标志错
    WARN_UNKNOWN_VEHCLASS = 56,      // 车型不明
    WARN_OBU_CARD_NOTSAME = 57,      // 标卡信息不符
    WARN_PLATE_MISMATCH = 58,        // 车牌不符车
    WARN_FREE_VEHICLE_EXPIRED = 59,  // 免费车到期
    WARN_FREE_CARD_EXPIRED = 60,     // 免费卡到期
    WARN_PROHIBITED_VEHICLE = 61,    // 禁止通行车辆
    WARN_OBU_DEVICE_FAULT = 71,      // OBU设备故障
    WARN_OBU_ANTENNA_FAIL = 72,      // OBU与天线通讯故障
    WARN_OTHER = 99,                 // 其它
    WARN_B5_FRAME_ERROR = 100,       // B5帧错误
    WARN_LOCATION_ILLEGAL = 111,     //卡标归属地不一致
    WARN_ETC_CARD_INVALID = 115,     // ETC卡无效,卡类型错
    WARN_TRADE_ERROR = 116,          //交易失败(B5帧返回错误)
    WARN_FREE_TIMEOUT = 118,         //超出免征期限
    WARN_FREE_SCOPE = 119,           //超出免费区域
    WARN_CMD_ERROR = 121,            // RSU指令帧错
    WARN_SYS_ERROR = 122,            //系统错(未知)
    WARN_NETID_EREROR = 123,         //入网编号不明
    WARN_LAW_BALCK = 124,            //路政黑名单
    WARN_LAW_TRAILER = 125,          //拖挂车
    WARN_LAW_WEIGHT = 127,           //无称重数据
    WARN_STOP_WEIGHT = 128,          //超限称重数据
    WARN_USERTYPE_LTC = 129,         //出口绿通车
    WARN_USERTYPE_LHSGJ = 130,       //出口联合收割机
    WARN_LAW_JZX = 131,              //集装箱J1
    WARN_LAW_JZX2 = 132,             //集装箱J2
    WARN_OBU_FEE = 133,              // OBU计费信息异常
    WARN_OBU_EN = 134,               //入口信息异常
    WARN_OBU_PROV = 135,             // OBU通行省份个数不对
    WARN_OBU_FEE0 = 136,             // OBU计费信息异常(累计金额为0)
    WARN_ERR_FEE = 137,              //实收金额超大
    WARN_ERR_WEIGHT = 138,           //入口称重信息异常
    WARN_ERR_AXIS = 139,             //超6轴
    WARN_ERR_KSJZX = 140,            //跨省集装箱
    WARN_ERR_RETRADE = 141,          //重复交易
    WARN_ERR_NOAXIS = 142,           //罕见轴型
    WARN_ERR_NOVEHPLATE = 143,       //车牌与抓拍车牌不符
    WARN_AXLE_MODEL_MISMATCH = 144,  // 车轴型不符
    WARN_OVERSIZED_VEHICLE = 145     // 大件运输车

};
M_ENUM_CREATE(WarnType)
M_END_ENUM_CREATE(EM_WarnType)

M_BEGIN_ENUM_CREATE(EM_CardRobotMsgType)
enum CardRobotMsgType {
    MSG_CMD_TYPE_0 = '0', //正回应
    MSG_CMD_TYPE_1 = '1', //负回应
    MSG_CMD_TYPE_A = 'A', //上电信息
    MSG_CMD_TYPE_B = 'B', //卡机状态信息,LPARAM为TRobotStatus*指针
    MSG_CMD_TYPE_D = 'D', //按键取卡,LPARAM高字节为工位信息:1或2;低字节为当前卡机编号1~4
    MSG_CMD_TYPE_C = 'C', //已出卡,  LPARAM高字节为工位信息:1或2,3出卡失败;低字节为当前卡机编号1~4
    MSG_CMD_TYPE_E = 'E', //卡被取走,LPARAM高字节为工位信息:1或2;低字节为当前卡机编号1~4
    MSG_CMD_TYPE_Q = 'Q', //卡夹信息,LPARAM为TCardBagInfo*指针
    MSG_CMD_TYPE_K = 'K', //卡夹属性回应,LPARAM返回0不包含异或校验,1包含
    MSG_CMD_TYPE_R = 'R', //自动备卡回应,LPARAM返回1~4卡机编号
    MSG_CMD_TYPE_G = 'G', //卡回收完成,LPARAM高字节为工位信息:1或2,3;低字节为当前卡机编号1~4
    MSG_CMD_TYPE_H = 'H', //按键忽略、延时,LPARAM高字节为工位信息:1或2;低字节为忽略或延时原因
    //(0:正在出卡/回收;1:无卡可发;2:卡机故障;3:一边故障，一边无卡;4:降级模式,不处理按键
    //5:取卡走不到3秒;6:连续多张坏卡(2张))
    MSG_CMD_TYPE_I = 'I', // 纸券取走及状态信息 2024-09-03
    MSG_CMD_TYPE_Z = 'Z'  //LPARAM整条命令
};
M_ENUM_CREATE(CardRobotMsgType)
M_END_ENUM_CREATE(EM_CardRobotMsgType)

M_BEGIN_ENUM_CREATE(EM_WeightCmd)
enum WeightCmd {
    WeightAllAx = 0,          // 读取一辆车的整车信息(轴组方式)
    WeightAll = 1,            // 读取一辆车的整车信息(单轴方式)
    WeightAx = 2,             // 只读取一辆车的轴组重信息
    Weight = 3,               // 只读取一辆车的轴重信息
    WeightNum = 4,            // 读取子机数据缓冲区车辆数信息
    WeightStatus = 5,         // 轴重仪器自检
    WeightExit = 6,           // 车道车辆队列中最后一辆车退出收费车道(数据流方向：从机->主机)
    WeightZero = 7,           // 设备置零
    WeightDel = 8,            // 删除从机数据缓冲区中最前面一条数据
    WeightIdxAx = 9,          // 从从机数据缓冲区中读取指定序号的车辆称重数据(轴组方式)
    WeightIdx = 10,           // 从从机数据缓冲区读取指定序号的车辆称重数据(单轴方式)
    WeightTime = 11,          // 重新设定从机时间
    VehicleWeightAll = 12,    // 整车秤专用称重信息获取
    VehicleWeightMode = 13,   // 整车秤专用工作模式设定
    VehicleWeightStatus = 14, // 整车秤专用设备状态信息获取
    VehicleWeight = 15        // 整车秤实时重量获取
};
M_ENUM_CREATE(WeightCmd)
M_END_ENUM_CREATE(EM_WeightCmd)

M_BEGIN_ENUM_CREATE(EM_DeviceException)
enum DeviceException {
    WEIGHT = 1 << 0,                     // 称重
    CARD_ROBOT = 1 << 1,                 // 自动发卡机
    CARD_ROBOT_READER_UP_IC = 1 << 2,    // 发卡机读卡器上工位通行卡
    CARD_ROBOT_READER_DOWN_IC = 1 << 3,  // 发卡机读卡器下工位通行卡
    CARD_ROBOT_READER_UP_CPU = 1 << 4,   // 发卡机读卡器上工位闽通卡
    CARD_ROBOT_READER_DOWN_CPU = 1 << 5, // 发卡机读卡器下工位闽通卡
    CARD_CTRL = 1 << 6,                  // 桌面读卡器
    LANE_CONTROL = 1 << 7,               // 车道控制器
    CAPTURE = 1 << 8,                    // 抓拍
    LED = 1 << 9,                        // 费显
    NEAR_RSU = 1 << 10,                  // 天线
    CHECK_VD = 1 << 11,                  // 检测线圈
    INFO_BOARD = 1 << 12,                // 情报板
    OVERLAP = 1 << 13,                   // 字符叠加器
};
M_ENUM_CREATE(DeviceException)
M_END_ENUM_CREATE(EM_DeviceException)

// 混合入口车道交易状态
M_BEGIN_ENUM_CREATE(EM_MIBizStatus)
enum MIBizStatus {
    BS_UNINIT = 0,
    BS_ZERO,
    BS_START,
    BS_PRESS, // 已按键取卡（自动发卡模式）
    BS_VEHCLASS,
    BS_WRITED,
    BS_SENT, // 已发卡（自动发卡模式）
    BS_GOT,
    BS_DONE
};
M_ENUM_CREATE(MIBizStatus)
M_END_ENUM_CREATE(EM_MIBizStatus)

// 车道模式
M_BEGIN_ENUM_CREATE(EM_LaneMode)
enum LaneMode {
    AUTO_MODE = 0,     // 混合入口，自动发卡模式
    MANUAL_MODE,       // 混合入口，手动发卡模式
    NORMAL_PAPER_MODE, // 混合入口，普通纸券模式
    HOLIDAY_MODE,      // 混合入口，节假日模式
    FLEET_MODE,        // 混合入口，车队车模式
    AUTO_PAPER_MODE,   // 混合入口，自动发机打纸券模式
    DESK_PAPER_MODE    // 混合入口，桌面发机打纸券模式
};
M_ENUM_CREATE(LaneMode)
M_END_ENUM_CREATE(EM_LaneMode)

// 远程辅助操作类型
M_BEGIN_ENUM_CREATE(EM_RemoteAssistOP)
enum RemoteAssistOP {
    FLUSH_DEV_STATUS = 0, // 同步设备状态
    FLUSH_SHIFT_INFO,     // 刷新班次信息
    FLUSH_SYSTEM_INFO,    // 刷新系统信息，如门架信息，软件版本号
};
M_ENUM_CREATE(RemoteAssistOP)
M_END_ENUM_CREATE(EM_RemoteAssistOP)

// 过车流程
M_BEGIN_ENUM_CREATE(EM_VehMode)
enum VehMode {
    CAR = 0,   //客车流程
    TRUCK,     //货车流程
    OPERATION, //专项作业车流程
    CAR_LOCK,  //客车流程锁定
    TRUCK_LOCK //货车流程锁定
};
M_ENUM_CREATE(VehMode)
M_END_ENUM_CREATE(EM_VehMode)

// 交易模式
M_BEGIN_ENUM_CREATE(EM_TradeMode)
enum TradeMode {
    NONE = 0, //初始状态
    DESK,     //桌面流程
    ROBOT,    //自助流程
    RSU,      //RSU流程
};
M_ENUM_CREATE(TradeMode)
M_END_ENUM_CREATE(EM_TradeMode)

// ETC处理类返回MainCtrl的类型
M_BEGIN_ENUM_CREATE(EM_ETCtoMainCtrlType)
enum ETCtoMainCtrlType {
    REPASS = 1, //重放行
    WARNINFO,   //异常信息提示
    TRUCK       //
};
M_ENUM_CREATE(ETCtoMainCtrlType)
M_END_ENUM_CREATE(EM_ETCtoMainCtrlType)
