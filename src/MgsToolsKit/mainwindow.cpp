#include "mainwindow.h"

#include "ElaContentDialog.h"
#include "ElaStatusBar.h"
#include "ElaText.h"

#include "global/constant.h"
#include "pages/t_cardrobot.h"
#include "pages/t_deploytool.h"
#include "pages/t_deskprinter.h"
#include "pages/t_infoboard.h"
#include "pages/t_mobileplusterminal.h"
#include "pages/t_smartcontroller.h"
#include "pages/t_vehrecognizer.h"

MainWindow::MainWindow(QWidget *parent)
    : ElaWindow{parent}
{
    initWindow();

    initEdgeLayout();

    initContent();

    // 拦截默认关闭事件
    m_closeDialog = new ElaContentDialog(this);
    connect(m_closeDialog, &ElaContentDialog::rightButtonClicked, this, &MainWindow::close);
    connect(m_closeDialog, &ElaContentDialog::middleButtonClicked, this, [=]() {
        m_closeDialog->close();
        showMinimized();
    });
    this->setIsDefaultClosed(false);
    connect(this, &MainWindow::closeButtonClicked, this, [=]() { m_closeDialog->exec(); });

    // 移动到中心
    moveToCenter();
}

MainWindow::~MainWindow() {}

void MainWindow::initWindow()
{
    setFocusPolicy(Qt::StrongFocus);
    setWindowIcon(QIcon(Constant::APP::ICON_PATH));
    setWindowTitle("车道运维工具箱");
    setUserInfoCardVisible(false); // 用户信息卡不可见
    setNavigationBarWidth(200);
    setNavigationBarDisplayMode(ElaNavigationType::Compact);
    setWindowButtonFlag(ElaAppBarType::ThemeChangeButtonHint, false);
    setWindowButtonFlag(ElaAppBarType::RouteBackButtonHint, false);
    setWindowButtonFlag(ElaAppBarType::RouteForwardButtonHint, false);

    resize(780, 650);

    // 自定义AppBar菜单（TODO）
}

void MainWindow::initEdgeLayout()
{
    // 状态栏
    ElaStatusBar *statusBar = new ElaStatusBar(this);
    m_statusText = new ElaText("初始化成功！", this);
    m_statusText->setTextPixelSize(14);
    statusBar->addWidget(m_statusText);
    this->setStatusBar(statusBar);
}

void MainWindow::initContent()
{
    m_deskPrinterPage = new T_DeskPrinter(this);
    addPageNode("桌面打印机测试", m_deskPrinterPage, ElaIconType::Print);

    m_infoBoardPage = new T_InfoBoard(this);
    addPageNode("折叠情报板测试", m_infoBoardPage, ElaIconType::Clapperboard);

    m_smartControllerPage = new T_SmartController(this);
    addPageNode("智能网关测试", m_smartControllerPage, ElaIconType::NetworkWired);

    m_cardRobotPage = new T_CardRobot(this);
    addPageNode("自动发卡机模拟", m_cardRobotPage, ElaIconType::UserRobot);

    m_mobilePlusTerminalPage = new T_MobilePlusTerminal(this);
    addPageNode("手机+自助终端测试", m_mobilePlusTerminalPage, ElaIconType::MobileScreenButton);

    m_deployToolPage = new T_DeployTool(this);
    addPageNode("信创车道系统部署", m_deployToolPage, ElaIconType::FerrisWheel);

    m_vehRecognizerPage = new T_VehRecognizer(this);
    addPageNode("车型识别器测试", m_vehRecognizerPage, ElaIconType::Dinosaur);
}
