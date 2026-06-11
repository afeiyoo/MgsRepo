#include <QApplication>
#include <QWidget>

#include "Logger.h"
#include "lanesystemgui.h"

namespace Color {
const char TOP_WIDGET_BG[] = "#EFF1F6";      // 顶栏背景色
const char MAIN_BG[] = "#D0D6E3";            // 主界面背景色
const char CUSTOM_AREA_BG[] = "#EFF1F6";     // 区域背景色
const char INSIDE_AREA_BG[] = "#EBEDF3";     // 内部信息区域背景色
const char CONFIRM_BUTTON_BG[] = "#007BFF";  // 确认按钮背景色
const char WARN_BUTTON_BG[] = "#FC3C3C";     // 警告背景色
const char INFO_BUTTON_BG[] = "#00BB65";     // 提示背景色
const char WEIGHTINFO_ITEM_BG[] = "#FFFFFF"; // 称重信息背景色

const char STATUS_TC[] = "#00BB65"; // 状态字体色
const char INFO_TC[] = "#0C4E94";   // 提示字体色
const char BUTTON_TC[] = "#FFFFFF"; // 按钮字体色
const char WARN_TC[] = "#FC3C3C";   // 警告字体色
} // namespace Color

void initMtcIn(MtcInPageController *pageCtrl)
{
    pageCtrl->setStationInfo("福州西(6701)(港口站)");
    pageCtrl->setLaneID(12);
    pageCtrl->setUserInfo("测试员(3501911)");
    pageCtrl->setShiftInfo("2025-12-25晚班");
    pageCtrl->setModeText("混合自动发卡");

    pageCtrl->setVirtualGantryInfo("福州西B向门架(34090.H)");
    pageCtrl->setAppVer("v1.0.1");
    pageCtrl->setFeeRateVer("1908");
    pageCtrl->setFullBlackVer("20250801");
    pageCtrl->setPartBlackVer("202508111211");

    pageCtrl->setScrollTip("欢迎使用福建省高速公路收费软件（如遇到软件问题请致电福建省高速公路信息科技有限公司）");

    pageCtrl->setPlate("拼闽B8L7222");
    pageCtrl->setVehClass("货四");
    pageCtrl->setVehStatus("抢险救灾车");
    pageCtrl->setSituation("超限");

    pageCtrl->setCardType("CPC卡");
    pageCtrl->setCardNum("35012042230602103792");
    pageCtrl->setEnTime("2025-07-12 12:00:04");
    pageCtrl->setEnStationName("福建莆田西站");
    pageCtrl->setBalance("88.2元");
    pageCtrl->setCardStatus("正常");

    pageCtrl->setTradeHint("自动发卡,等待过车");
    pageCtrl->setObuHint("大件运输车; 蓝闽B8L722; 普通车; 专二; OBUSN: 35011603330999998340 12轴型 2轴 10.00吨 限重18.00吨 超限0.00%");

    pageCtrl->setWeightLow(true);
    pageCtrl->setCurWeightInfo("12轴型 2轴 10.00吨 限重18.00吨 超限0.00%");
    pageCtrl->setCurWeightInfoCount(1);
    ST_WeightInfoItem item1;
    item1.plate = "闽A12345";
    item1.axisType = 1127;
    item1.axisNum = 6;
    item1.tollWeight = 25.74;
    item1.status = 1;
    pageCtrl->appendWeightInfoItem(item1);
    ST_WeightInfoItem item2;
    item2.plate = "闽B100000";
    item2.axisType = 1127;
    item2.axisNum = 6;
    item2.tollWeight = 25.74;
    item2.status = 0;
    pageCtrl->appendWeightInfoItem(item2);
    ST_WeightInfoItem item3;
    item3.plate = "闽BA50430";
    item3.axisType = 125;
    item3.axisNum = 4;
    item3.tollWeight = 22.74;
    item3.status = 0;
    pageCtrl->appendWeightInfoItem(item3);
    ST_WeightInfoItem item4;
    item4.plate = "闽BA50430";
    item4.axisType = 125;
    item4.axisNum = 4;
    item4.tollWeight = 22.74;
    item4.status = 0;
    pageCtrl->appendWeightInfoItem(item4);

    pageCtrl->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "CPC卡", "35012042230602103792"});
    pageCtrl->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "纸券", "35012042230602103792"});
    pageCtrl->appendTradeItem({"闽A12345F", "货一", "07-12 12:00:04", "ETC卡", "35012042230602103792"});
    pageCtrl->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "CPC卡", "35012042230602103792"});
    pageCtrl->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "无卡", ""});

    pageCtrl->setRobotStatus1(false, 200);
    pageCtrl->setRobotStatus2(true, 200);
    pageCtrl->setRobotStatus3(true, 400);
    pageCtrl->setRobotStatus4(true, 2);

    pageCtrl->setDeviceList({LaneSystemGUI::WEIGHT, LaneSystemGUI::FIRST_COIL, LaneSystemGUI::CELLING_LAMP, LaneSystemGUI::CAP_COIL,
                             LaneSystemGUI::CAPTURE, LaneSystemGUI::RAILLING_COIL, LaneSystemGUI::PASSING_LAMP, LaneSystemGUI::RAILING_MACHINE,
                             LaneSystemGUI::RSU});

    pageCtrl->setTotalVehCnt(9999);
    pageCtrl->setTotalCardCnt(9999);
    pageCtrl->setNoFlagCardCnt(2);
    pageCtrl->setPeccanyVehCnt(3);
    pageCtrl->setFleetVehCnt(4);
    pageCtrl->setCpcCardCnt(5);
    pageCtrl->setEtcCardCnt(80);
    pageCtrl->setBadCardCnt(123);
    pageCtrl->setPaperCardCnt(0);
    pageCtrl->setHolidayFreeVehCnt(3);
    pageCtrl->setCurTicketNum(9999999);
    pageCtrl->setStartTicketNum(9999999);
    pageCtrl->setEndTicketNum(9999999);
    pageCtrl->setRestTickCnt(20);

    pageCtrl->logAppend(LaneSystemGUI::INFO, "界面初始化1....");
    pageCtrl->logAppend(LaneSystemGUI::INFO, "数据库连接初始化完成1...");
    pageCtrl->logAppend(LaneSystemGUI::INFO, "界面初始化2....");
    pageCtrl->logAppend(LaneSystemGUI::INFO, "数据库连接初始化完成2...");
}

void initMtcOut(MtcOutPageController *pageCtrl)
{
    pageCtrl->setStationInfo("福州西(6701)(港口站)");
    pageCtrl->setLaneID(12);
    pageCtrl->setUserInfo("测试员(3501911)");
    pageCtrl->setShiftInfo("2025-12-25晚班");
    pageCtrl->setModeText("正常过车流程");

    pageCtrl->setVirtualGantryInfo("福州西B向门架(34090.H)");
    pageCtrl->setAppVer("v1.0.1");
    pageCtrl->setFeeRateVer("1908");
    pageCtrl->setFullBlackVer("20250801");
    pageCtrl->setPartBlackVer("202508111211");

    pageCtrl->setPrevImage(QImage(":/static/images/prev_pic.jpg"));

    pageCtrl->setStartTicketNum(63909944);
    pageCtrl->setCurTicketNum(63909946);
    pageCtrl->setNormalTicketCnt(102);
    pageCtrl->setScrapTicketCnt(0);
    pageCtrl->setRestTicketCnt(644);
    pageCtrl->setTotalVehCnt(879);
    pageCtrl->setRecycleCardCnt(857);
    pageCtrl->setEtcCardCnt(19);
    pageCtrl->setFleetVehCnt(0);
    pageCtrl->setThirdPayCnt(9858);
    pageCtrl->setUpStartTicketNum(63909944);
    pageCtrl->setUpCurTicketNum(63909946);
    pageCtrl->setUpRestTicketCnt(234);
    pageCtrl->setUpRecycleCardCnt(12);
    pageCtrl->setUpScrapTicketCnt(21);
    pageCtrl->setUpWriteErrCnt(75);
    pageCtrl->setDownStartTicketNum(63909944);
    pageCtrl->setDownCurTicketNum(63909946);
    pageCtrl->setDownRestTicketCnt(97);
    pageCtrl->setDownRecycleCardCnt(76);
    pageCtrl->setDownScrapTicketCnt(97);
    pageCtrl->setDownWriteErrCnt(76);

    pageCtrl->logAppend(LaneSystemGUI::INFO, "界面初始化....");
    pageCtrl->logAppend(LaneSystemGUI::INFO, "数据库连接初始化完成1...");
    pageCtrl->logAppend(LaneSystemGUI::INFO, "界面初始化....");
    pageCtrl->logAppend(LaneSystemGUI::INFO, "数据库连接初始化完成2...");
    pageCtrl->logAppend(LaneSystemGUI::INFO, "界面初始化....");
    pageCtrl->logAppend(LaneSystemGUI::INFO, "数据库连接初始化完成3...");
    pageCtrl->logAppend(LaneSystemGUI::INFO, "界面初始化....");
    pageCtrl->logAppend(LaneSystemGUI::INFO, "数据库连接初始化完成4...");

    pageCtrl->setScrollTip("欢迎使用福建省高速公路收费软件（如遇到软件问题请致电福建省高速公路信息科技有限公司）");

    pageCtrl->setPlate("拼闽B8L7222");
    pageCtrl->setVehClass("货四");
    pageCtrl->setVehStatus("抢险救灾车");
    pageCtrl->setSituation("超限");
    pageCtrl->setCardType("CPC卡");
    pageCtrl->setCardNum("35012211520060004842");
    pageCtrl->setEnStationName("福建莆田西站");
    pageCtrl->setEnTime("2025-07-12 12:00:00");
    pageCtrl->setWeightInfo("2轴 114 12.5吨");
    pageCtrl->setEnPlate("蓝闽B8L722");
    pageCtrl->setLabel1("福建莆田涵江萩芦镇");
    pageCtrl->setSplitProvincesInfo("福建100元;广东12元;武汉234元;福建100元;广东12元;武汉234元;");

    pageCtrl->setTradeHint("自动发卡,等待过车");
    pageCtrl->setObuHint("大件运输车; 蓝闽B8L722; 普通车; 专二; OBUSN: 35011603330999998340 12轴型 2轴 10.00吨 限重18.00吨 超限0.00%");
    pageCtrl->appendHintButton("第三方支付", Color::BUTTON_TC, Color::CONFIRM_BUTTON_BG);
    pageCtrl->appendHintButton("第三方支付", Color::BUTTON_TC, Color::INFO_BUTTON_BG);
    pageCtrl->appendHintButton("第三方支付", Color::BUTTON_TC, Color::WARN_BUTTON_BG);

    pageCtrl->setWeightLow(true);
    pageCtrl->setCurWeightInfo("12轴型 2轴 10.00吨 限重18.00吨 超限0.00%");
    pageCtrl->setCurWeightInfoCount(1);
    ST_WeightInfoItem item1;
    item1.plate = "闽A12345";
    item1.axisType = 1127;
    item1.axisNum = 6;
    item1.tollWeight = 25.74;
    item1.status = 1;
    pageCtrl->appendWeightInfoItem(item1);
    ST_WeightInfoItem item2;
    item2.plate = "闽B100000";
    item2.axisType = 1127;
    item2.axisNum = 6;
    item2.tollWeight = 25.74;
    item2.status = 0;
    pageCtrl->appendWeightInfoItem(item2);
    ST_WeightInfoItem item3;
    item3.plate = "闽BA50430";
    item3.axisType = 125;
    item3.axisNum = 4;
    item3.tollWeight = 22.74;
    item3.status = 0;
    pageCtrl->appendWeightInfoItem(item3);
    ST_WeightInfoItem item4;
    item4.plate = "闽BA50430";
    item4.axisType = 125;
    item4.axisNum = 4;
    item4.tollWeight = 22.74;
    item4.status = 0;
    pageCtrl->appendWeightInfoItem(item4);

    pageCtrl->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "1321.2/电子支付", "35012042230602103792"});
    pageCtrl->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "1323.2/第三方支付", "35012042230602103792"});
    pageCtrl->appendTradeItem({"闽A12345F", "货一", "07-12 12:00:04", "234/现金支付", "35012042230602103792"});
    pageCtrl->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "1234.5/第三方支付", "35012042230602103792"});
    pageCtrl->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "4/现金支付", "35012042230602103792"});

    pageCtrl->setDeviceList({LaneSystemGUI::WEIGHT, LaneSystemGUI::FIRST_COIL, LaneSystemGUI::CELLING_LAMP, LaneSystemGUI::CAP_COIL,
                             LaneSystemGUI::CAPTURE, LaneSystemGUI::RAILLING_COIL, LaneSystemGUI::PASSING_LAMP, LaneSystemGUI::RAILING_MACHINE,
                             LaneSystemGUI::RSU});
}

void initEtc(EtcPageController *pageCtrl)
{
    pageCtrl->setStationInfo("福州西(6701)(港口站)");
    pageCtrl->setLaneID(12);
    pageCtrl->setUserInfo("测试员(3501911)");
    pageCtrl->setShiftInfo("2025-12-25晚班");
    pageCtrl->setModeText("ETC入口");

    pageCtrl->setVirtualGantryInfo("福州西B向门架(34090.H)");
    pageCtrl->setAppVer("v1.0.1");
    pageCtrl->setFeeRateVer("1908");
    pageCtrl->setFullBlackVer("20250801");
    pageCtrl->setPartBlackVer("202508111211");

    pageCtrl->logAppend(LaneSystemGUI::INFO, "界面初始化....");
    pageCtrl->logAppend(LaneSystemGUI::INFO, "数据库连接初始化完成1...");
    pageCtrl->logAppend(LaneSystemGUI::INFO, "界面初始化....");
    pageCtrl->logAppend(LaneSystemGUI::INFO, "数据库连接初始化完成2...");
    pageCtrl->logAppend(LaneSystemGUI::INFO, "界面初始化....");
    pageCtrl->logAppend(LaneSystemGUI::INFO, "数据库连接初始化完成3...");
    pageCtrl->logAppend(LaneSystemGUI::INFO, "界面初始化....");
    pageCtrl->logAppend(LaneSystemGUI::INFO, "数据库连接初始化完成4...");

    pageCtrl->setScrollTip("欢迎使用福建省高速公路收费软件（如遇到软件问题请致电福建省高速公路信息科技有限公司）");

    pageCtrl->setPlate("拼闽B8L7222");
    pageCtrl->setVehClass("货四");
    pageCtrl->setVehStatus("抢险救灾车");
    pageCtrl->setSituation("无电子标签");

    pageCtrl->setCardType("CPC卡");
    pageCtrl->setTradeTime("16:55:20");
    pageCtrl->setEnStationName("莆田西站");
    pageCtrl->setProvince("福建");
    pageCtrl->setToll("入口");
    pageCtrl->setBalance("1234");

    pageCtrl->appendTradeItem({"闽A12345", "货一", "12:00:04", "祥谦", "CPC卡", "入口", "35012042230602103792"});
    pageCtrl->setTradeHint("等待过车");

    pageCtrl->setDeviceList({LaneSystemGUI::WEIGHT, LaneSystemGUI::FIRST_COIL, LaneSystemGUI::CELLING_LAMP, LaneSystemGUI::CAP_COIL,
                             LaneSystemGUI::CAPTURE, LaneSystemGUI::RAILLING_COIL, LaneSystemGUI::PASSING_LAMP, LaneSystemGUI::RAILING_MACHINE,
                             LaneSystemGUI::RSU});
}

int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    #else
    qputenv("QT_SCALE_FACTOR", "1.5");
    #endif
#endif
    QApplication a(argc, argv);

    LaneSystemGUI gui;
    gui.initFront(a);

    auto *mtcout = gui.createMtcOutWindow(false, true);
    initMtcOut(mtcout);

    QObject::connect(mtcout, &MtcOutPageController::sigKeyPress, [&](int key) {
        switch (key) {
        case Qt::Key_I:
            mtcout->showAuthDialog("", "");
            break;
        case Qt::Key_X:
            a.exit(0);
            break;
        }
    });

    int result = a.exec();
    if (result)
        LOG_WARNING().noquote() << "LaneSystemGUI 运行错误，错误码: " << result;
    return result;
}
