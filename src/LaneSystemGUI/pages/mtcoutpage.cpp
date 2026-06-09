#include "mtcoutpage.h"

#include "ElaWidgetTools/ElaImageCard.h"
#include "components/devicepanel.h"
#include "components/pagearea.h"
#include "components/recenttradepanel.h"
#include "global/uiconst.h"
#include "utils/uiutils.h"

#include <QDebug>
#include <QGridLayout>
#include <QPushButton>

using namespace Utils;

MtcOutPage::MtcOutPage(QWidget *parent)
    : BasePage{parent}
{}

MtcOutPage::~MtcOutPage() {}

void MtcOutPage::setPrevImage(const QImage &img)
{
    if (!m_prevImage)
        return;

    if (img.isNull()) {
        QImage transparentImage(m_prevImage->size(), QImage::Format_ARGB32);
        transparentImage.fill(Qt::transparent);
        m_prevImage->setCardImage(transparentImage);
    } else {
        m_prevImage->setCardImage(img);
    }

    updatePrevImageSize(); // 小图比例调整

    m_prevImage->update();
}

void MtcOutPage::enableSptShiftInfoShow(bool isSptShiftInfo)
{
    m_isSptShiftInfo = isSptShiftInfo;
}

void MtcOutPage::setStartTicketNum(int num)
{
    if (!m_startTicketNum)
        return;
    m_startTicketNum->setText(QString("%1").arg(num));
}

void MtcOutPage::setCurTicketNum(int num)
{
    if (!m_curTicketNum)
        return;
    m_curTicketNum->setText(QString("%1").arg(num));
}

void MtcOutPage::setNormalTicketCnt(int cnt)
{
    if (!m_normalTicketCnt)
        return;
    m_normalTicketCnt->setText(QString("%1").arg(cnt));
}

void MtcOutPage::setScrapTicketCnt(int cnt)
{
    if (!m_scrapTicketCnt)
        return;
    m_scrapTicketCnt->setText(QString("%1").arg(cnt));
}

void MtcOutPage::setRestTicketCnt(int cnt)
{
    if (!m_restTicketCnt)
        return;
    m_restTicketCnt->setText(QString("%1").arg(cnt));
}

void MtcOutPage::setRecycleCardCnt(int cnt)
{
    if (!m_recycleCardCnt)
        return;
    m_recycleCardCnt->setText(QString("%1").arg(cnt));
}

void MtcOutPage::setThirdPayCnt(int cnt)
{
    if (!m_thirdPayCnt)
        return;
    m_thirdPayCnt->setText(QString("%1").arg(cnt));
}

void MtcOutPage::setUpStartTicketNum(int num)
{
    if (!m_upStartTicketNum)
        return;
    m_upStartTicketNum->setText(QString("%1").arg(num));
}

void MtcOutPage::setUpCurTicketNum(int num)
{
    if (!m_upCurTicketNum)
        return;
    m_upCurTicketNum->setText(QString("%1").arg(num));
}

void MtcOutPage::setUpRestTicketCnt(int cnt)
{
    if (!m_upRestTicketCnt)
        return;
    m_upRestTicketCnt->setText(QString("%1").arg(cnt));
}

void MtcOutPage::setUpRecycleCardCnt(int cnt)
{
    if (!m_upRecycleCardCnt)
        return;
    m_upRecycleCardCnt->setText(QString("%1").arg(cnt));
}

void MtcOutPage::setUpScrapTicketCnt(int cnt)
{
    if (!m_upScrapTicketCnt)
        return;
    m_upScrapTicketCnt->setText(QString("%1").arg(cnt));
}

void MtcOutPage::setUpWriteErrCnt(int cnt)
{
    if (!m_upWriteErrCnt)
        return;
    m_upWriteErrCnt->setText(QString("%1").arg(cnt));
}

void MtcOutPage::setDownStartTicketNum(int num)
{
    if (!m_downStartTicketNum)
        return;
    m_downStartTicketNum->setText(QString("%1").arg(num));
}

void MtcOutPage::setDownCurTicketNum(int num)
{
    if (!m_downCurTicketNum)
        return;
    m_downCurTicketNum->setText(QString("%1").arg(num));
}

void MtcOutPage::setDownRestTicketCnt(int cnt)
{
    if (!m_downRestTicketCnt)
        return;
    m_downRestTicketCnt->setText(QString("%1").arg(cnt));
}

void MtcOutPage::setDownRecycleCardCnt(int cnt)
{
    if (!m_downRecycleCardCnt)
        return;
    m_downRecycleCardCnt->setText(QString("%1").arg(cnt));
}

void MtcOutPage::setDownScrapTicketCnt(int cnt)
{
    if (!m_downScrapTicketCnt)
        return;
    m_downScrapTicketCnt->setText(QString("%1").arg(cnt));
}

void MtcOutPage::setDownWriteErrCnt(int cnt)
{
    if (!m_downWriteErrCnt)
        return;
    m_downWriteErrCnt->setText(QString("%1").arg(cnt));
}

void MtcOutPage::setCardType(const QString &cardType)
{
    if (!m_cardType)
        return;
    m_cardType->setText(cardType);
}

void MtcOutPage::setCardNum(const QString &cardNum)
{
    if (!m_cardNum)
        return;
    m_cardNum->setText(cardNum);
}

void MtcOutPage::setEnTime(const QString &enTime)
{
    if (!m_enTime)
        return;
    m_enTime->setText(enTime);
}

void MtcOutPage::setEnStationName(const QString &enStationName)
{
    if (!m_enStationName)
        return;
    m_enStationName->setText(enStationName);
}

void MtcOutPage::setWeightInfo(const QString &info)
{
    if (!m_weightInfo)
        return;
    m_weightInfo->setText(info);
}

void MtcOutPage::setEnPlate(const QString &plate)
{
    if (!m_enPlate)
        return;
    m_enPlate->setText(plate);
}

void MtcOutPage::setLabel1(const QString &info)
{
    if (!m_label1)
        return;
    m_label1->setText(info);
}

void MtcOutPage::setSplitProvincesInfo(const QString &info)
{
    if (!m_splitProvincesInfo)
        return;
    m_splitProvincesInfo->setText(info);
}

void MtcOutPage::appendHintButton(const QString &hint, const QString &fontColor, const QString &bgColor)
{
    if (!m_hintButtonArea->layout())
        return;

    QPushButton *hintBtn = new QPushButton();
    hintBtn->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    hintBtn->setFixedSize(80, 20);
    hintBtn->setStyleSheet(QString("background-color: %1; color: %2; border-radius: 5px;").arg(bgColor).arg(fontColor));
    QFont font = hintBtn->font();
    font.setPixelSize(13);
    font.setWeight(QFont::DemiBold);
    hintBtn->setFont(font);

    hintBtn->setText(hint);

    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(m_hintButtonArea->layout());

    layout->insertWidget(0, hintBtn);
}

void MtcOutPage::setTotalVehCnt(int cnt)
{
    if (!m_totalVehCnt)
        return;
    m_totalVehCnt->setText(QString("%1").arg(cnt));
}

void MtcOutPage::setFleetVehCnt(int cnt)
{
    if (!m_fleetVehCnt)
        return;
    m_fleetVehCnt->setText(QString("%1").arg(cnt));
}

void MtcOutPage::setEtcCardCnt(int cnt)
{
    if (!m_etcCardCnt)
        return;
    m_etcCardCnt->setText(QString("%1").arg(cnt));
}

void MtcOutPage::initLeftUi()
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

void MtcOutPage::initRightUi()
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
    rightVLayout->addWidget(m_deviceIconPanel, m_deviceIconPanel->minimumHeight());
}

QWidget *MtcOutPage::initDisplayArea()
{
    QWidget *displayArea = new QWidget();
    displayArea->setMinimumHeight(300);

    m_capImage = new ElaImageCard(displayArea);
    m_capImage->setBorderRadius(8);
    m_capImage->setIsPreserveAspectCrop(false);
    m_capImage->setCardImage(QImage(Path::CAP_AREA_BACKGROUND));

    m_prevImage = new ElaImageCard(displayArea);
    m_prevImage->setBorderRadius(8);
    m_prevImage->setIsPreserveAspectCrop(false);

    QGridLayout *displayAreaLayout = new QGridLayout(displayArea);
    displayAreaLayout->setContentsMargins(0, 0, 0, 0);
    displayAreaLayout->addWidget(m_capImage, 0, 0);
    displayAreaLayout->addWidget(m_prevImage, 0, 0, Qt::AlignTop | Qt::AlignRight);

    return displayArea;
}

PageArea *MtcOutPage::initShiftInfoArea()
{
    PageArea *shiftInfoArea = new PageArea();
    shiftInfoArea->setBorderRadius(8);
    shiftInfoArea->setMinimumHeight(156);

    if (m_isSptShiftInfo) {
        initSptShiftInfoArea(shiftInfoArea);
    } else {
        initNormalShiftInfoArea(shiftInfoArea);
    }

    return shiftInfoArea;
}

PageArea *MtcOutPage::initVehInfoArea()
{
    PageArea *vehInfoArea = new PageArea();
    vehInfoArea->setBorderRadius(8);
    vehInfoArea->setMinimumHeight(190);

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
    cardInfoWidget->setMinimumHeight(126);

    QLabel *label1 = new QLabel("类型", cardInfoWidget);
    QLabel *label2 = new QLabel("卡号", cardInfoWidget);
    QLabel *label3 = new QLabel("站名", cardInfoWidget);
    QLabel *label4 = new QLabel("时间", cardInfoWidget);
    QLabel *label5 = new QLabel("称重", cardInfoWidget);
    QLabel *label6 = new QLabel("车牌", cardInfoWidget);
    QLabel *label7 = new QLabel("标签", cardInfoWidget);
    QLabel *label8 = new QLabel("分省信息", cardInfoWidget);
    QList<QLabel *> labels = {label1, label2, label3, label4, label5, label6, label7, label8};
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
    m_enStationName = new QLabel(cardInfoWidget);
    m_enTime = new QLabel(cardInfoWidget);
    m_weightInfo = new QLabel(cardInfoWidget);
    m_enPlate = new QLabel(cardInfoWidget);
    m_label1 = new QLabel(cardInfoWidget);
    m_splitProvincesInfo = new QLabel(cardInfoWidget);
    QList<QLabel *> texts = {m_cardType, m_cardNum, m_enStationName, m_enTime, m_weightInfo, m_enPlate, m_label1, m_splitProvincesInfo};
    for (auto *t : texts) {
        QFont font = t->font();
        font.setWeight(QFont::DemiBold);
        font.setPixelSize(16);
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
    cardInfoGLayout->addWidget(label2, 0, 2);
    cardInfoGLayout->addWidget(m_cardNum, 0, 3);

    // 第二行：站名，时间
    cardInfoGLayout->addWidget(label3, 1, 0);
    cardInfoGLayout->addWidget(m_enStationName, 1, 1);
    cardInfoGLayout->addWidget(label4, 1, 2);
    cardInfoGLayout->addWidget(m_enTime, 1, 3);

    // 第三行：车牌，称重
    cardInfoGLayout->addWidget(label6, 2, 0);
    cardInfoGLayout->addWidget(m_enPlate, 2, 1);
    cardInfoGLayout->addWidget(label5, 2, 2);
    cardInfoGLayout->addWidget(m_weightInfo, 2, 3);

    // 第四行：标签
    cardInfoGLayout->addWidget(label7, 3, 0);
    cardInfoGLayout->addWidget(m_label1, 3, 1, 1, 3);

    // 第五行：分省信息
    cardInfoGLayout->addWidget(label8, 4, 0);
    cardInfoGLayout->addWidget(m_splitProvincesInfo, 4, 1, 1, 3);

    QVBoxLayout *vehInfoAreaLayout = new QVBoxLayout(vehInfoArea);
    vehInfoAreaLayout->setContentsMargins(8, 8, 8, 8);
    vehInfoAreaLayout->setSpacing(8);
    vehInfoAreaLayout->addWidget(carInfoWidget);
    vehInfoAreaLayout->addWidget(cardInfoWidget);

    return vehInfoArea;
}

PageArea *MtcOutPage::initTradeHintArea()
{
    PageArea *tradeHintArea = new PageArea();
    tradeHintArea->setBorderRadius(8);
    tradeHintArea->setMinimumHeight(131);

    m_hintButtonArea = new QWidget(tradeHintArea);
    QVBoxLayout *hintButtonAreaLayout = new QVBoxLayout(m_hintButtonArea);
    hintButtonAreaLayout->setContentsMargins(0, 0, 0, 0);
    hintButtonAreaLayout->setSpacing(5);
    hintButtonAreaLayout->addStretch();

    m_tradeHint = new QLabel(tradeHintArea);
    m_tradeHint->setWordWrap(true);
    QFont tradeHintfont = m_tradeHint->font();
    tradeHintfont.setPixelSize(30);
    tradeHintfont.setBold(true);
    m_tradeHint->setFont(tradeHintfont);

    QHBoxLayout *hintHLayout = new QHBoxLayout();
    hintHLayout->setContentsMargins(0, 0, 0, 0);
    hintHLayout->setSpacing(8);
    hintHLayout->addWidget(m_hintButtonArea);
    hintHLayout->addWidget(m_tradeHint, 1);

    m_obuHint = new QLabel(tradeHintArea);
    m_obuHint->setStyleSheet(QString("color: %1;").arg(Color::INFO_TC));
    m_obuHint->setWordWrap(true);
    QFont obuHintFont = m_obuHint->font();
    obuHintFont.setPixelSize(14);
    m_obuHint->setFont(obuHintFont);

    QVBoxLayout *tradeHintAreaLayout = new QVBoxLayout(tradeHintArea);
    tradeHintAreaLayout->setContentsMargins(8, 8, 8, 8);
    tradeHintAreaLayout->setSpacing(5);
    tradeHintAreaLayout->addStretch();
    tradeHintAreaLayout->addLayout(hintHLayout);
    tradeHintAreaLayout->addStretch();
    tradeHintAreaLayout->addWidget(m_obuHint);

    return tradeHintArea;
}

RecentTradePanel *MtcOutPage::initRecentTradeArea()
{
    // 交易记录表格
    QStringList header = {"车牌", "车型", "出口时间", "支付金额(元)", "卡号"};
    RecentTradePanel *recentTradePanel = new RecentTradePanel(header);
    recentTradePanel->setMinimumHeight(89);
    QTableView *view = recentTradePanel->getRecentTradeView();
    view->setColumnWidth(0, 100);
    view->setColumnWidth(1, 45);
    view->setColumnWidth(2, 120);
    view->setColumnWidth(3, 150);

    return recentTradePanel;
}

void MtcOutPage::resizeEvent(QResizeEvent *event)
{
    BasePage::resizeEvent(event);

    updatePrevImageSize(); // 小图缩放调整
}

void MtcOutPage::updatePrevImageSize()
{
    if (!m_displayArea)
        return;

    const double scale = 0.3; // 小图缩放比例，可自行调整（0.25 = 小图为大图的25%）
    double aspectRatio = static_cast<double>(m_displayArea->width()) / m_displayArea->height();
    int smallH = static_cast<int>(m_displayArea->height() * scale);
    int smallW = static_cast<int>(smallH * aspectRatio);
    m_prevImage->setFixedSize(smallW, smallH);
}

void MtcOutPage::initNormalShiftInfoArea(PageArea *parent)
{
    QWidget *widget1 = createShiftInfoBlock("起始票据", 15, m_startTicketNum, 16, 85, 70, parent);
    QWidget *widget2 = createShiftInfoBlock("剩余票据", 15, m_restTicketCnt, 18, 67, 70, parent);
    QWidget *widget3 = createShiftInfoBlock("正常票数", 15, m_normalTicketCnt, 18, 67, 70, parent);
    QWidget *widget4 = createShiftInfoBlock("废票张数", 15, m_scrapTicketCnt, 18, 67, 70, parent);
    QWidget *widget5 = createShiftInfoBlock("车队车", 15, m_fleetVehCnt, 18, 64, 70, parent);
    QWidget *widget6 = createShiftInfoBlock("当前票据", 15, m_curTicketNum, 16, 85, 70, parent);
    QWidget *widget7 = createShiftInfoBlock("回收卡数", 15, m_recycleCardCnt, 18, 67, 70, parent);
    QWidget *widget8 = createShiftInfoBlock("闽通卡数", 15, m_etcCardCnt, 18, 67, 70, parent);
    QWidget *widget9 = createShiftInfoBlock("车辆计数", 15, m_totalVehCnt, 18, 67, 70, parent);
    QWidget *widget10 = createShiftInfoBlock("第三方", 15, m_thirdPayCnt, 18, 64, 70, parent);

    QGridLayout *shiftInfoAreaLayout = new QGridLayout(parent);
    shiftInfoAreaLayout->setContentsMargins(5, 5, 5, 5);
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
}

void MtcOutPage::initSptShiftInfoArea(PageArea *parent)
{
    QWidget *normInfoWidget = new QWidget();

    QWidget *widget1 = createShiftInfoBlock("起始票据", 13, m_startTicketNum, 14, 80, 42, parent);
    QWidget *widget2 = createShiftInfoBlock("正常票数", 13, m_normalTicketCnt, 14, 65, 42, parent);
    QWidget *widget3 = createShiftInfoBlock("当前票据", 13, m_curTicketNum, 14, 80, 42, parent);
    QWidget *widget4 = createShiftInfoBlock("废票张数", 13, m_scrapTicketCnt, 14, 65, 42, parent);
    QWidget *widget5 = createShiftInfoBlock("剩余票据", 13, m_restTicketCnt, 14, 80, 42, parent);
    QWidget *widget6 = createShiftInfoBlock("车辆计数", 13, m_totalVehCnt, 14, 65, 42, parent);
    QWidget *widget7 = createShiftInfoBlock("回收卡数", 13, m_recycleCardCnt, 14, 80, 42, parent);
    QWidget *widget8 = createShiftInfoBlock("闽通卡数", 13, m_etcCardCnt, 14, 65, 42, parent);
    QWidget *widget9 = createShiftInfoBlock("车队车", 13, m_fleetVehCnt, 14, 80, 42, parent);
    QWidget *widget10 = createShiftInfoBlock("第三方", 13, m_thirdPayCnt, 14, 65, 42, parent);

    QGridLayout *tempGLayout = new QGridLayout(normInfoWidget);
    tempGLayout->setContentsMargins(0, 0, 0, 0);
    tempGLayout->setSpacing(0);
    tempGLayout->addWidget(widget1, 0, 0);
    tempGLayout->addWidget(widget2, 0, 1);
    tempGLayout->addWidget(widget3, 1, 0);
    tempGLayout->addWidget(widget4, 1, 1);
    tempGLayout->addWidget(widget5, 2, 0);
    tempGLayout->addWidget(widget6, 2, 1);
    tempGLayout->addWidget(widget7, 3, 0);
    tempGLayout->addWidget(widget8, 3, 1);
    tempGLayout->addWidget(widget9, 4, 0);
    tempGLayout->addWidget(widget10, 4, 1);

    m_upStationInfoArea = new QGroupBox("上工位", parent);
    m_downStationInfoArea = new QGroupBox("下工位", parent);
    QList<QGroupBox *> boxs = {m_upStationInfoArea, m_downStationInfoArea};
    for (auto *b : boxs) {
        b->setStyleSheet(QString("QGroupBox { background-color: %1; border: 1px solid %2; border-radius: 4px; "
                                 "margin-top: 8px; font-size: 12px; }"
                                 "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top center; padding: 2px 10px; "
                                 "background-color: %3; color: %4; border-radius: 4px; }")
                             .arg(Color::INSIDE_AREA_BG, "#D9DEE5", Color::CONFIRM_BUTTON_BG, Color::BUTTON_TC));
    }

    QWidget *widget11 = createShiftInfoBlock("起始票据", 13, m_upStartTicketNum, 14, 80, 42, m_upStationInfoArea);
    QWidget *widget12 = createShiftInfoBlock("回收卡数", 13, m_upRecycleCardCnt, 14, 65, 42, m_upStationInfoArea);
    QWidget *widget13 = createShiftInfoBlock("剩余票据", 13, m_upRestTicketCnt, 14, 65, 42, m_upStationInfoArea);
    QWidget *widget14 = createShiftInfoBlock("当前票据", 13, m_upCurTicketNum, 14, 80, 42, m_upStationInfoArea);
    QWidget *widget15 = createShiftInfoBlock("作废票数", 13, m_upScrapTicketCnt, 14, 65, 42, m_upStationInfoArea);
    QWidget *widget16 = createShiftInfoBlock("写卡错误", 13, m_upWriteErrCnt, 14, 65, 42, m_upStationInfoArea);

    QGridLayout *upStationGLayout = new QGridLayout(m_upStationInfoArea);
    upStationGLayout->setContentsMargins(0, 10, 0, 6);
    upStationGLayout->setSpacing(0);
    upStationGLayout->addWidget(widget11, 0, 0);
    upStationGLayout->addWidget(widget12, 0, 1);
    upStationGLayout->addWidget(widget13, 0, 2);
    upStationGLayout->addWidget(widget14, 1, 0);
    upStationGLayout->addWidget(widget15, 1, 1);
    upStationGLayout->addWidget(widget16, 1, 2);

    QWidget *widget17 = createShiftInfoBlock("起始票据", 13, m_downStartTicketNum, 14, 80, 42, m_downStationInfoArea);
    QWidget *widget18 = createShiftInfoBlock("回收卡数", 13, m_downRecycleCardCnt, 14, 65, 42, m_downStationInfoArea);
    QWidget *widget19 = createShiftInfoBlock("剩余票据", 13, m_downRestTicketCnt, 14, 65, 42, m_downStationInfoArea);
    QWidget *widget20 = createShiftInfoBlock("当前票据", 13, m_downCurTicketNum, 14, 80, 42, m_downStationInfoArea);
    QWidget *widget21 = createShiftInfoBlock("作废票数", 13, m_downScrapTicketCnt, 14, 65, 42, m_downStationInfoArea);
    QWidget *widget22 = createShiftInfoBlock("写卡错误", 13, m_downWriteErrCnt, 14, 65, 42, m_downStationInfoArea);

    QGridLayout *downStationGLayout = new QGridLayout(m_downStationInfoArea);
    downStationGLayout->setContentsMargins(0, 10, 0, 6);
    downStationGLayout->setSpacing(0);
    downStationGLayout->addWidget(widget17, 0, 0);
    downStationGLayout->addWidget(widget18, 0, 1);
    downStationGLayout->addWidget(widget19, 0, 2);
    downStationGLayout->addWidget(widget20, 1, 0);
    downStationGLayout->addWidget(widget21, 1, 1);
    downStationGLayout->addWidget(widget22, 1, 2);

    QVBoxLayout *tempVLayout = new QVBoxLayout();
    tempVLayout->setContentsMargins(0, 0, 0, 0);
    tempVLayout->setSpacing(6);
    tempVLayout->addWidget(m_upStationInfoArea);
    tempVLayout->addWidget(m_downStationInfoArea);

    QHBoxLayout *shiftInfoAreaLayout = new QHBoxLayout(parent);
    shiftInfoAreaLayout->setContentsMargins(5, 5, 5, 5);
    shiftInfoAreaLayout->setSpacing(0);
    shiftInfoAreaLayout->addWidget(normInfoWidget, 128);
    shiftInfoAreaLayout->addLayout(tempVLayout, 216);
}
