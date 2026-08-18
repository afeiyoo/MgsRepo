#include "mainwindow.h"

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

    m_mainPageIn->setFocusPolicy(Qt::StrongFocus);
    m_mainPageIn->setFocus();
}

void MainWindow::initMtcOut(bool isSptInfoShow)
{
    m_mainPageOut = new MtcOutPage(this);

    addPageNode("主页面", m_mainPageOut, ElaIconType::House);

    m_mainPageOut->enableSptShiftInfoShow(isSptInfoShow);
    m_mainPageOut->initUi();

    m_mainPageOut->setFocusPolicy(Qt::StrongFocus);
    m_mainPageOut->setFocus();
}

void MainWindow::initEtc()
{
    m_mainPageEtc = new EtcPage(this);

    addPageNode("主页面", m_mainPageEtc, ElaIconType::House);

    m_mainPageEtc->initUi();

    m_mainPageEtc->setFocusPolicy(Qt::StrongFocus);
    m_mainPageEtc->setFocus();
}

BasePage *MainWindow::mtcInPage() const
{
    return m_mainPageIn;
}

BasePage *MainWindow::mtcOutPage() const
{
    return m_mainPageOut;
}

BasePage *MainWindow::etcPage() const
{
    return m_mainPageEtc;
}
