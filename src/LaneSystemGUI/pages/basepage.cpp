#include "basepage.h"

#include "ElaWidgetTools/ElaIconButton.h"
#include "ElaWidgetTools/ElaImageCard.h"
#include "ElaWidgetTools/ElaPushButton.h"
#include "components/devicepanel.h"
#include "components/iconbutton.h"
#include "components/pagearea.h"
#include "components/recenttradepanel.h"
#include "components/scrolltext.h"
#include "components/weightinfopanel.h"
#include "global/globalmanager.h"
#include "global/signalmanager.h"
#include "global/uiconst.h"
#include "utils/datadealutils.h"
#include "utils/uiutils.h"
#include "json/dialogparams.h"
#include "json/infodialogparams.h"

#include <QHBoxLayout>
#include <QJsonObject>
#include <QScrollBar>
#include <QTableView>
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

    // 登录窗口初始化
    m_authDlg = new AuthDialog(this);
    m_authDlg->hide();
}

BasePage::~BasePage() {}

void BasePage::initUi()
{
    initLeftUi();
    initRightUi();

    QHBoxLayout *centralHlayout = new QHBoxLayout(m_centralWidget);
    centralHlayout->setContentsMargins(8, 8, 8, 5);
    centralHlayout->setSpacing(8);
    centralHlayout->addWidget(m_leftWidget, 380);
    centralHlayout->addWidget(m_rightWidget, 620);
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
        m_capImage->setCardImage(QImage(Path::CAP_AREA_BACKGROUND));
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

void BasePage::logAppend(IPageController::EM_LogLevel logLevel, const QString &log)
{
    const int maxCount = 300;
    const int trimCount = 100;

    // 维护日志缓存
    QString level;
    if (logLevel == IPageController::INFO) {
        level = "INFO";
    } else if (logLevel == IPageController::WARN) {
        level = "WARN";
    } else if (logLevel == IPageController::ERROR) {
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
        QColor textColor = QColor(Color::INFO_TC);
        if (line.contains("[ERROR]") || line.contains("[WARN]")) {
            textColor = Qt::red;
        }
        QTextCursor cursor = m_logBrowser->textCursor();
        cursor.movePosition(QTextCursor::End);

        QTextCharFormat format;
        format.setForeground(textColor);
        cursor.setCharFormat(format);

        if (i != m_logBuffer.size() - 1) {
            cursor.insertText(line + "\n");
        } else {
            cursor.insertText(line);
        }
    }

    m_logBrowser->moveCursor(QTextCursor::End);
    m_logBrowser->ensureCursorVisible();
}

void BasePage::setPlate(const QString &plate)
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

void BasePage::setVehClass(const QString &vehClass)
{
    if (!m_vehClass)
        return;
    m_vehClass->setText(vehClass);
}

void BasePage::setVehStatus(const QString &vehStatus)
{
    if (!m_vehStatus)
        return;
    m_vehStatus->setText(vehStatus);
}

void BasePage::setSituation(const QString &situation)
{
    if (!m_situation)
        return;
    m_situation->setText(situation);
    m_situation->setStyleSheet(QString("color: %1").arg(Color::WARN_TC));
}

void BasePage::setTradeHint(const QString &tradeHint, const QString &color)
{
    if (!m_tradeHint)
        return;

    m_tradeHint->setStyleSheet(QString("color: %1;").arg(color));
    m_tradeHint->setText(tradeHint);
}

void BasePage::setObuHint(const QString &obuHint, const QString &color)
{
    if (!m_obuHint)
        return;

    m_obuHint->setStyleSheet(QString("color: %1;").arg(color));
    m_obuHint->setText(obuHint);
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

void BasePage::setDeviceList(const QList<uint> &devList)
{
    if (!m_deviceIconPanel)
        return;
    m_deviceIconPanel->setDeviceList(devList);
}

void BasePage::updateDeviceStatus(IPageController::EM_DeviceIcon dev, uint status)
{
    if (!m_deviceIconPanel)
        return;
    m_deviceIconPanel->changeDevStatus(dev, status);
}

void BasePage::showAuthDialog(const QString &id, const QString &name)
{
    if (!m_authDlg || m_authDlg->isVisible())
        return;

    m_authDlg->show();
    m_authDlg->setID(id);
    m_authDlg->setName(name);
}

void BasePage::showInfoDialog(const QString &title, const QStringList &strs, bool switchLine)
{
    QString message;
    for (const auto &s : strs) {
        QString line = s;
        line.replace("\t", "&nbsp;&nbsp;&nbsp;&nbsp;");
        message += line + "<br/>";
    }
    auto btn = UiUtils::showMessageBoxInfo(title, message, QMessageBox::Yes | QMessageBox::Cancel, switchLine);

    DialogParams<InfoDialogResponse> params;
    params.api = m_api;
    params.data.yes = (btn == QMessageBox::Yes);

    emit GM_INSTANCE->m_signalMan->sigShowDialogResp("infoDialog", params.toJsonObj());
}

QWidget *BasePage::initDisplayArea()
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
    infoBoardArea->setBackgroundColor(QColor(Color::WARN_BUTTON_BG));

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
    logArea->setMinimumHeight(70);

    m_logBrowser = new QPlainTextEdit(logArea);
    m_logBrowser->setObjectName("logBrowser");
    m_logBrowser->setReadOnly(true);
    m_logBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_logBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_logBrowser->document()->setDocumentMargin(0);
    QFont font = m_logBrowser->font();
    font.setPixelSize(12);
    m_logBrowser->setFont(font);

    m_logBrowser->setFrameShape(QFrame::NoFrame);
    m_logBrowser->viewport()->setAutoFillBackground(false);

    m_logBrowser->setStyleSheet(QString("#logBrowser {background: transparent; border: none; color: %1;}").arg(Color::WARN_TC));

    QVBoxLayout *logAreaLayout = new QVBoxLayout(logArea);
    logAreaLayout->setContentsMargins(5, 3, 5, 3);
    logAreaLayout->addWidget(m_logBrowser);

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
    m_scrollTip->setFont(font);

    QHBoxLayout *scrollTipHLayout = new QHBoxLayout(scrollTipArea);
    scrollTipHLayout->setContentsMargins(8, 0, 8, 0);
    scrollTipHLayout->setSpacing(8);
    scrollTipHLayout->addWidget(tipIcon);
    scrollTipHLayout->addWidget(m_scrollTip, 1);

    return scrollTipArea;
}

PageArea *BasePage::initTradeHintArea()
{
    PageArea *tradeHintArea = new PageArea();
    tradeHintArea->setBorderRadius(8);
    tradeHintArea->setMinimumHeight(135);

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
    tradeHintAreaLayout->setSpacing(5);
    tradeHintAreaLayout->addStretch();
    tradeHintAreaLayout->addWidget(m_tradeHint);
    tradeHintAreaLayout->addStretch();
    tradeHintAreaLayout->addWidget(m_obuHint);

    return tradeHintArea;
}

PageArea *BasePage::initWeightInfoArea()
{
    PageArea *weightInfoArea = new PageArea();
    weightInfoArea->setBorderRadius(8);
    weightInfoArea->setFixedHeight(120);
    UiUtils::disableMouseEvents(weightInfoArea);

    // 当前车辆称重信息
    m_curWeightInfo = new QLabel(weightInfoArea);
    m_curWeightInfo->setStyleSheet(QString("color: %1;").arg(Color::INFO_TC));
    m_curWeightInfo->setWordWrap(false);
    QFont curWeightInfoFont = m_curWeightInfo->font();
    curWeightInfoFont.setPixelSize(14);
    m_curWeightInfo->setFont(curWeightInfoFont);

    // 称重降级提示
    m_weightLow = new IconButton(ElaIconType::TriangleExclamation, 14, QFont::DemiBold, weightInfoArea);
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

DevicePanel *BasePage::initDeviceArea()
{
    m_deviceIconPanel = new DevicePanel();
    m_deviceIconPanel->setMinimumHeight(50);

    return m_deviceIconPanel;
}

QWidget *BasePage::createShiftInfoBlock(const QString &infoTitle, ushort titleFontPx, QLabel *&numLabel, ushort labelFontPx, ushort width,
                                        ushort height, QWidget *parent)
{
    QWidget *infoWidget = new QWidget(parent);
    infoWidget->setFixedSize(width, height);

    QLabel *title = new QLabel(infoTitle, infoWidget);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QString("color: %1").arg(Color::INFO_TC));
    QFont titleFont = title->font();
    titleFont.setWeight(QFont::DemiBold);
    titleFont.setPixelSize(titleFontPx);
    title->setFont(titleFont);

    numLabel = new QLabel(infoWidget);
    numLabel->setAlignment(Qt::AlignCenter);
    QFont countFont = numLabel->font();
    countFont.setWeight(QFont::DemiBold);
    countFont.setPixelSize(labelFontPx);
    numLabel->setFont(countFont);
    numLabel->setText("0");

    QVBoxLayout *blockVLayout = new QVBoxLayout(infoWidget);
    blockVLayout->setContentsMargins(3, 3, 3, 3);
    blockVLayout->setSpacing(0);
    blockVLayout->addWidget(title);
    blockVLayout->addWidget(numLabel);

    return infoWidget;
}

void BasePage::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_NumLock)
        return;

    QDateTime now = DataDealUtils::curDateTime();

    // 同一按键，连续触发不响应
    if (m_lastKeyPressTime.isValid() && m_lastKeyPressTime.msecsTo(now) < 200 && m_lastKey == key)
        return;

    m_lastKeyPressTime = now;
    m_lastKey = key;

    emit GM_SIG->sigKeyPress(key);
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
    m_bottomWidget->setFixedHeight(45);
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
    clock->setFixedWidth(225);
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
    bottomHLayout->setContentsMargins(15, 0, 15, 5);
    bottomHLayout->addWidget(widget1);
    bottomHLayout->addStretch();
    bottomHLayout->addWidget(clock);
}

void BasePage::setApi(int newApi)
{
    m_api = newApi;
}
