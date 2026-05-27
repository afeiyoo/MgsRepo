#include "basepage.h"

#include "ElaWidgetTools/ElaIconButton.h"
#include "ElaWidgetTools/ElaImageCard.h"
#include "ElaWidgetTools/ElaPushButton.h"
#include "components/iconbutton.h"
#include "components/pagearea.h"
#include "components/recenttradepanel.h"
#include "components/scrolltext.h"
#include "components/weightinfopanel.h"
#include "global/uiconst.h"
#include "utils/datadealutils.h"
#include "utils/uiutils.h"
#include <QTableView>

#include <QDebug>
#include <QHBoxLayout>
#include <QTimer>
#include <QVBoxLayout>

using namespace Utils;

BasePage::BasePage(QWidget *parent)
    : QWidget{parent}
{
    createTopWidget();
    createBottomWidget();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_topWidget);
    mainLayout->addStretch();
    mainLayout->addWidget(m_bottomWidget);

    m_centralWidget = new QWidget(this);
    m_centralWidget->setObjectName("mainWidget");
    m_centralWidget->setStyleSheet(QString("#mainWidget { background-color: %1; }").arg(Color::MAIN_BG));
    addCentralWidget(m_centralWidget);
}

BasePage::~BasePage() {}

void BasePage::initUi()
{
    initLeftUi();
    initRightUi();

    QHBoxLayout *centralHlayout = new QHBoxLayout(m_centralWidget);
    centralHlayout->setContentsMargins(8, 8, 8, 8);
    centralHlayout->setSpacing(8);
    centralHlayout->addWidget(m_leftWidget, 370);
    centralHlayout->addWidget(m_rightWidget, 630);

    setStationInfo("福州西(6701)(港口站)");
    setLaneID(12);
    setUserInfo("测试员(3501911)");
    setShiftInfo("2025-12-25晚班");
    setModeText("混合自动发卡");

    setVirtualGantryInfo("福州西B向门架(34090.H)");
    setAppVer("v1.0.1");
    setFeeRateVer("1908");
    setFullBlackVer("20250801");
    setPartBlackVer("202508111211");

    setScrollTip("欢迎使用福建省高速公路收费软件（如遇到软件问题请致电福建省高速公路信息科技有限公司）");

    setPlate("蓝闽B8L722");
    setVehClass("货四");
    setVehStatus("抢险救灾车");
    setSituation("超限");

    setCardType("CPC卡");
    setCardNum("35012042230602103792");
    setEnTime("2025-07-12 12:00:04");
    setEnStationName("福建莆田西站");
    setBalance("88.2元");
    setCardStatus("正常");

    setTradeHint("自动发卡,等待过车");
    setObuHint("大件运输车; 蓝闽B8L722; 普通车; 专二; OBUSN: 35011603330999998340 12轴型 2轴 10.00吨 限重18.00吨 超限0.00%");

    setWeightLow(true);
    setCurWeightInfo("12轴型 2轴 10.00吨 限重18.00吨 超限0.00%");
    setCurWeightInfoCount(1);
    ST_WeightInfoItem item1;
    item1.plate = "闽A12345";
    item1.axisType = 1127;
    item1.axisNum = 6;
    item1.tollWeight = 25.74;
    item1.status = 1;
    appendWeightInfoItem(item1);
    ST_WeightInfoItem item2;
    item2.plate = "闽B100000";
    item2.axisType = 1127;
    item2.axisNum = 6;
    item2.tollWeight = 25.74;
    item2.status = 0;
    appendWeightInfoItem(item2);
    ST_WeightInfoItem item3;
    item3.plate = "闽BA50430";
    item3.axisType = 125;
    item3.axisNum = 4;
    item3.tollWeight = 22.74;
    item3.status = 0;
    appendWeightInfoItem(item3);
    ST_WeightInfoItem item4;
    item4.plate = "闽BA50430";
    item4.axisType = 125;
    item4.axisNum = 4;
    item4.tollWeight = 22.74;
    item4.status = 0;
    appendWeightInfoItem(item4);

    appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "CPC卡", "35012042230602103792"});
    appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "纸券", "35012042230602103792"});
    appendTradeItem({"闽A12345F", "货一", "07-12 12:00:04", "ETC卡", "35012042230602103792"});
    appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "CPC卡", "35012042230602103792"});
    appendTradeItem({"闽A12345", "货一", "07-12 12:00:04", "无卡", ""});

    setDeviceList({EM_DeviceIcon::WEIGHT, EM_DeviceIcon::FIRST_COIL, EM_DeviceIcon::CELLING_LAMP, EM_DeviceIcon::CAP_COIL, EM_DeviceIcon::CAPTURE,
                   EM_DeviceIcon::RAILLING_COIL, EM_DeviceIcon::PASSING_LAMP, EM_DeviceIcon::RAILING_MACHINE, EM_DeviceIcon::RSU});
}

void BasePage::setStationInfo(const QString &stationInfo)
{
    if (!m_stationInfo)
        return;
    m_stationInfo->setText(stationInfo);
}

void BasePage::setUserInfo(const QString &userInfo)
{
    if (!m_userInfo)
        return;
    m_userInfo->setText(userInfo);
}

void BasePage::setLaneID(uint laneID)
{
    if (!m_laneID)
        return;
    m_laneID->setText(QString::number(laneID) + "道");
}

void BasePage::setShiftInfo(const QString &shiftInfo)
{
    if (!m_shiftInfo)
        return;
    m_shiftInfo->setText(shiftInfo);
}

void BasePage::setModeText(const QString &mode)
{
    if (!m_modeText)
        return;
    m_modeText->setText(QString(" %1 ").arg(mode));
}

void BasePage::setFullBlackVer(const QString &ver)
{
    if (!m_fullBlackVer)
        return;
    m_fullBlackVer->setText(QString("状态名单版本:全量%1").arg(ver));
}

void BasePage::setPartBlackVer(const QString &ver)
{
    if (!m_partBlackVer)
        return;
    m_partBlackVer->setText(QString("增量%1").arg(ver));
}

void BasePage::setVirtualGantryInfo(const QString &info)
{
    if (!m_virtualGantryInfo)
        return;
    m_virtualGantryInfo->setText(QString("承载门架:%1").arg(info));
}

void BasePage::setAppVer(const QString &ver)
{
    if (!m_appVer)
        return;
    m_appVer->setText(QString("系统版本:%1").arg(ver));
}

void BasePage::setFeeRateVer(const QString &ver)
{
    if (!m_feeRateVer)
        return;
    m_feeRateVer->setText(QString("费率版本:%1").arg(ver));
}

void BasePage::setCapImage(const QImage &img)
{
    if (!m_capImage)
        return;

    if (img.isNull()) {
        QImage transparentImage(m_capImage->size(), QImage::Format_ARGB32);
        transparentImage.fill(Qt::transparent);
        m_capImage->setCardImage(transparentImage);
    } else {
        m_capImage->setCardImage(img);
    }

    m_capImage->update();
}

void BasePage::setScrollTip(const QString &tip)
{
    if (!m_scrollTip)
        return;
    m_scrollTip->setScrollText(tip);
}

void BasePage::logAppend(EM_LogLevel::LogLevel logLevel, const QString &log)
{
    const int maxCount = 100;
    const int trimCount = 50;

    // 维护日志缓存
    QString level;
    if (logLevel == EM_LogLevel::INFO) {
        level = "INFO";
    } else if (logLevel == EM_LogLevel::WARN) {
        level = "WARN";
    } else if (logLevel == EM_LogLevel::ERROR) {
        level = "ERROR";
    } else {
        level = "DEBUG";
    }
    m_logBuffer.append(QString("%1 [%2] | %3").arg(DataDealUtils::curDateTimeStr("hh:mm:ss"), level, log));
    if (m_logBuffer.size() > maxCount) {
        m_logBuffer.erase(m_logBuffer.begin(), m_logBuffer.begin() + trimCount);
    }

    // 清空后重建整个日志区域
    m_logBrowser->clear();
    for (int i = 0; i < m_logBuffer.size(); ++i) {
        const QString &line = m_logBuffer[i];
        QColor textColor = Qt::black;
        if (line.contains("[ERROR]") || line.contains("[WARN]"))
            textColor = Qt::red;

        QTextCursor cursor = m_logBrowser->textCursor();
        cursor.movePosition(QTextCursor::End);

        QTextCharFormat format;
        format.setForeground(textColor);
        cursor.setCharFormat(format);
        cursor.insertText(line);
        if (i != m_logBuffer.size() - 1)
            cursor.insertText("\n");
    }

    m_logBrowser->moveCursor(QTextCursor::End);
    m_logBrowser->ensureCursorVisible();
}

void BasePage::setCurWeightInfoCount(uint curWeightInfoCount)
{
    if (!m_curWeightInfoCount)
        return;
    m_curWeightInfoCount->setText(QString("车辆数: %1").arg(curWeightInfoCount));
}

void BasePage::setCurWeightInfo(const QString &curWeightInfo, const QString &color)
{
    if (!m_curWeightInfo)
        return;
    m_curWeightInfo->setText(curWeightInfo);
    if (color != "#000000") {
        m_curWeightInfo->setStyleSheet(QString("color: %1;").arg(color));
    }
}

void BasePage::setCurWeightStatus(uint status)
{
    if (!m_weightInfoPanel)
        return;
    m_weightInfoPanel->updateItem(status);
}

void BasePage::appendWeightInfoItem(const ST_WeightInfoItem &item)
{
    if (!m_weightInfoPanel)
        return;
    m_weightInfoPanel->appendItem(item);
}

void BasePage::removeWeightInfoItem(uint index)
{
    if (!m_weightInfoPanel)
        return;
    m_weightInfoPanel->removeItem(index);
}

void BasePage::refreshWeightInfoItem(uint index, const ST_WeightInfoItem &item)
{
    if (!m_weightInfoPanel)
        return;
    m_weightInfoPanel->updateItem(index, item);
}

void BasePage::clearWeightInfoItem()
{
    if (!m_weightInfoPanel)
        return;
    m_weightInfoPanel->clearItem();
}

void BasePage::setWeightLow(bool isLow)
{
    if (!m_weightLow)
        return;
    m_weightLow->setVisible(isLow);
}

void BasePage::appendTradeItem(const QStringList &trade)
{
    if (!m_recentTradePanel)
        return;
    m_recentTradePanel->appendTrade(trade);
}

void BasePage::clearTradeItems()
{
    if (!m_recentTradePanel)
        return;
    m_recentTradePanel->clearTrades();
}

QWidget *BasePage::initLaneStatusArea()
{
    QWidget *laneStatusArea = new QWidget();
    laneStatusArea->setMinimumHeight(40);

    m_infoBoard = new QLabel("车道关闭", laneStatusArea);
    m_vehMode = new QLabel("客车流程", laneStatusArea);
    QList<QLabel *> texts = {m_infoBoard, m_vehMode};
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
    infoBoardArea->setBackgroundColor(QColor(Color::INFOBOARD_WARN_BG));

    QHBoxLayout *tempHLayout1 = new QHBoxLayout(infoBoardArea);
    tempHLayout1->setContentsMargins(0, 0, 0, 0);
    tempHLayout1->addWidget(m_infoBoard);

    PageArea *vehModeArea = new PageArea(laneStatusArea);
    vehModeArea->setBorderRadius(8);
    vehModeArea->setMinimumHeight(40);
    vehModeArea->setBackgroundColor(QColor(Color::CONFIRM_BUTTON_BG));

    QHBoxLayout *tempHLayout2 = new QHBoxLayout(vehModeArea);
    tempHLayout2->setContentsMargins(0, 0, 0, 0);
    tempHLayout2->addWidget(m_vehMode);

    QHBoxLayout *laneStatusAreaLayout = new QHBoxLayout(laneStatusArea);
    laneStatusAreaLayout->setSpacing(8);
    laneStatusAreaLayout->setContentsMargins(0, 0, 0, 0);
    laneStatusAreaLayout->addWidget(infoBoardArea);
    laneStatusAreaLayout->addWidget(vehModeArea);

    return laneStatusArea;
}

PageArea *BasePage::initLogBrowseArea()
{
    PageArea *logArea = new PageArea();
    logArea->setBorderRadius(8);
    logArea->setMinimumHeight(64);

    QPlainTextEdit *logBrowser = new QPlainTextEdit(logArea);
    logBrowser->setObjectName("logBrowser");
    logBrowser->setReadOnly(true);
    logBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    logBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QFont font = logBrowser->font();
    font.setPixelSize(12);
    logBrowser->setFont(font);

    logBrowser->setFrameShape(QFrame::NoFrame);
    logBrowser->viewport()->setAutoFillBackground(false);

    logBrowser->setStyleSheet(QString("#logBrowser {background: transparent; border: none; color: %1;}").arg(Color::INFO_TC));

    QVBoxLayout *logAreaLayout = new QVBoxLayout(logArea);
    logAreaLayout->setContentsMargins(5, 5, 5, 5);
    logAreaLayout->addWidget(logBrowser);

    return logArea;
}

PageArea *BasePage::initScrollTipArea()
{
    PageArea *scrollTipArea = new PageArea();
    scrollTipArea->setBackgroundColor(QColor(Color::CONFIRM_BUTTON_BG));
    scrollTipArea->setBorderRadius(8);
    scrollTipArea->setMinimumHeight(40);

    ElaIconButton *tipIcon = new ElaIconButton(ElaIconType::MessageCaptions, 25, 30, 30, scrollTipArea);
    tipIcon->setLightIconColor(QColor(Color::BUTTON_TC));

    m_scrollTip = new ScrollText(scrollTipArea);
    m_scrollTip->setStyleSheet(QString("color: %1;").arg(Color::BUTTON_TC));
    QFont font = m_scrollTip->font();
    font.setPixelSize(16);
    font.setWeight(QFont::DemiBold);
    m_scrollTip->setFont(font);

    QHBoxLayout *scrollTipHLayout = new QHBoxLayout(scrollTipArea);
    scrollTipHLayout->setContentsMargins(8, 0, 8, 0);
    scrollTipHLayout->setSpacing(8);
    scrollTipHLayout->addWidget(tipIcon);
    scrollTipHLayout->addWidget(m_scrollTip, 1);

    return scrollTipArea;
}

PageArea *BasePage::initWeightInfoArea()
{
    PageArea *weightInfoArea = new PageArea();
    weightInfoArea->setBorderRadius(8);
    weightInfoArea->setFixedHeight(120);

    // 当前车辆称重信息
    m_curWeightInfo = new QLabel(weightInfoArea);
    m_curWeightInfo->setStyleSheet(QString("color: %1;").arg(Color::INFO_TC));
    m_curWeightInfo->setWordWrap(false);
    QFont curWeightInfoFont = m_curWeightInfo->font();
    curWeightInfoFont.setPixelSize(14);
    m_curWeightInfo->setFont(curWeightInfoFont);

    // 称重降级提示
    m_weightLow = new IconButton(ElaIconType::TriangleExclamation, 14, weightInfoArea);
    m_weightLow->setColor(Color::WARN_TC);
    m_weightLow->setText("注意:称重降级已启用!!!");
    m_weightLow->hide();

    // 称重队列车辆数
    m_curWeightInfoCount = new QLabel("车辆数:0", weightInfoArea);
    m_curWeightInfoCount->setStyleSheet(QString("color: %1;").arg(Color::INFO_TC));
    m_curWeightInfoCount->setWordWrap(false);
    QFont curWeightInfoCountFont = m_curWeightInfoCount->font();
    curWeightInfoCountFont.setPixelSize(14);
    m_curWeightInfoCount->setFont(curWeightInfoCountFont);

    QHBoxLayout *weightSubHLayout = new QHBoxLayout();
    weightSubHLayout->setContentsMargins(0, 0, 0, 0);
    weightSubHLayout->addWidget(m_curWeightInfo);
    weightSubHLayout->addStretch(1);
    weightSubHLayout->addWidget(m_weightLow);
    weightSubHLayout->addWidget(m_curWeightInfoCount);

    // 称重信息队列
    m_weightInfoPanel = new WeightInfoPanel(weightInfoArea);

    QVBoxLayout *weightInfoAreaLayout = new QVBoxLayout(weightInfoArea);
    weightInfoAreaLayout->setContentsMargins(8, 8, 8, 8);
    weightInfoAreaLayout->setSpacing(8);
    weightInfoAreaLayout->addLayout(weightSubHLayout);
    weightInfoAreaLayout->addWidget(m_weightInfoPanel, 1);

    return weightInfoArea;
}

QWidget *BasePage::createShiftInfoBlock(const QString &infoTitle, QLabel *numLabel, ushort width, ushort height, QWidget *parent)
{
    QWidget *infoWidget = new QWidget();
    infoWidget->setFixedSize(width, height);

    QLabel *title = new QLabel(infoTitle, infoWidget);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QString("color: %1").arg(Color::INFO_TC));
    QFont titleFont = title->font();
    titleFont.setWeight(QFont::DemiBold);
    titleFont.setPixelSize(16);
    title->setFont(titleFont);

    numLabel = new QLabel(parent);
    numLabel->setAlignment(Qt::AlignCenter);
    QFont countFont = numLabel->font();
    countFont.setWeight(QFont::DemiBold);
    countFont.setPixelSize(24);
    numLabel->setFont(countFont);
    numLabel->setText("0");

    QVBoxLayout *blockVLayout = new QVBoxLayout(infoWidget);
    blockVLayout->setContentsMargins(3, 3, 3, 3);
    blockVLayout->setSpacing(0);
    blockVLayout->addWidget(title);
    blockVLayout->addWidget(numLabel);

    return infoWidget;
}

void BasePage::createTopWidget()
{
    m_topWidget = new QWidget(this);
    m_topWidget->setObjectName("topWidget");
    m_topWidget->setStyleSheet(QString("#topWidget { background-color: %1; }").arg(Color::TOP_WIDGET_BG));
    m_topWidget->setFixedHeight(50);
    UiUtils::disableMouseEvents(m_topWidget);

    // 图标
    ElaImageCard *pixmap = new ElaImageCard(m_topWidget);
    pixmap->setFixedSize(140, 40);
    pixmap->setIsPreserveAspectCrop(false);
    pixmap->setCardImage(QImage(Path::APP_BIG_ICON));

    // 信息栏
    QWidget *widget1 = new QWidget(this);

    m_stationInfo = new QLabel(widget1);
    m_userInfo = new QLabel(widget1);
    m_laneID = new QLabel(widget1);
    m_shiftInfo = new QLabel(widget1);
    QList<QLabel *> texts = {m_stationInfo, m_userInfo, m_laneID, m_shiftInfo};
    for (auto *t : texts) {
        t->setStyleSheet(QString("color: %1").arg(Color::INFO_TC));
        t->setWordWrap(false);
        QFont font = t->font();
        font.setWeight(QFont::DemiBold);
        font.setPixelSize(20);
        t->setFont(font);
    }

    QHBoxLayout *tempHLayout1 = new QHBoxLayout(widget1);
    tempHLayout1->setContentsMargins(0, 0, 0, 0);
    tempHLayout1->setSpacing(18);
    tempHLayout1->addWidget(m_stationInfo);
    tempHLayout1->addWidget(m_laneID);
    tempHLayout1->addWidget(m_userInfo);
    tempHLayout1->addWidget(m_shiftInfo);

    // 车道模式标签
    m_modeText = new ElaPushButton(m_topWidget);
    m_modeText->setBorderRadius(16);
    m_modeText->setMinimumWidth(115);
    m_modeText->setFixedHeight(40);
    m_modeText->setLightDefaultColor(QColor(Color::CONFIRM_BUTTON_BG));
    m_modeText->setLightTextColor(QColor(Color::BUTTON_TC));
    QFont font = m_modeText->font();
    font.setPixelSize(15);
    font.setWeight(QFont::DemiBold);
    m_modeText->setFont(font);

    QWidget *widget2 = new QWidget(this);
    QHBoxLayout *tempHLayout2 = new QHBoxLayout(widget2);
    tempHLayout2->setContentsMargins(0, 0, 0, 0);
    tempHLayout2->addWidget(m_modeText);

    QHBoxLayout *topHLayout = new QHBoxLayout(m_topWidget);
    topHLayout->setContentsMargins(10, 5, 10, 5);
    topHLayout->addWidget(pixmap);
    topHLayout->addStretch();
    topHLayout->addWidget(widget1);
    topHLayout->addStretch();
    topHLayout->addWidget(widget2);
}

void BasePage::addCentralWidget(QWidget *w)
{
    if (!w)
        return;

    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(this->layout());
    layout->insertWidget(layout->indexOf(m_topWidget) + 1, w, 1);
}

void BasePage::createBottomWidget()
{
    m_bottomWidget = new QWidget(this);
    m_bottomWidget->setFixedHeight(48);
    m_bottomWidget->setObjectName("bottomWidget");
    m_bottomWidget->setStyleSheet(QString("#bottomWidget { background-color: %1; }").arg(Color::MAIN_BG));
    UiUtils::disableMouseEvents(m_bottomWidget);

    // 版本信息区
    m_virtualGantryInfo = new QLabel("承载门架:", m_bottomWidget);
    m_appVer = new QLabel("系统版本:", m_bottomWidget);
    m_feeRateVer = new QLabel("费率版本:", m_bottomWidget);
    m_fullBlackVer = new QLabel("状态名单版本:", m_bottomWidget);
    m_partBlackVer = new QLabel(m_bottomWidget);

    QList<QLabel *> texts = {m_fullBlackVer, m_partBlackVer, m_appVer, m_feeRateVer, m_virtualGantryInfo};
    for (auto *t : texts) {
        t->setWordWrap(false);
        QFont font = t->font();
        font.setPixelSize(14);
        font.setWeight(QFont::DemiBold);
        t->setFont(font);
    }

    QHBoxLayout *tempHLayout = new QHBoxLayout();
    tempHLayout->setContentsMargins(0, 0, 0, 0);
    tempHLayout->setSpacing(20);
    tempHLayout->addWidget(m_appVer);
    tempHLayout->addStretch();
    tempHLayout->addWidget(m_feeRateVer);
    tempHLayout->addStretch();
    tempHLayout->addWidget(m_fullBlackVer);
    tempHLayout->addWidget(m_partBlackVer);

    QWidget *widget1 = new QWidget(m_bottomWidget);
    QVBoxLayout *tempVLayout1 = new QVBoxLayout(widget1);
    tempVLayout1->setContentsMargins(0, 0, 0, 0);
    tempVLayout1->setSpacing(0);
    tempVLayout1->addWidget(m_virtualGantryInfo);
    tempVLayout1->addLayout(tempHLayout);

    // 状态区（时钟）
    QLabel *clock = new QLabel(m_bottomWidget);
    clock->setFixedWidth(215);
    clock->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    clock->setStyleSheet(QString("color: %1").arg(Color::INFO_TC));
    clock->setWordWrap(false);
    QFont font = clock->font();
    font.setPixelSize(21);
    font.setWeight(QFont::DemiBold);
    clock->setFont(font);
    clock->setText(DataDealUtils::curDateTimeStr());

    QTimer *timer = new QTimer(clock);
    connect(timer, &QTimer::timeout, clock, [clock]() { clock->setText(DataDealUtils::curDateTimeStr()); });
    timer->start(1000); // 每秒刷新

    QHBoxLayout *bottomHLayout = new QHBoxLayout(m_bottomWidget);
    bottomHLayout->setContentsMargins(15, 0, 15, 8);
    bottomHLayout->addWidget(widget1);
    bottomHLayout->addStretch();
    bottomHLayout->addWidget(clock);
}

void BasePage::initLeftUi()
{
    // 抓拍显示区域
    m_displayArea = initDisplayArea();
    m_displayArea->setParent(this);

    // 车道状态显示区域
    m_laneStatusArea = initLaneStatusArea();
    m_laneStatusArea->setParent(this);

    QWidget *shiftAndLogArea = new QWidget(this);
    shiftAndLogArea->setMinimumHeight(298);

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

    leftVLayout->addWidget(m_displayArea, 300);
    leftVLayout->addWidget(m_laneStatusArea, 40);
    leftVLayout->addWidget(shiftAndLogArea, 298);
}

void BasePage::initRightUi()
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
    m_deviceArea = initDeviceArea();
    m_deviceArea->setParent(this);

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
    rightVLayout->addWidget(m_deviceArea, m_deviceArea->minimumHeight());
}
