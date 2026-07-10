#include "mtcinpage.h"

#include <QHBoxLayout>
#include <QTableView>

#include "components/devicepanel.h"
#include "components/pagearea.h"
#include "components/recenttradepanel.h"
#include "global/uiconst.h"
#include "utils/uiutils.h"

using namespace Utils;

MtcInPage::MtcInPage(QWidget *parent)
    : BasePage{parent}
{}

MtcInPage::~MtcInPage() {}

void MtcInPage::setTotalCardCnt(uint cnt)
{
    if (!m_totalCardCnt)
        return;
    m_totalCardCnt->setText(QString("%1").arg(cnt));
}

void MtcInPage::setNoFlagCardCnt(uint cnt)
{
    if (!m_noFlagCardCnt)
        return;
    m_noFlagCardCnt->setText(QString("%1").arg(cnt));
}

void MtcInPage::setPeccanyVehCnt(uint cnt)
{
    if (!m_peccanyVehCnt)
        return;
    m_peccanyVehCnt->setText(QString("%1").arg(cnt));
}

void MtcInPage::setFleetVehCnt(uint cnt)
{
    if (!m_fleetVehCnt)
        return;
    m_fleetVehCnt->setText(QString("%1").arg(cnt));
}

void MtcInPage::setCpcCardCnt(uint cnt)
{
    if (!m_cpcCardCnt)
        return;
    m_cpcCardCnt->setText(QString("%1").arg(cnt));
}

void MtcInPage::setEtcCardCnt(uint cnt)
{
    if (!m_etcCardCnt)
        return;
    m_etcCardCnt->setText(QString("%1").arg(cnt));
}

void MtcInPage::setBadCardCnt(uint cnt)
{
    if (!m_badCardCnt)
        return;
    m_badCardCnt->setText(QString("%1").arg(cnt));
}

void MtcInPage::setPaperCardCnt(uint cnt)
{
    if (!m_paperCardCnt)
        return;
    m_paperCardCnt->setText(QString("%1").arg(cnt));
}

void MtcInPage::setHolidayFreeVehCnt(uint cnt)
{
    if (!m_holidayFreeVehCnt)
        return;
    m_holidayFreeVehCnt->setText(QString("%1").arg(cnt));
}

void MtcInPage::setRestTickCnt(uint cnt)
{
    if (!m_restTicketCnt)
        return;
    m_restTicketCnt->setText(QString("%1").arg(cnt));
}

void MtcInPage::setStartTicketNum(uint num)
{
    if (!m_startTicketNum)
        return;

    QFont font = m_startTicketNum->font();
    if (num > 9999) {
        font.setPixelSize(16);
    } else {
        font.setPixelSize(22);
    }
    m_startTicketNum->setFont(font);
    m_startTicketNum->setText(QString("%1").arg(num));
}

void MtcInPage::setEndTicketNum(uint num)
{
    if (!m_endTicketNum)
        return;

    QFont font = m_endTicketNum->font();
    if (num > 9999) {
        font.setPixelSize(16);
    } else {
        font.setPixelSize(22);
    }
    m_endTicketNum->setFont(font);
    m_endTicketNum->setText(QString("%1").arg(num));
}

void MtcInPage::setCurTicketNum(uint num)
{
    if (!m_curTicketNum)
        return;

    QFont font = m_curTicketNum->font();
    if (num > 9999) {
        font.setPixelSize(16);
    } else {
        font.setPixelSize(22);
    }
    m_curTicketNum->setFont(font);
    m_curTicketNum->setText(QString("%1").arg(num));
}

void MtcInPage::setCardType(const QString &cardType)
{
    if (!m_cardType)
        return;
    m_cardType->setText(cardType);
}

void MtcInPage::setCardNum(const QString &cardNum)
{
    if (!m_cardNum)
        return;
    m_cardNum->setText(cardNum);
}

void MtcInPage::setBalance(const QString &balance)
{
    if (!m_balance)
        return;
    m_balance->setText(balance);
}

void MtcInPage::setEnTime(const QString &enTime)
{
    if (!m_enTime)
        return;
    m_enTime->setText(enTime);
}

void MtcInPage::setEnStationName(const QString &enStationName)
{
    if (!m_enStationName)
        return;
    m_enStationName->setText(enStationName);
}

void MtcInPage::setCardStatus(const QString &cardStatus)
{
    if (!m_cardStatus)
        return;
    m_cardStatus->setText(cardStatus);
}

void MtcInPage::setRobotStatus1(bool isOk, ushort restCard)
{
    if (!m_robotStatus1)
        return;

    QString styleStr = QString("background-color: %1; color: %2; border: none; border-radius: 4px;");
    m_robotStatus1->setText(QString::number(restCard));
    if (!isOk || restCard < 10) {
        m_robotStatus1->setStyleSheet(styleStr.arg(Color::WARN_BUTTON_BG, Color::BUTTON_TC));
    } else {
        m_robotStatus1->setStyleSheet(styleStr.arg(Color::INFO_BUTTON_BG, Color::BUTTON_TC));
    }
}

void MtcInPage::setRobotStatus2(bool isOk, ushort restCard)
{
    if (!m_robotStatus2)
        return;

    QString styleStr = QString("background-color: %1; color: %2; border: none; border-radius: 4px;");
    m_robotStatus2->setText(QString::number(restCard));
    if (!isOk || restCard < 10) {
        m_robotStatus2->setStyleSheet(styleStr.arg(Color::WARN_BUTTON_BG, Color::BUTTON_TC));
    } else {
        m_robotStatus2->setStyleSheet(styleStr.arg(Color::INFO_BUTTON_BG, Color::BUTTON_TC));
    }
}

void MtcInPage::setRobotStatus3(bool isOk, ushort restCard)
{
    if (!m_robotStatus3)
        return;

    QString styleStr = QString("background-color: %1; color: %2; border: none; border-radius: 4px;");
    m_robotStatus3->setText(QString::number(restCard));
    if (!isOk || restCard < 10) {
        m_robotStatus3->setStyleSheet(styleStr.arg(Color::WARN_BUTTON_BG, Color::BUTTON_TC));
    } else {
        m_robotStatus3->setStyleSheet(styleStr.arg(Color::INFO_BUTTON_BG, Color::BUTTON_TC));
    }
}

void MtcInPage::setRobotStatus4(bool isOk, ushort restCard)
{
    if (!m_robotStatus4)
        return;

    QString styleStr = QString("background-color: %1; color: %2; border: none; border-radius: 4px;");
    m_robotStatus4->setText(QString::number(restCard));
    if (!isOk || restCard < 10) {
        m_robotStatus4->setStyleSheet(styleStr.arg(Color::WARN_BUTTON_BG, Color::BUTTON_TC));
    } else {
        m_robotStatus4->setStyleSheet(styleStr.arg(Color::INFO_BUTTON_BG, Color::BUTTON_TC));
    }
}

void MtcInPage::initLeftUi()
{
    // 抓拍显示区域
    m_displayArea = initDisplayArea();
    m_displayArea->setParent(this);

    // 车道状态显示区域
    m_laneStatusArea = initLaneStatusArea();
    m_laneStatusArea->setParent(this);

    QWidget *shiftAndLogArea = new QWidget(this);
    shiftAndLogArea->setMinimumHeight(304);

    // 工班信息显示区域
    m_shiftInfoArea = initShiftInfoArea();
    m_shiftInfoArea->setParent(shiftAndLogArea);

    // 日志显示区域
    m_logArea = initLogBrowseArea();
    m_logArea->setParent(shiftAndLogArea);

    QVBoxLayout *shiftAndLogVLayout = new QVBoxLayout(shiftAndLogArea);
    shiftAndLogVLayout->setContentsMargins(0, 0, 0, 0);
    shiftAndLogVLayout->setSpacing(8);
    shiftAndLogVLayout->addWidget(m_shiftInfoArea);
    shiftAndLogVLayout->addWidget(m_logArea, 1);

    // 左侧主布局
    m_leftWidget = new QWidget(this);
    QVBoxLayout *leftVLayout = new QVBoxLayout(m_leftWidget);
    leftVLayout->setContentsMargins(0, 0, 0, 0);
    leftVLayout->setSpacing(8);

    leftVLayout->addWidget(m_displayArea, m_displayArea->minimumHeight());
    leftVLayout->addWidget(m_laneStatusArea, m_laneStatusArea->minimumHeight());
    leftVLayout->addWidget(shiftAndLogArea, shiftAndLogArea->minimumHeight());
}

void MtcInPage::initRightUi()
{
    // 滚动提示显示区域
    m_scrollTipArea = initScrollTipArea();
    m_scrollTipArea->setParent(this);

    // 当前车辆与卡内信息显示区域
    m_vehInfoArea = initVehInfoArea();
    m_vehInfoArea->setParent(this);

    // 交易提示区域
    m_tradeHintArea = initTradeHintArea();
    m_tradeHintArea->setParent(this);

    // 称重信息显示区域
    m_weightInfoArea = initWeightInfoArea();
    m_weightInfoArea->setParent(this);

    // 近期交易记录查看区域
    m_recentTradePanel = initRecentTradeArea();
    m_recentTradePanel->setParent(this);

    // 卡机状态区域
    m_robotStatusArea = initRobotStatusArea();
    m_robotStatusArea->setParent(this);

    // 设备图标显示区域
    m_deviceIconPanel = initDeviceArea();
    m_deviceIconPanel->setParent(this);

    // 右侧主布局
    m_rightWidget = new QWidget(this);
    QVBoxLayout *rightVLayout = new QVBoxLayout(m_rightWidget);
    rightVLayout->setContentsMargins(0, 0, 0, 0);
    rightVLayout->setSpacing(8);

    rightVLayout->addWidget(m_scrollTipArea, m_scrollTipArea->minimumHeight());
    rightVLayout->addWidget(m_vehInfoArea, m_vehInfoArea->minimumHeight());
    rightVLayout->addWidget(m_tradeHintArea, m_tradeHintArea->minimumHeight());
    rightVLayout->addWidget(m_weightInfoArea);
    rightVLayout->addWidget(m_recentTradePanel);
    rightVLayout->addWidget(m_robotStatusArea);
    rightVLayout->addWidget(m_deviceIconPanel, m_deviceIconPanel->minimumHeight());
}

PageArea *MtcInPage::initShiftInfoArea()
{
    PageArea *shiftInfoArea = new PageArea();
    shiftInfoArea->setBorderRadius(8);
    shiftInfoArea->setMinimumHeight(156);

    QWidget *widget1 = createShiftInfoBlock("总过车", 15, m_totalVehCnt, 18, 70, 70, shiftInfoArea);
    QWidget *widget2 = createShiftInfoBlock("总发卡", 15, m_totalCardCnt, 18, 70, 70, shiftInfoArea);
    QWidget *widget3 = createShiftInfoBlock("错标卡", 15, m_noFlagCardCnt, 18, 70, 70, shiftInfoArea);
    QWidget *widget4 = createShiftInfoBlock("冲关车", 15, m_peccanyVehCnt, 18, 70, 70, shiftInfoArea);
    QWidget *widget5 = createShiftInfoBlock("车队车", 15, m_fleetVehCnt, 18, 70, 70, shiftInfoArea);
    QWidget *widget6 = createShiftInfoBlock("通行卡", 15, m_cpcCardCnt, 18, 70, 70, shiftInfoArea);
    QWidget *widget7 = createShiftInfoBlock("闽通卡", 15, m_etcCardCnt, 18, 70, 70, shiftInfoArea);
    QWidget *widget8 = createShiftInfoBlock("坏卡数", 15, m_badCardCnt, 18, 70, 70, shiftInfoArea);
    QWidget *widget9 = createShiftInfoBlock("纸券数", 15, m_paperCardCnt, 18, 70, 70, shiftInfoArea);
    QWidget *widget10 = createShiftInfoBlock("动免车", 15, m_holidayFreeVehCnt, 18, 70, 70, shiftInfoArea);

    // 纸券信息区域
    m_paperInfoArea = new PageArea(shiftInfoArea);
    m_paperInfoArea->setBorderRadius(4);
    m_paperInfoArea->setFixedHeight(70);
    m_paperInfoArea->setIsUseBorder(true);
    m_paperInfoArea->setBackgroundColor(QColor(Color::INSIDE_AREA_BG));

    QWidget *widget11 = createShiftInfoBlock("剩余券数", 15, m_restTicketCnt, 18, 85, 70, shiftInfoArea);
    QWidget *widget12 = createShiftInfoBlock("起始券号", 15, m_startTicketNum, 18, 85, 70, shiftInfoArea);
    QWidget *widget13 = createShiftInfoBlock("结束券号", 15, m_endTicketNum, 18, 85, 70, shiftInfoArea);
    QWidget *widget14 = createShiftInfoBlock("当前券号", 15, m_curTicketNum, 18, 85, 70, shiftInfoArea);

    QHBoxLayout *paperInfoHLayout = new QHBoxLayout(m_paperInfoArea);
    paperInfoHLayout->setContentsMargins(0, 0, 0, 0);
    paperInfoHLayout->setSpacing(0);
    paperInfoHLayout->addWidget(widget11);
    paperInfoHLayout->addWidget(widget12);
    paperInfoHLayout->addWidget(widget13);
    paperInfoHLayout->addWidget(widget14);

    QGridLayout *shiftInfoAreaLayout = new QGridLayout(shiftInfoArea);
    shiftInfoAreaLayout->setContentsMargins(8, 8, 8, 8);
    shiftInfoAreaLayout->setSpacing(0);
    shiftInfoAreaLayout->addWidget(widget1, 0, 0);
    shiftInfoAreaLayout->addWidget(widget2, 0, 1);
    shiftInfoAreaLayout->addWidget(widget3, 0, 2);
    shiftInfoAreaLayout->addWidget(widget4, 0, 3);
    shiftInfoAreaLayout->addWidget(widget5, 0, 4);
    shiftInfoAreaLayout->addWidget(widget6, 1, 0);
    shiftInfoAreaLayout->addWidget(widget7, 1, 1);
    shiftInfoAreaLayout->addWidget(widget8, 1, 2);
    shiftInfoAreaLayout->addWidget(widget9, 1, 3);
    shiftInfoAreaLayout->addWidget(widget10, 1, 4);
    shiftInfoAreaLayout->addWidget(m_paperInfoArea, 2, 0, 1, 5);

    return shiftInfoArea;
}

PageArea *MtcInPage::initVehInfoArea()
{
    PageArea *vehInfoArea = new PageArea();
    vehInfoArea->setBorderRadius(8);
    vehInfoArea->setMinimumHeight(138);

    // 车辆信息区：抓拍车牌，车型，车种，特情
    QWidget *carInfoWidget = new QWidget(vehInfoArea);
    carInfoWidget->setFixedHeight(40);
    carInfoWidget->setStyleSheet("background-color: white;");

    m_plate = new QLabel(carInfoWidget);
    m_plate->setMinimumWidth(160);
    m_vehClass = new QLabel(carInfoWidget);
    m_vehClass->setMinimumWidth(70);
    m_vehStatus = new QLabel(carInfoWidget);
    m_vehStatus->setMinimumWidth(133);
    m_situation = new QLabel(carInfoWidget);
    m_situation->setMinimumWidth(235);

    QList<QLabel *> carTexts = {m_plate, m_vehClass, m_vehStatus, m_situation};
    for (auto *t : carTexts) {
        t->setAlignment(Qt::AlignCenter);
        t->setWordWrap(false);
        QFont font = t->font();
        font.setPixelSize(20);
        font.setWeight(QFont::Bold);
        t->setFont(font);
    }

    QHBoxLayout *carInfoHLayout = new QHBoxLayout(carInfoWidget);
    carInfoHLayout->setContentsMargins(0, 0, 0, 0);
    carInfoHLayout->setSpacing(0);
    carInfoHLayout->addWidget(m_plate, m_plate->minimumWidth());
    UiUtils::addLine(carInfoHLayout, 2, 20);
    carInfoHLayout->addWidget(m_vehClass, m_vehClass->minimumWidth());
    UiUtils::addLine(carInfoHLayout, 2, 20);
    carInfoHLayout->addWidget(m_vehStatus, m_vehStatus->minimumWidth());
    UiUtils::addLine(carInfoHLayout, 2, 20);
    carInfoHLayout->addWidget(m_situation, m_situation->minimumWidth());

    // 卡内信息区
    QWidget *cardInfoWidget = new QWidget(vehInfoArea);
    cardInfoWidget->setMinimumHeight(74);

    QLabel *label1 = new QLabel("卡类型", cardInfoWidget);
    QLabel *label2 = new QLabel("卡内余额", cardInfoWidget);
    QLabel *label3 = new QLabel("状态", cardInfoWidget);
    QLabel *label4 = new QLabel("卡号", cardInfoWidget);
    QLabel *label5 = new QLabel("入口站名", cardInfoWidget);
    QLabel *label6 = new QLabel("入口时间", cardInfoWidget);
    QList<QLabel *> labels = {label1, label2, label3, label4, label5, label6};
    for (auto *l : labels) {
        l->setStyleSheet(QString("color: %1;").arg(Color::INFO_TC));
        l->setWordWrap(false);
        QFont font = l->font();
        font.setWeight(QFont::DemiBold);
        font.setPixelSize(16);
        l->setFont(font);
    }

    m_cardType = new QLabel(cardInfoWidget);
    m_cardNum = new QLabel(cardInfoWidget);
    m_balance = new QLabel(cardInfoWidget);
    m_enTime = new QLabel(cardInfoWidget);
    m_enStationName = new QLabel(cardInfoWidget);
    m_cardStatus = new QLabel(cardInfoWidget);
    QList<QLabel *> cardTexts = {m_cardType, m_cardNum, m_balance, m_enTime, m_enStationName, m_cardStatus};
    for (auto *t : cardTexts) {
        QFont font = t->font();
        font.setPixelSize(16);
        font.setWeight(QFont::DemiBold);
        t->setFont(font);
        t->setWordWrap(false);
    }

    QGridLayout *cardInfoGLayout = new QGridLayout(cardInfoWidget);
    cardInfoGLayout->setContentsMargins(0, 0, 0, 0);
    cardInfoGLayout->setHorizontalSpacing(8);
    cardInfoGLayout->setVerticalSpacing(4);

    // 设置标签列最小化，值列弹性拉伸
    cardInfoGLayout->setColumnStretch(0, 0);
    cardInfoGLayout->setColumnStretch(2, 0);
    cardInfoGLayout->setColumnStretch(1, 1);
    cardInfoGLayout->setColumnStretch(3, 2);

    // 第一行：卡类型、卡号
    cardInfoGLayout->addWidget(label1, 0, 0);
    cardInfoGLayout->addWidget(m_cardType, 0, 1);
    cardInfoGLayout->addWidget(label4, 0, 2);
    cardInfoGLayout->addWidget(m_cardNum, 0, 3);

    // 第二行：卡内余额、入口站名
    cardInfoGLayout->addWidget(label2, 1, 0);
    cardInfoGLayout->addWidget(m_balance, 1, 1);
    cardInfoGLayout->addWidget(label5, 1, 2);
    cardInfoGLayout->addWidget(m_enStationName, 1, 3);

    // 第三行：状态，入口时间
    cardInfoGLayout->addWidget(label3, 2, 0);
    cardInfoGLayout->addWidget(m_cardStatus, 2, 1);
    cardInfoGLayout->addWidget(label6, 2, 2);
    cardInfoGLayout->addWidget(m_enTime, 2, 3);

    QVBoxLayout *vehInfoAreaLayout = new QVBoxLayout(vehInfoArea);
    vehInfoAreaLayout->setSpacing(8);
    vehInfoAreaLayout->setContentsMargins(8, 8, 8, 8);
    vehInfoAreaLayout->addWidget(carInfoWidget);
    vehInfoAreaLayout->addWidget(cardInfoWidget);

    return vehInfoArea;
}

RecentTradePanel *MtcInPage::initRecentTradeArea()
{
    // 交易记录表格
    QStringList header = {"车牌", "车型", "入口时间", "卡类型", "卡号"};
    RecentTradePanel *recentTradePanel = new RecentTradePanel(header);
    recentTradePanel->setMinimumHeight(102);
    QTableView *view = recentTradePanel->getRecentTradeView();
    view->setColumnWidth(0, 100);
    view->setColumnWidth(1, 45);
    view->setColumnWidth(2, 120);
    view->setColumnWidth(3, 80);

    return recentTradePanel;
}

QWidget *MtcInPage::initRobotStatusArea()
{
    QWidget *robotStatusWidget = new QWidget();
    robotStatusWidget->setFixedHeight(20);
    UiUtils::disableMouseEvents(robotStatusWidget);

    QLabel *label1 = new QLabel("●上工位1状态");
    QLabel *label2 = new QLabel("●上工位2状态");
    QLabel *label3 = new QLabel("●下工位1状态");
    QLabel *label4 = new QLabel("●下工位2状态");
    QList<QLabel *> labels = {label1, label2, label3, label4};
    for (auto *l : labels) {
        l->setStyleSheet(QString("color: %1;").arg(Color::INFO_TC));
        QFont font = l->font();
        font.setPixelSize(14);
        font.setWeight(QFont::DemiBold);
        l->setFont(font);
    }

    m_robotStatus1 = new QPushButton("0", robotStatusWidget);
    m_robotStatus2 = new QPushButton("0", robotStatusWidget);
    m_robotStatus3 = new QPushButton("0", robotStatusWidget);
    m_robotStatus4 = new QPushButton("0", robotStatusWidget);
    QList<QPushButton *> robotStatusButtons = {m_robotStatus1, m_robotStatus2, m_robotStatus3, m_robotStatus4};
    for (auto *rcb : robotStatusButtons) {
        rcb->setFixedSize(45, 16);
        rcb->setStyleSheet(QString("background-color: %1; color: %2; border: none; border-radius: 4px;").arg(Color::WARN_BUTTON_BG, Color::BUTTON_TC));
        QFont font = rcb->font();
        font.setPixelSize(14);
        rcb->setFont(font);
    }

    QHBoxLayout *statusHLayout = new QHBoxLayout(robotStatusWidget);
    statusHLayout->setContentsMargins(8, 2, 8, 2);
    statusHLayout->setSpacing(5);
    statusHLayout->addWidget(label1);
    statusHLayout->addWidget(m_robotStatus1);
    statusHLayout->addStretch();
    statusHLayout->addWidget(label2);
    statusHLayout->addWidget(m_robotStatus2);
    statusHLayout->addStretch();
    statusHLayout->addWidget(label3);
    statusHLayout->addWidget(m_robotStatus3);
    statusHLayout->addStretch();
    statusHLayout->addWidget(label4);
    statusHLayout->addWidget(m_robotStatus4);

    return robotStatusWidget;
}
