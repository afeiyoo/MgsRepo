#include "mtcinpage.h"

#include <QHBoxLayout>
#include <QTableView>

#include "ElaWidgetTools/ElaImageCard.h"
#include "components/devicepanel.h"
#include "components/pagearea.h"
#include "components/recenttradepanel.h"
#include "global/uiconst.h"
#include "utils/uiutils.h"

#include <QDebug>

using namespace Utils;

MtcInPage::MtcInPage(QWidget *parent)
    : BasePage{parent}
{}

MtcInPage::~MtcInPage() {}

void MtcInPage::setPlate(const QString &plate)
{
    if (!m_plate)
        return;

    m_plate->setText(plate);

    QString bgImage;
    QColor textColor = Qt::white; // 默认字体颜色

    if (plate.startsWith("白")) {
        bgImage = Path::WHITE_PLATE;
        textColor = Qt::black;
    } else if (plate.startsWith("黑")) {
        bgImage = Path::BLACK_PLATE;
        textColor = Qt::white;
    } else if (plate.startsWith("黄")) {
        bgImage = Path::YELLOW_PLATE;
        textColor = Qt::black;
    } else if (plate.startsWith("绿")) {
        bgImage = Path::GREEN_PLATE;
        textColor = Qt::black;
    } else if (plate.startsWith("渐")) {
        bgImage = Path::GRADIENT_PLATE;
        textColor = Qt::black;
    } else if (plate.startsWith("拼")) {
        bgImage = Path::MIX_PLATE;
        textColor = Qt::black;
    } else if (plate.startsWith("蓝")) {
        bgImage = Path::BLUE_PLATE;
        textColor = Qt::white;
    } else {
        bgImage = Path::WHITE_PLATE;
        textColor = Qt::black;
    }

    if (!bgImage.isEmpty()) {
        m_plate->setStyleSheet(QString("border-image: url(%1) 0 0 0 0 stretch stretch; color: %2;").arg(bgImage, textColor.name()));
    } else {
        m_plate->setStyleSheet(""); // 没有匹配到颜色，清除样式
    }
}

void MtcInPage::setVehClass(const QString &vehClass)
{
    if (!m_vehClass)
        return;
    m_vehClass->setText(vehClass);
}

void MtcInPage::setVehStatus(const QString &vehStatus)
{
    if (!m_vehStatus)
        return;
    m_vehStatus->setText(vehStatus);
}

void MtcInPage::setSituation(const QString &situation)
{
    if (!m_situation)
        return;
    m_situation->setText(situation);
    m_situation->setStyleSheet(QString("color: %1").arg(Color::WARN_TC));
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

void MtcInPage::setWeightInfo(const QString &info)
{
    Q_UNUSED(info);
}

void MtcInPage::setEnPlate(const QString &plate)
{
    Q_UNUSED(plate);
}

void MtcInPage::setLabel1(const QString &info)
{
    Q_UNUSED(info);
}

void MtcInPage::setLabel2(const QString &info)
{
    Q_UNUSED(info);
}

void MtcInPage::setToll(const QString &toll)
{
    Q_UNUSED(toll);
}

void MtcInPage::setObuHint(const QString &obuHint, const QString &color)
{
    if (!m_obuHint)
        return;

    m_obuHint->setStyleSheet(QString("color: %1;").arg(color));
    m_obuHint->setText(obuHint);
}

void MtcInPage::appendHintButton(const QString &hint, const QString &fontColor, const QString &bgColor)
{
    Q_UNUSED(hint);
    Q_UNUSED(fontColor);
    Q_UNUSED(bgColor);
}

void MtcInPage::setDeviceList(const QList<uint> &devList)
{
    if (!m_deviceIconPanel)
        return;
    m_deviceIconPanel->setDeviceList(devList);
}

void MtcInPage::updateDeviceStatus(EM_DeviceIcon::DeviceIcon dev, uint status)
{
    if (!m_deviceIconPanel)
        return;
    m_deviceIconPanel->changeDevStatus(dev, status);
}

QWidget *MtcInPage::initDisplayArea()
{
    QWidget *displayArea = new QWidget();
    displayArea->setMinimumHeight(300);

    m_capImage = new ElaImageCard(displayArea);
    m_capImage->setBorderRadius(8);
    m_capImage->setIsPreserveAspectCrop(false);
    m_capImage->setCardImage(QImage(Path::CAP_AREA_BACKGROUND));

    QGridLayout *displayAreaLayout = new QGridLayout(displayArea);
    displayAreaLayout->setContentsMargins(0, 0, 0, 0);
    displayAreaLayout->addWidget(m_capImage, 0, 0);

    return displayArea;
}

PageArea *MtcInPage::initShiftInfoArea()
{
    PageArea *shiftInfoArea = new PageArea();
    shiftInfoArea->setBorderRadius(8);
    shiftInfoArea->setMinimumHeight(156);

    QWidget *widget1 = createShiftInfoBlock("总过车数", m_totalVehCnt, 70, 70, shiftInfoArea);
    QWidget *widget2 = createShiftInfoBlock("总发卡数", m_totalCardCnt, 70, 70, shiftInfoArea);
    QWidget *widget3 = createShiftInfoBlock("错标卡数", m_noFlagCardCnt, 70, 70, shiftInfoArea);
    QWidget *widget4 = createShiftInfoBlock("冲关车数", m_peccanyVehCnt, 70, 70, shiftInfoArea);
    QWidget *widget5 = createShiftInfoBlock("车队车数", m_fleetVehCnt, 70, 70, shiftInfoArea);
    QWidget *widget6 = createShiftInfoBlock("通行卡数", m_cpcCardCnt, 70, 70, shiftInfoArea);
    QWidget *widget7 = createShiftInfoBlock("闽通卡数", m_etcCardCnt, 70, 70, shiftInfoArea);
    QWidget *widget8 = createShiftInfoBlock("坏卡数", m_badCardCnt, 70, 70, shiftInfoArea);
    QWidget *widget9 = createShiftInfoBlock("纸性券数", m_paperCardCnt, 70, 70, shiftInfoArea);
    QWidget *widget10 = createShiftInfoBlock("动免车数", m_holidayFreeVehCnt, 70, 70, shiftInfoArea);

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

    return shiftInfoArea;
}

void MtcInPage::setTradeHint(const QString &tradeHint, const QString &color)
{
    if (!m_tradeHint)
        return;

    m_tradeHint->setStyleSheet(QString("color: %1;").arg(color));
    m_tradeHint->setText(tradeHint);
}

PageArea *MtcInPage::initVehInfoArea()
{
    PageArea *vehInfoArea = new PageArea();
    vehInfoArea->setBorderRadius(8);
    vehInfoArea->setMinimumHeight(146);

    // 车辆信息区：抓拍车牌，车型，车种，特情
    QWidget *carInfoWidget = new QWidget(vehInfoArea);
    carInfoWidget->setFixedHeight(40);
    carInfoWidget->setStyleSheet("background-color: white;");

    m_plate = new QLabel(carInfoWidget);
    m_plate->setMinimumWidth(160);
    m_vehClass = new QLabel(carInfoWidget);
    m_vehClass->setMinimumWidth(73);
    m_vehStatus = new QLabel(carInfoWidget);
    m_vehStatus->setMinimumWidth(133);
    m_situation = new QLabel(carInfoWidget);
    m_situation->setMinimumWidth(242);

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
    cardInfoWidget->setMinimumHeight(82);

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
        t->setFont(font);
        t->setWordWrap(false);
    }

    QGridLayout *cardInfoGLayout = new QGridLayout(cardInfoWidget);
    cardInfoGLayout->setContentsMargins(0, 0, 0, 0);
    cardInfoGLayout->setHorizontalSpacing(8);
    cardInfoGLayout->setVerticalSpacing(8);

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

PageArea *MtcInPage::initTradeHintArea()
{
    PageArea *tradeHintArea = new PageArea();
    tradeHintArea->setBorderRadius(8);
    tradeHintArea->setMinimumHeight(140);

    m_tradeHint = new QLabel(tradeHintArea);
    m_tradeHint->setWordWrap(true);
    QFont tradeHintfont = m_tradeHint->font();
    tradeHintfont.setPixelSize(30);
    tradeHintfont.setBold(true);
    m_tradeHint->setFont(tradeHintfont);

    m_obuHint = new QLabel(tradeHintArea);
    m_obuHint->setStyleSheet(QString("color: %1;").arg(Color::INFO_TC));
    m_obuHint->setWordWrap(true);
    QFont obuHintFont = m_obuHint->font();
    obuHintFont.setPixelSize(14);
    m_obuHint->setFont(obuHintFont);

    QVBoxLayout *tradeHintAreaLayout = new QVBoxLayout(tradeHintArea);
    tradeHintAreaLayout->setContentsMargins(8, 8, 8, 8);
    tradeHintAreaLayout->setSpacing(8);
    tradeHintAreaLayout->addStretch();
    tradeHintAreaLayout->addWidget(m_tradeHint);
    tradeHintAreaLayout->addStretch();
    tradeHintAreaLayout->addWidget(m_obuHint);

    return tradeHintArea;
}

RecentTradePanel *MtcInPage::initRecentTradeArea()
{
    QStringList header = {"车牌", "车型", "入口时间", "卡类型", "卡号"};
    RecentTradePanel *recentTradePanel = new RecentTradePanel(header);
    recentTradePanel->setMinimumHeight(103);
    QTableView *view = recentTradePanel->getRecentTradeView();
    // 初始列表宽度
    view->setColumnWidth(0, 100);
    view->setColumnWidth(1, 50);
    view->setColumnWidth(2, 130);
    view->setColumnWidth(3, 80);
    return recentTradePanel;
}

QWidget *MtcInPage::initDeviceArea()
{
    QWidget *deviceArea = new QWidget();
    deviceArea->setMinimumHeight(50);

    m_deviceIconPanel = new DevicePanel(deviceArea);

    QVBoxLayout *deviceAreaVLayout = new QVBoxLayout(deviceArea);
    deviceAreaVLayout->setContentsMargins(0, 0, 0, 0);
    deviceAreaVLayout->addWidget(m_deviceIconPanel);

    return deviceArea;
}
