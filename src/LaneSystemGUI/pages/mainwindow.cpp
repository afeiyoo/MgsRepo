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

    m_mainPage->setFocusPolicy(Qt::StrongFocus);
    m_mainPage->setFocus();
}
