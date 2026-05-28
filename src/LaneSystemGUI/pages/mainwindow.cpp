#include "mainwindow.h"

#include "Logger.h"
#include "pages/mtcinpage.h"

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
    m_mainPage = new MtcInPage(this);

    addPageNode("主页面", m_mainPage, ElaIconType::House);

    m_mainPage->initUi();

    m_mainPage->setStationInfo("福州西(6701)(港口站)");
    m_mainPage->setLaneID(12);
    m_mainPage->setUserInfo("测试员(3501911)");
    m_mainPage->setShiftInfo("2025-12-25晚班");
    m_mainPage->setModeText("混合自动发卡");

    m_mainPage->setVirtualGantryInfo("福州西B向门架(34090.H)");
    m_mainPage->setAppVer("v1.0.1");
    m_mainPage->setFeeRateVer("1908");
    m_mainPage->setFullBlackVer("20250801");
    m_mainPage->setPartBlackVer("202508111211");

    m_mainPage->setScrollTip("欢迎使用福建省高速公路收费软件（如遇到软件问题请致电福建省高速公路信息科技有限公司）");

    m_mainPage->setPlate("拼闽B8L7222");
    m_mainPage->setVehClass("货四");
    m_mainPage->setVehStatus("抢险救灾车");
    m_mainPage->setSituation("超限");

    m_mainPage->setCardType("CPC卡");
    m_mainPage->setCardNum("35012042230602103792");
    m_mainPage->setEnTime("2025-07-12 12:00:04");
    m_mainPage->setEnStationName("福建莆田西站");
    m_mainPage->setBalance("88.2元");
    m_mainPage->setCardStatus("正常");

    m_mainPage->setTradeHint("自动发卡,等待过车");
    m_mainPage->setObuHint("大件运输车; 蓝闽B8L722; 普通车; 专二; OBUSN: 35011603330999998340 12轴型 2轴 10.00吨 限重18.00吨 超限0.00%");

    m_mainPage->setWeightLow(true);
    m_mainPage->setCurWeightInfo("12轴型 2轴 10.00吨 限重18.00吨 超限0.00%");
    m_mainPage->setCurWeightInfoCount(1);
    ST_WeightInfoItem item1;
    item1.plate = "闽A12345";
    item1.axisType = 1127;
    item1.axisNum = 6;
    item1.tollWeight = 25.74;
    item1.status = 1;
    m_mainPage->appendWeightInfoItem(item1);
    ST_WeightInfoItem item2;
    item2.plate = "闽B100000";
    item2.axisType = 1127;
    item2.axisNum = 6;
    item2.tollWeight = 25.74;
    item2.status = 0;
    m_mainPage->appendWeightInfoItem(item2);
    ST_WeightInfoItem item3;
    item3.plate = "闽BA50430";
    item3.axisType = 125;
    item3.axisNum = 4;
    item3.tollWeight = 22.74;
    item3.status = 0;
    m_mainPage->appendWeightInfoItem(item3);
    ST_WeightInfoItem item4;
    item4.plate = "闽BA50430";
    item4.axisType = 125;
    item4.axisNum = 4;
    item4.tollWeight = 22.74;
    item4.status = 0;
    m_mainPage->appendWeightInfoItem(item4);

    m_mainPage->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "CPC卡", "35012042230602103792"});
    m_mainPage->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "纸券", "35012042230602103792"});
    m_mainPage->appendTradeItem({"闽A12345F", "货一", "07-12 12:00:04", "ETC卡", "35012042230602103792"});
    m_mainPage->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "CPC卡", "35012042230602103792"});
    m_mainPage->appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "无卡", ""});

    m_mainPage->setRobotStatus1(false, 200);
    m_mainPage->setRobotStatus2(true, 200);
    m_mainPage->setRobotStatus3(true, 400);
    m_mainPage->setRobotStatus4(true, 2);

    m_mainPage->setDeviceList({EM_DeviceIcon::WEIGHT, EM_DeviceIcon::FIRST_COIL, EM_DeviceIcon::CELLING_LAMP, EM_DeviceIcon::CAP_COIL,
                               EM_DeviceIcon::CAPTURE, EM_DeviceIcon::RAILLING_COIL, EM_DeviceIcon::PASSING_LAMP, EM_DeviceIcon::RAILING_MACHINE,
                               EM_DeviceIcon::RSU});

    m_mainPage->setTotalVehCnt(9999);
    m_mainPage->setTotalCardCnt(9999);
    m_mainPage->setNoFlagCardCnt(2);
    m_mainPage->setPeccanyVehCnt(3);
    m_mainPage->setFleetVehCnt(4);
    m_mainPage->setCpcCardCnt(5);
    m_mainPage->setEtcCardCnt(80);
    m_mainPage->setBadCardCnt(123);
    m_mainPage->setPaperCardCnt(0);
    m_mainPage->setHolidayFreeVehCnt(3);
    m_mainPage->setCurTicketNum(9999999);
    m_mainPage->setStartTicketNum(9999999);
    m_mainPage->setEndTicketNum(9999999);
    m_mainPage->setRestTickCnt(20);

    m_mainPage->logAppend(EM_LogLevel::INFO, "界面初始化....");
    m_mainPage->logAppend(EM_LogLevel::INFO, "数据库连接初始化完成...");

    m_mainPage->setFocusPolicy(Qt::StrongFocus);
    m_mainPage->setFocus();
}
