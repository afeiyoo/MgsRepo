#include "etcpage.h"

#include <QVBoxLayout>

#include "components/devicepanel.h"
#include "components/recenttradepanel.h"
#include "global/uiconst.h"
#include "utils/uiutils.h"

using namespace Utils;

EtcPage::EtcPage(QWidget *parent)
    : BasePage{parent}
{}

EtcPage::~EtcPage() {}

void EtcPage::setTotalVehCnt(int cnt)
{
    if (!m_totalVehCnt)
        return;
    m_totalVehCnt->setText(QString("%1").arg(cnt));
}

void EtcPage::setNormalVehCnt(int cnt)
{
    if (!m_normalVehCnt)
        return;
    m_normalVehCnt->setText(QString("%1").arg(cnt));
}

void EtcPage::setFreeVehCnt(int cnt)
{
    if (!m_freeVehCnt)
        return;
    m_freeVehCnt->setText(QString("%1").arg(cnt));
}

void EtcPage::setTotalToll(qreal toll)
{
    if (!m_totalToll)
        return;
    m_totalToll->setText(QString("%1").arg(toll));
}

void EtcPage::setCreditCardCnt(int cnt)
{
    if (!m_creditCardCnt)
        return;
    m_creditCardCnt->setText(QString("%1").arg(cnt));
}

void EtcPage::setPrePayCardCnt(int cnt)
{
    if (!m_prePayCardCnt)
        return;
    m_prePayCardCnt->setText(QString("%1").arg(cnt));
}

void EtcPage::setHolidayFreeVehCnt(int cnt)
{
    if (!m_holidayFreeVehCnt)
        return;
    m_holidayFreeVehCnt->setText(QString("%1").arg(cnt));
}

void EtcPage::setPeccanyVehCnt(int cnt)
{
    if (!m_peccanyVehCnt)
        return;
    m_peccanyVehCnt->setText(QString("%1").arg(cnt));
}

void EtcPage::setLastShiftTotalVehCnt(int cnt)
{
    if (!m_lastShiftTotalVehCnt)
        return;
    m_lastShiftTotalVehCnt->setText(QString("%1").arg(cnt));
}

void EtcPage::setCardType(const QString &cardType)
{
    if (!m_cardType)
        return;
    m_cardType->setText(cardType);
}

void EtcPage::setProvince(const QString &province)
{
    if (!m_province)
        return;
    m_province->setText(province);
}

void EtcPage::setBalance(const QString &balance)
{
    if (!m_balance)
        return;
    m_balance->setText(balance);
}

void EtcPage::setTradeTime(const QString &time)
{
    if (!m_tradeTime)
        return;
    m_tradeTime->setText(time);
}

void EtcPage::setEnStationName(const QString &enStationName)
{
    if (!m_enStationName)
        return;
    m_enStationName->setText(enStationName);
}

void EtcPage::setToll(const QString &toll)
{
    if (!m_toll)
        return;
    m_toll->setText(toll);
}

void EtcPage::initLeftUi()
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

void EtcPage::initRightUi()
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

    // 近期交易记录查看区域
    m_recentTradePanel = initRecentTradeArea();
    m_recentTradePanel->setParent(this);

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
    rightVLayout->addWidget(m_recentTradePanel);
    rightVLayout->addWidget(m_deviceIconPanel, m_deviceIconPanel->minimumHeight());
}

QWidget *EtcPage::initLaneStatusArea()
{
    QWidget *laneStatusArea = new QWidget();
    laneStatusArea->setMinimumHeight(40);

    m_infoBoard = new QLabel("车道关闭", laneStatusArea);
    QList<QLabel *> texts = {m_infoBoard};
    for (auto *t : texts) {
        t->setStyleSheet(QString("color: %1").arg(Color::BUTTON_TC));
        t->setWordWrap(false);
        QFont font = t->font();
        font.setWeight(QFont::Bold);
        font.setPixelSize(24);
        t->setFont(font);
        t->setAlignment(Qt::AlignCenter);
    }

    PageArea *infoBoardArea = new PageArea(laneStatusArea);
    infoBoardArea->setBorderRadius(8);
    infoBoardArea->setMinimumHeight(40);
    infoBoardArea->setBackgroundColor(QColor(Color::WARN_BUTTON_BG));

    QHBoxLayout *tempHLayout1 = new QHBoxLayout(infoBoardArea);
    tempHLayout1->setContentsMargins(0, 0, 0, 0);
    tempHLayout1->addWidget(m_infoBoard);

    QHBoxLayout *laneStatusAreaLayout = new QHBoxLayout(laneStatusArea);
    laneStatusAreaLayout->setSpacing(8);
    laneStatusAreaLayout->setContentsMargins(0, 0, 0, 0);
    laneStatusAreaLayout->addWidget(infoBoardArea);

    return laneStatusArea;
}

PageArea *EtcPage::initShiftInfoArea()
{
    PageArea *shiftInfoArea = new PageArea();
    shiftInfoArea->setBorderRadius(8);
    shiftInfoArea->setMinimumHeight(156);

    QWidget *widget1 = createShiftInfoBlock("总车次", 15, m_totalVehCnt, 18, 70, 70, shiftInfoArea);
    QWidget *widget2 = createShiftInfoBlock("普通车", 15, m_normalVehCnt, 18, 70, 70, shiftInfoArea);
    QWidget *widget3 = createShiftInfoBlock("免征车", 15, m_freeVehCnt, 18, 70, 70, shiftInfoArea);
    QWidget *widget4 = createShiftInfoBlock("总金额", 15, m_totalToll, 18, 70, 70, shiftInfoArea);
    QWidget *widget5 = createShiftInfoBlock("记账卡", 15, m_creditCardCnt, 18, 70, 70, shiftInfoArea);
    QWidget *widget6 = createShiftInfoBlock("储值卡", 15, m_prePayCardCnt, 18, 70, 70, shiftInfoArea);
    QWidget *widget7 = createShiftInfoBlock("动免车", 15, m_holidayFreeVehCnt, 18, 70, 70, shiftInfoArea);
    QWidget *widget8 = createShiftInfoBlock("冲关车", 15, m_peccanyVehCnt, 18, 70, 70, shiftInfoArea);
    QWidget *widget9 = createShiftInfoBlock("上个班次总车次", 15, m_lastShiftTotalVehCnt, 18, 140, 70, shiftInfoArea);

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
    shiftInfoAreaLayout->addWidget(widget9, 1, 3, 1, 2);

    return shiftInfoArea;
}

PageArea *EtcPage::initVehInfoArea()
{
    PageArea *vehInfoArea = new PageArea();
    vehInfoArea->setBorderRadius(8);
    vehInfoArea->setMinimumHeight(140);

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
    QLabel *label3 = new QLabel("省份", cardInfoWidget);
    QLabel *label4 = new QLabel("收费", cardInfoWidget);
    QLabel *label5 = new QLabel("入口站名", cardInfoWidget);
    QLabel *label6 = new QLabel("交易时间", cardInfoWidget);
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
    m_province = new QLabel(cardInfoWidget);
    m_balance = new QLabel(cardInfoWidget);
    m_tradeTime = new QLabel(cardInfoWidget);
    m_enStationName = new QLabel(cardInfoWidget);
    m_toll = new QLabel(cardInfoWidget);
    QList<QLabel *> cardTexts = {m_cardType, m_province, m_balance, m_tradeTime, m_enStationName, m_toll};
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
    cardInfoGLayout->setColumnStretch(3, 1);

    // 第一行：卡类型、卡内余额
    cardInfoGLayout->addWidget(label1, 0, 0);
    cardInfoGLayout->addWidget(m_cardType, 0, 1);
    cardInfoGLayout->addWidget(label2, 0, 2);
    cardInfoGLayout->addWidget(m_balance, 0, 3);

    // 第二行：入口站名、交易时间
    cardInfoGLayout->addWidget(label5, 1, 0);
    cardInfoGLayout->addWidget(m_enStationName, 1, 1);
    cardInfoGLayout->addWidget(label6, 1, 2);
    cardInfoGLayout->addWidget(m_tradeTime, 1, 3);

    // 第三行：省份、收费
    cardInfoGLayout->addWidget(label3, 2, 0);
    cardInfoGLayout->addWidget(m_province, 2, 1);
    cardInfoGLayout->addWidget(label4, 2, 2);
    cardInfoGLayout->addWidget(m_toll, 2, 3);

    QVBoxLayout *vehInfoAreaLayout = new QVBoxLayout(vehInfoArea);
    vehInfoAreaLayout->setSpacing(8);
    vehInfoAreaLayout->setContentsMargins(8, 8, 8, 8);
    vehInfoAreaLayout->addWidget(carInfoWidget);
    vehInfoAreaLayout->addWidget(cardInfoWidget);

    return vehInfoArea;
}

RecentTradePanel *EtcPage::initRecentTradeArea()
{
    // 交易记录表格
    QStringList header = {"车牌", "车型", "时间", "入口", "卡类型", "收费", "卡号"};
    RecentTradePanel *recentTradePanel = new RecentTradePanel(header);
    recentTradePanel->setMinimumHeight(256);
    // 初始列表宽度
    QTableView *view = recentTradePanel->getRecentTradeView();
    view->setColumnWidth(0, 95);
    view->setColumnWidth(1, 40);
    view->setColumnWidth(2, 75);
    view->setColumnWidth(3, 100);
    view->setColumnWidth(4, 60);
    view->setColumnWidth(5, 60);

    return recentTradePanel;
}
