#include "mainwindow.h"

#include "Logger.h"
#include "global/uiconst.h"
#include "pages/etcpage.h"
#include "pages/mtcinpage.h"
#include "pages/mtcoutpage.h"

MainWindow::MainWindow(QWidget *parent)
    : ElaWindow(parent)
{
    setAppBarHeight(0);
    setIsFixedSize(false);
    setIsNavigationBarEnable(false);
    setWindowButtonFlags(ElaAppBarType::None);
    setWindowIcon(QIcon(":/static/images/mgskj_icon.png"));
    setMinimumSize(1024, 768);
    resize(1024, 768);
}

MainWindow::~MainWindow() {}

void MainWindow::initMtcIn()
{
    m_mainPageIn = new MtcInPage(this);

    addPageNode("主页面", m_mainPageIn, ElaIconType::House);

    m_mainPageIn->initUi();

    m_mainPageIn->setStationInfo("福州西(6701)(港口站)");
    m_mainPageIn->setLaneID(12);
    m_mainPageIn->setUserInfo("测试员(3501911)");
    m_mainPageIn->setShiftInfo("2025-12-25晚班");
    m_mainPageIn->setModeText("混合自动发卡");

    m_mainPageIn->setVirtualGantryInfo("福州西B向门架(34090.H)");
    m_mainPageIn->setAppVer("v1.0.1");
    m_mainPageIn->setFeeRateVer("1908");
    m_mainPageIn->setFullBlackVer("20250801");
    m_mainPageIn->setPartBlackVer("202508111211");

    m_mainPageIn->setScrollTip("欢迎使用福建省高速公路收费软件（如遇到软件问题请致电福建省高速公路信息科技有限公司）");

    m_mainPageIn->setPlate("拼闽B8L7222");
    m_mainPageIn->setVehClass("货四");
    m_mainPageIn->setVehStatus("抢险救灾车");
    m_mainPageIn->setSituation("超限");

    m_mainPageIn->setCardType("CPC卡");
    m_mainPageIn->setCardNum("35012042230602103792");
    m_mainPageIn->setEnTime("2025-07-12 12:00:04");
    m_mainPageIn->setEnStationName("福建莆田西站");
    m_mainPageIn->setBalance("88.2元");
    m_mainPageIn->setCardStatus("正常");

    m_mainPageIn->setTradeHint("自动发卡,等待过车");
    m_mainPageIn->setObuHint("大件运输车; 蓝闽B8L722; 普通车; 专二; OBUSN: 35011603330999998340 12轴型 2轴 10.00吨 限重18.00吨 超限0.00%");

    m_mainPageIn->setWeightLow(true);
    m_mainPageIn->setCurWeightInfo("12轴型 2轴 10.00吨 限重18.00吨 超限0.00%");
    m_mainPageIn->setCurWeightInfoCount(1);
    ST_WeightInfoItem item1;
    item1.plate = "闽A12345";
    item1.axisType = 1127;
    item1.axisNum = 6;
    item1.tollWeight = 25.74;
    item1.status = 1;
    m_mainPageIn->appendWeightInfoItem(item1);
    ST_WeightInfoItem item2;
    item2.plate = "闽B100000";
    item2.axisType = 1127;
    item2.axisNum = 6;
    item2.tollWeight = 25.74;
    item2.status = 0;
    m_mainPageIn->appendWeightInfoItem(item2);
    ST_WeightInfoItem item3;
    item3.plate = "闽BA50430";
    item3.axisType = 125;
    item3.axisNum = 4;
    item3.tollWeight = 22.74;
    item3.status = 0;
    m_mainPageIn->appendWeightInfoItem(item3);
    ST_WeightInfoItem item4;
    item4.plate = "闽BA50430";
    item4.axisType = 125;
    item4.axisNum = 4;
    item4.tollWeight = 22.74;
    item4.status = 0;
    m_mainPageIn->appendWeightInfoItem(item4);

    m_mainPageIn->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "CPC卡", "35012042230602103792"});
    m_mainPageIn->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "纸券", "35012042230602103792"});
    m_mainPageIn->appendTradeItem({"闽A12345F", "货一", "07-12 12:00:04", "ETC卡", "35012042230602103792"});
    m_mainPageIn->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "CPC卡", "35012042230602103792"});
    m_mainPageIn->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "无卡", ""});

    m_mainPageIn->setRobotStatus1(false, 200);
    m_mainPageIn->setRobotStatus2(true, 200);
    m_mainPageIn->setRobotStatus3(true, 400);
    m_mainPageIn->setRobotStatus4(true, 2);

    m_mainPageIn->setDeviceList({EM_DeviceIcon::WEIGHT, EM_DeviceIcon::FIRST_COIL, EM_DeviceIcon::CELLING_LAMP, EM_DeviceIcon::CAP_COIL,
                                 EM_DeviceIcon::CAPTURE, EM_DeviceIcon::RAILLING_COIL, EM_DeviceIcon::PASSING_LAMP, EM_DeviceIcon::RAILING_MACHINE,
                                 EM_DeviceIcon::RSU});

    m_mainPageIn->setTotalVehCnt(9999);
    m_mainPageIn->setTotalCardCnt(9999);
    m_mainPageIn->setNoFlagCardCnt(2);
    m_mainPageIn->setPeccanyVehCnt(3);
    m_mainPageIn->setFleetVehCnt(4);
    m_mainPageIn->setCpcCardCnt(5);
    m_mainPageIn->setEtcCardCnt(80);
    m_mainPageIn->setBadCardCnt(123);
    m_mainPageIn->setPaperCardCnt(0);
    m_mainPageIn->setHolidayFreeVehCnt(3);
    m_mainPageIn->setCurTicketNum(9999999);
    m_mainPageIn->setStartTicketNum(9999999);
    m_mainPageIn->setEndTicketNum(9999999);
    m_mainPageIn->setRestTickCnt(20);

    m_mainPageIn->logAppend(EM_LogLevel::INFO, "界面初始化1....");
    m_mainPageIn->logAppend(EM_LogLevel::INFO, "数据库连接初始化完成1...");
    m_mainPageIn->logAppend(EM_LogLevel::INFO, "界面初始化2....");
    m_mainPageIn->logAppend(EM_LogLevel::INFO, "数据库连接初始化完成2...");

    m_mainPageIn->setFocusPolicy(Qt::StrongFocus);
    m_mainPageIn->setFocus();
}

void MainWindow::initMtcOut()
{
    m_mainPageOut = new MtcOutPage(this);

    addPageNode("主页面", m_mainPageOut, ElaIconType::House);

    m_mainPageOut->enableSptShiftInfoShow(true);

    m_mainPageOut->initUi();

    m_mainPageOut->setStationInfo("福州西(6701)(港口站)");
    m_mainPageOut->setLaneID(12);
    m_mainPageOut->setUserInfo("测试员(3501911)");
    m_mainPageOut->setShiftInfo("2025-12-25晚班");
    m_mainPageOut->setModeText("正常过车流程");

    m_mainPageOut->setVirtualGantryInfo("福州西B向门架(34090.H)");
    m_mainPageOut->setAppVer("v1.0.1");
    m_mainPageOut->setFeeRateVer("1908");
    m_mainPageOut->setFullBlackVer("20250801");
    m_mainPageOut->setPartBlackVer("202508111211");

    m_mainPageOut->setPrevImage(QImage(":/static/images/prev_pic.jpg"));

    m_mainPageOut->setStartTicketNum(63909944);
    m_mainPageOut->setCurTicketNum(63909946);
    m_mainPageOut->setNormalTicketCnt(102);
    m_mainPageOut->setScrapTicketCnt(0);
    m_mainPageOut->setRestTicketCnt(644);
    m_mainPageOut->setTotalVehCnt(879);
    m_mainPageOut->setRecycleCardCnt(857);
    m_mainPageOut->setEtcCardCnt(19);
    m_mainPageOut->setFleetVehCnt(0);
    m_mainPageOut->setThirdPayCnt(9858);
    m_mainPageOut->setUpStartTicketNum(63909944);
    m_mainPageOut->setUpCurTicketNum(63909946);
    m_mainPageOut->setUpRestTicketCnt(234);
    m_mainPageOut->setUpRecycleCardCnt(12);
    m_mainPageOut->setUpScrapTicketCnt(21);
    m_mainPageOut->setUpWriteErrCnt(75);
    m_mainPageOut->setDownStartTicketNum(63909944);
    m_mainPageOut->setDownCurTicketNum(63909946);
    m_mainPageOut->setDownRestTicketCnt(97);
    m_mainPageOut->setDownRecycleCardCnt(76);
    m_mainPageOut->setDownScrapTicketCnt(97);
    m_mainPageOut->setDownWriteErrCnt(76);

    m_mainPageOut->logAppend(EM_LogLevel::INFO, "界面初始化....");
    m_mainPageOut->logAppend(EM_LogLevel::INFO, "数据库连接初始化完成1...");
    m_mainPageOut->logAppend(EM_LogLevel::INFO, "界面初始化....");
    m_mainPageOut->logAppend(EM_LogLevel::INFO, "数据库连接初始化完成2...");
    m_mainPageOut->logAppend(EM_LogLevel::INFO, "界面初始化....");
    m_mainPageOut->logAppend(EM_LogLevel::INFO, "数据库连接初始化完成3...");
    m_mainPageOut->logAppend(EM_LogLevel::INFO, "界面初始化....");
    m_mainPageOut->logAppend(EM_LogLevel::INFO, "数据库连接初始化完成4...");

    m_mainPageOut->setScrollTip("欢迎使用福建省高速公路收费软件（如遇到软件问题请致电福建省高速公路信息科技有限公司）");

    m_mainPageOut->setPlate("拼闽B8L7222");
    m_mainPageOut->setVehClass("货四");
    m_mainPageOut->setVehStatus("抢险救灾车");
    m_mainPageOut->setSituation("超限");
    m_mainPageOut->setCardType("CPC卡");
    m_mainPageOut->setCardNum("35012211520060004842");
    m_mainPageOut->setEnStationName("福建莆田西站");
    m_mainPageOut->setEnTime("2025-07-12 12:00:00");
    m_mainPageOut->setWeightInfo("2轴 114 12.5吨");
    m_mainPageOut->setEnPlate("蓝闽B8L722");
    m_mainPageOut->setLabel1("福建莆田涵江萩芦镇");
    m_mainPageOut->setSplitProvincesInfo("福建100元;广东12元;武汉234元;福建100元;广东12元;武汉234元;");

    m_mainPageOut->setTradeHint("自动发卡,等待过车");
    m_mainPageOut->setObuHint("大件运输车; 蓝闽B8L722; 普通车; 专二; OBUSN: 35011603330999998340 12轴型 2轴 10.00吨 限重18.00吨 超限0.00%");
    m_mainPageOut->appendHintButton("第三方支付", Color::BUTTON_TC, Color::CONFIRM_BUTTON_BG);
    m_mainPageOut->appendHintButton("第三方支付", Color::BUTTON_TC, Color::INFO_BUTTON_BG);
    m_mainPageOut->appendHintButton("第三方支付", Color::BUTTON_TC, Color::WARN_BUTTON_BG);

    m_mainPageOut->setWeightLow(true);
    m_mainPageOut->setCurWeightInfo("12轴型 2轴 10.00吨 限重18.00吨 超限0.00%");
    m_mainPageOut->setCurWeightInfoCount(1);
    ST_WeightInfoItem item1;
    item1.plate = "闽A12345";
    item1.axisType = 1127;
    item1.axisNum = 6;
    item1.tollWeight = 25.74;
    item1.status = 1;
    m_mainPageOut->appendWeightInfoItem(item1);
    ST_WeightInfoItem item2;
    item2.plate = "闽B100000";
    item2.axisType = 1127;
    item2.axisNum = 6;
    item2.tollWeight = 25.74;
    item2.status = 0;
    m_mainPageOut->appendWeightInfoItem(item2);
    ST_WeightInfoItem item3;
    item3.plate = "闽BA50430";
    item3.axisType = 125;
    item3.axisNum = 4;
    item3.tollWeight = 22.74;
    item3.status = 0;
    m_mainPageOut->appendWeightInfoItem(item3);
    ST_WeightInfoItem item4;
    item4.plate = "闽BA50430";
    item4.axisType = 125;
    item4.axisNum = 4;
    item4.tollWeight = 22.74;
    item4.status = 0;
    m_mainPageOut->appendWeightInfoItem(item4);

    m_mainPageOut->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "1321.2/电子支付", "35012042230602103792"});
    m_mainPageOut->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "1323.2/第三方支付", "35012042230602103792"});
    m_mainPageOut->appendTradeItem({"闽A12345F", "货一", "07-12 12:00:04", "234/现金支付", "35012042230602103792"});
    m_mainPageOut->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "1234.5/第三方支付", "35012042230602103792"});
    m_mainPageOut->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "4/现金支付", "35012042230602103792"});

    m_mainPageOut->setDeviceList({EM_DeviceIcon::WEIGHT, EM_DeviceIcon::FIRST_COIL, EM_DeviceIcon::CELLING_LAMP, EM_DeviceIcon::CAP_COIL,
                                  EM_DeviceIcon::CAPTURE, EM_DeviceIcon::RAILLING_COIL, EM_DeviceIcon::PASSING_LAMP, EM_DeviceIcon::RAILING_MACHINE,
                                  EM_DeviceIcon::RSU});

    m_mainPageOut->setFocusPolicy(Qt::StrongFocus);
    m_mainPageOut->setFocus();
}

void MainWindow::initEtc()
{
    m_mainPageEtc = new EtcPage(this);

    addPageNode("主页面", m_mainPageEtc, ElaIconType::House);

    m_mainPageEtc->initUi();

    m_mainPageEtc->setStationInfo("福州西(6701)(港口站)");
    m_mainPageEtc->setLaneID(12);
    m_mainPageEtc->setUserInfo("测试员(3501911)");
    m_mainPageEtc->setShiftInfo("2025-12-25晚班");
    m_mainPageEtc->setModeText("ETC入口");

    m_mainPageEtc->setVirtualGantryInfo("福州西B向门架(34090.H)");
    m_mainPageEtc->setAppVer("v1.0.1");
    m_mainPageEtc->setFeeRateVer("1908");
    m_mainPageEtc->setFullBlackVer("20250801");
    m_mainPageEtc->setPartBlackVer("202508111211");

    m_mainPageEtc->logAppend(EM_LogLevel::INFO, "界面初始化....");
    m_mainPageEtc->logAppend(EM_LogLevel::INFO, "数据库连接初始化完成1...");
    m_mainPageEtc->logAppend(EM_LogLevel::INFO, "界面初始化....");
    m_mainPageEtc->logAppend(EM_LogLevel::INFO, "数据库连接初始化完成2...");
    m_mainPageEtc->logAppend(EM_LogLevel::INFO, "界面初始化....");
    m_mainPageEtc->logAppend(EM_LogLevel::INFO, "数据库连接初始化完成3...");
    m_mainPageEtc->logAppend(EM_LogLevel::INFO, "界面初始化....");
    m_mainPageEtc->logAppend(EM_LogLevel::INFO, "数据库连接初始化完成4...");

    m_mainPageEtc->setScrollTip("欢迎使用福建省高速公路收费软件（如遇到软件问题请致电福建省高速公路信息科技有限公司）");

    m_mainPageEtc->setPlate("拼闽B8L7222");
    m_mainPageEtc->setVehClass("货四");
    m_mainPageEtc->setVehStatus("抢险救灾车");
    m_mainPageEtc->setSituation("无电子标签");

    m_mainPageEtc->setCardType("CPC卡");
    m_mainPageEtc->setTradeTime("16:55:20");
    m_mainPageEtc->setEnStationName("莆田西站");
    m_mainPageEtc->setProvince("福建");
    m_mainPageEtc->setToll("入口");
    m_mainPageEtc->setBalance("1234");

    m_mainPageEtc->appendTradeItem({"闽A12345", "货一", "12:00:04", "祥谦", "CPC卡", "入口", "35012042230602103792"});
    m_mainPageEtc->setTradeHint("等待过车");

    m_mainPageEtc->setDeviceList({EM_DeviceIcon::WEIGHT, EM_DeviceIcon::FIRST_COIL, EM_DeviceIcon::CELLING_LAMP, EM_DeviceIcon::CAP_COIL,
                                  EM_DeviceIcon::CAPTURE, EM_DeviceIcon::RAILLING_COIL, EM_DeviceIcon::PASSING_LAMP, EM_DeviceIcon::RAILING_MACHINE,
                                  EM_DeviceIcon::RSU});

    m_mainPageEtc->setFocusPolicy(Qt::StrongFocus);
    m_mainPageEtc->setFocus();
}
