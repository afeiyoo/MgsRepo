#include "mainwindow.h"

#include <QHBoxLayout>

#include "ElaActionCommander.h"
#include "ElaContentDialog.h"
#include "ElaStatusBar.h"
#include "ElaText.h"
#include "ElaToolButton.h"

#include "global/constant.h"
#include "pages/t_cardrobot.h"
#include "pages/t_deskprinter.h"
#include "pages/t_infoboard.h"
#include "pages/t_smartcontroller.h"

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

    resize(780, 680);

    // 自定义AppBar菜单（TODO）

    // 堆栈独立自定义窗口
    QWidget *centralCustomWidget = new QWidget(this);
    QHBoxLayout *centralCustomWidgetLayout = new QHBoxLayout(centralCustomWidget);
    centralCustomWidgetLayout->setContentsMargins(13, 15, 9, 6);
    ElaToolButton *leftButton = new ElaToolButton(this);
    leftButton->setElaIcon(ElaIconType::AngleLeft);
    leftButton->setEnabled(false);
    connect(leftButton, &ElaToolButton::clicked, this, [=]() { ElaActionCommander::getInstance()->undoCommand("MgsToolsKitAction"); });
    ElaToolButton *rightButton = new ElaToolButton(this);
    rightButton->setElaIcon(ElaIconType::AngleRight);
    rightButton->setEnabled(false);
    connect(rightButton, &ElaToolButton::clicked, this, [=]() { ElaActionCommander::getInstance()->redoCommand("MgsToolsKitAction"); });
    connect(ElaActionCommander::getInstance(), &ElaActionCommander::commanderStateChanged, this,
            [=](const QString &domainName, ElaActionCommanderType::CommanderState state) {
                if (domainName != "MgsToolsKitAction") {
                    return;
                }
                switch (state) {
                case ElaActionCommanderType::UndoValid: {
                    leftButton->setEnabled(true);
                    break;
                }
                case ElaActionCommanderType::UndoInvalid: {
                    leftButton->setEnabled(false);
                    break;
                }
                case ElaActionCommanderType::RedoValid: {
                    rightButton->setEnabled(true);
                    break;
                }
                case ElaActionCommanderType::RedoInvalid: {
                    rightButton->setEnabled(false);
                    break;
                }
                }
            });
    m_windowSuggestBox = new ElaSuggestBox(this);
    m_windowSuggestBox->setFixedHeight(32);
    m_windowSuggestBox->setPlaceholderText("搜索页面关键字");
    connect(m_windowSuggestBox, &ElaSuggestBox::suggestionClicked, this,
            [=](const ElaSuggestBox::SuggestData &suggestData) { navigation(suggestData.getSuggestData().value("ElaPageKey").toString()); });

    centralCustomWidgetLayout->addWidget(leftButton);
    centralCustomWidgetLayout->addWidget(rightButton);
    centralCustomWidgetLayout->addWidget(m_windowSuggestBox);
    centralCustomWidgetLayout->addStretch();
    setCentralCustomWidget(centralCustomWidget);
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
    addPageNode("桌面打印机测试工具", m_deskPrinterPage, ElaIconType::Print);

    m_infoBoardPage = new T_InfoBoard(this);
    addPageNode("折叠情报板测试工具", m_infoBoardPage, ElaIconType::Clapperboard);

    m_smartControllerPage = new T_SmartController(this);
    addPageNode("智能网关测试工具", m_smartControllerPage, ElaIconType::NetworkWired);

    m_cardRobotPage = new T_CardRobot(this);
    addPageNode("自动发卡机测试工具", m_cardRobotPage, ElaIconType::UserRobot);
}
