#include "t_vehrecognizer.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHostAddress>
#include <QLabel>
#include <QUrl>
#include <QVBoxLayout>

#include "ElaLineEdit.h"
#include "ElaMessageBar.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"
#include "ElaSpinBox.h"
#include "ElaText.h"
#include "Logger.h"
#include "ivehrecognizer.h"
#include "utils/bizutils.h"
#include "utils/datadealutils.h"

using namespace Utils;

namespace {
ElaText *createLabel(const QString &text, QWidget *parent = nullptr)
{
    auto *label = new ElaText(text, parent);
    label->setTextPixelSize(14);
    return label;
}

QLabel *createImageLabel(QWidget *parent)
{
    auto *label = new QLabel("等待车辆图片", parent);
    label->setAlignment(Qt::AlignCenter);
    label->setMinimumSize(120, 80);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    label->setStyleSheet("QLabel { border: 1px solid #999; border-radius: 4px; color: #777; background: #181818; }");
    return label;
}
} // namespace

T_VehRecognizer::T_VehRecognizer(QWidget *parent)
    : T_BasePage{parent}
{
    setWindowTitle("车型识别器测试工具");
    createCustomWidget("测试车型识别器是否正常工作");
    initContent();

    createRecognizer();

    connect(m_connectButton, &ElaPushButton::clicked, this, &T_VehRecognizer::onConnectServer);
    connect(m_clearButton, &ElaPushButton::clicked, this, &T_VehRecognizer::resetVehicleWidgets);
    connect(m_uploadButton, &ElaPushButton::clicked, this, &T_VehRecognizer::onSetUploadUrl);
    connect(m_ledButton, &ElaPushButton::clicked, this, &T_VehRecognizer::onShowLed);
    connect(m_voiceButton, &ElaPushButton::clicked, this, &T_VehRecognizer::onPlayVoice);
    connect(m_logClearButton, &ElaPushButton::clicked, m_logEdit, &ElaPlainTextEdit::clear);
    connect(cuteLogger, &Logger::sigLogWrite, this, [this](Logger::LogLevel, const QString &log, const QString &category) {
        if (category == "VehRecognizer")
            m_logEdit->appendPlainText(DataDealUtils::curDateTimeStr() + " | " + log + "\n");
    });
    setCommandButtonsEnabled(false);
}

T_VehRecognizer::~T_VehRecognizer()
{
    if (m_recognizer)
        destroyVehRecognizer(m_recognizer);
}

void T_VehRecognizer::initContent()
{
    auto *connectionBox = new QGroupBox("设备连接（初始化信息: 站代码6701/福州西/车道11/协议0x01）", this);
    auto *connectionLayout = new QHBoxLayout(connectionBox);
    m_connectInfoEdit = new ElaLineEdit(this);
    m_connectInfoEdit->setPlaceholderText("IP地址:端口，例如 127.0.0.1:9588");
    m_connectButton = new ElaPushButton("连接", this);
    m_clearButton = new ElaPushButton("重置界面", this);
    m_connectionStatusText = createLabel("未连接", this);
    m_connectionStatusText->setStyleSheet("color: #ff0000");
    connectionLayout->addWidget(createLabel("服务地址", this));
    connectionLayout->addWidget(m_connectInfoEdit, 1);
    connectionLayout->addWidget(m_connectButton);
    connectionLayout->addWidget(m_clearButton);
    connectionLayout->addSpacing(8);
    connectionLayout->addWidget(createLabel("状态", this));
    connectionLayout->addWidget(m_connectionStatusText);

    auto *imageBox = new QGroupBox("车辆图片", this);
    auto *imageLayout = new QHBoxLayout(imageBox);
    const auto addImage = [this, imageLayout](const QString &title, QLabel *&image, ElaText *&info) {
        auto *widget = new QWidget(this);
        auto *layout = new QVBoxLayout(widget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(4);
        auto *text = createLabel(title, this);
        QFont font = text->font();
        font.setBold(true);
        text->setFont(font);
        image = createImageLabel(this);
        info = createLabel("-", this);
        info->setAlignment(Qt::AlignCenter);

        layout->addWidget(text, 0, Qt::AlignCenter);
        layout->addWidget(image, 1);
        layout->addWidget(info);
        imageLayout->addWidget(widget, 1);
    };
    addImage("车头图片", m_headImage, m_headImageInfo);
    addImage("车尾图片", m_tailImage, m_tailImageInfo);
    addImage("车身图片", m_bodyImage, m_bodyImageInfo);

    auto *detailBox = new QGroupBox("车型信息", this);
    auto *detailLayout = new QGridLayout(detailBox);
    const auto addField = [this, detailLayout](int row, int column, const QString &name, ElaText *&value) {
        detailLayout->addWidget(createLabel(name, this), row, column * 2);
        value = createLabel("-", this);
        QFont font = value->font();
        font.setBold(true);
        value->setFont(font);
        detailLayout->addWidget(value, row, column * 2 + 1);
    };
    addField(0, 0, "车牌", m_plateText);
    addField(0, 1, "车牌颜色", m_plateColorText);
    addField(0, 2, "过车时间", m_timeText);
    addField(1, 0, "车型", m_vehicleClassText);
    addField(1, 1, "轴型", m_axleTypeText);
    addField(1, 2, "轴数", m_axleCountText);
    addField(2, 0, "长×宽×高(cm)", m_dimensionText);
    addField(2, 1, "扩展标识", m_extFlagText);
    addField(2, 2, "行驶方向", m_directionText);
    detailLayout->setColumnStretch(1, 1);
    detailLayout->setColumnStretch(3, 1);
    detailLayout->setColumnStretch(5, 2);

    auto *commandBox = new QGroupBox("设备指令", this);
    auto *commandLayout = new QGridLayout(commandBox);
    m_uploadUrlEdit = new ElaLineEdit(this);
    m_uploadUrlEdit->setPlaceholderText("状态上报URL，例如 http://127.0.0.1/status");
    m_uploadMinutesSpinBox = new ElaSpinBox(this);
    m_uploadMinutesSpinBox->setRange(1, 60);
    m_uploadMinutesSpinBox->setValue(10);
    m_uploadMinutesSpinBox->setSuffix("min");
    m_uploadButton = new ElaPushButton("设置上报", this);

    m_ledTextEdit = new ElaLineEdit(this);
    m_ledTextEdit->setPlaceholderText("LED显示内容");
    m_ledColorSpinBox = new ElaSpinBox(this);
    m_ledColorSpinBox->setRange(0, 2);
    m_ledColorSpinBox->setPrefix("颜色 ");
    m_ledColorSpinBox->setValue(1);
    m_ledButton = new ElaPushButton("显示LED", this);

    m_voiceTextEdit = new ElaLineEdit(this);
    m_voiceTextEdit->setPlaceholderText("语音播报内容");
    m_voiceCountSpinBox = new ElaSpinBox(this);
    m_voiceCountSpinBox->setRange(0, 10);
    m_voiceCountSpinBox->setValue(1);
    m_voiceCountSpinBox->setPrefix("次数 ");
    m_voiceIntervalSpinBox = new ElaSpinBox(this);
    m_voiceIntervalSpinBox->setRange(0, 60);
    m_voiceIntervalSpinBox->setValue(3);
    m_voiceIntervalSpinBox->setSuffix("s");
    m_voiceButton = new ElaPushButton("播放语音", this);

    commandLayout->addWidget(createLabel("状态上报", this), 0, 0);
    commandLayout->addWidget(m_uploadUrlEdit, 0, 1, 1, 2);
    commandLayout->addWidget(m_uploadMinutesSpinBox, 0, 3);
    commandLayout->addWidget(m_uploadButton, 0, 4);
    commandLayout->addWidget(createLabel("LED", this), 1, 0);
    commandLayout->addWidget(m_ledTextEdit, 1, 1, 1, 2);
    commandLayout->addWidget(m_ledColorSpinBox, 1, 3);
    commandLayout->addWidget(m_ledButton, 1, 4);
    commandLayout->addWidget(createLabel("语音", this), 2, 0);
    commandLayout->addWidget(m_voiceTextEdit, 2, 1);
    commandLayout->addWidget(m_voiceCountSpinBox, 2, 2);
    commandLayout->addWidget(m_voiceIntervalSpinBox, 2, 3);
    commandLayout->addWidget(m_voiceButton, 2, 4);
    commandLayout->setColumnStretch(1, 1);

    auto *logTitle = createLabel("交互日志", this);
    QFont logTitleFont = logTitle->font();
    logTitleFont.setBold(true);
    logTitle->setFont(logTitleFont);
    m_logClearButton = new ElaPushButton("清除", this);
    auto *logTitleLayout = new QHBoxLayout();
    logTitleLayout->setContentsMargins(0, 0, 0, 0);
    logTitleLayout->addWidget(logTitle);
    logTitleLayout->addWidget(m_logClearButton);
    logTitleLayout->addStretch();
    m_logEdit = new ElaPlainTextEdit(this);
    m_logEdit->setReadOnly(true);
    m_logEdit->setMinimumHeight(150);
    m_logEdit->setPlaceholderText("设备交互日志将在这里显示");

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("车型识别器测试工具");
    auto *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setContentsMargins(0, 5, 5, 0);
    centralLayout->setSpacing(5);
    centralLayout->addWidget(connectionBox);
    centralLayout->addWidget(imageBox);
    centralLayout->addWidget(detailBox);
    centralLayout->addWidget(commandBox);
    centralLayout->addLayout(logTitleLayout);
    centralLayout->addWidget(m_logEdit, 1);
    addCentralWidget(centralWidget, true, true, 0);
}

void T_VehRecognizer::createRecognizer()
{
    if (m_recognizer) {
        destroyVehRecognizer(m_recognizer);
        m_recognizer = nullptr;
    }

    m_recognizer = createVehRecognizer("6701", "福州西", 11);
    m_recognizer->setVersion(uchar(0x01));

    connect(m_recognizer, &IVehRecognizer::sigVehicleTypeInfoReady, this, &T_VehRecognizer::onVehicleTypeInfoReady);
    connect(m_recognizer, &IVehRecognizer::sigVehicleImageInfoReady, this, &T_VehRecognizer::onVehicleImageInfoReady);
    connect(m_recognizer, &IVehRecognizer::sigConnectionStateChanged, this, [this](bool connected) {
        m_connecting = false;
        m_connected = connected;
        m_reconnecting = !connected && !m_userDisconnectRequested;
        setCommandButtonsEnabled(connected);
        if (connected) {
            m_userDisconnectRequested = false;
            m_connectInfoEdit->setEnabled(false);
            m_connectButton->setText("断开");
            updateConnectionStatus("初始化中", StatusTone::Pending);
        } else if (m_userDisconnectRequested) {
            resetConnectionUi();
        } else {
            m_connectInfoEdit->setEnabled(false);
            m_connectButton->setText("停止重连");
            updateConnectionStatus("重连中", StatusTone::Pending);
        }
    });
    connect(m_recognizer, &IVehRecognizer::sigInitStateChanged, this, [this](bool initialized) {
        updateConnectionStatus(initialized ? "已就绪" : "初始化失败", initialized ? StatusTone::Success : StatusTone::Error);
    });
    connect(m_recognizer, &IVehRecognizer::sigCmdFinished, this, [this](uchar type, bool success) {
        QString cmdTypeStr = QString("%1").arg(type, 2, 16, QLatin1Char('0')).toUpper();
        QString tip = QString("指令 0x%1 执行失败或超时").arg(cmdTypeStr);
        if (!success)
            ElaMessageBar::error(ElaMessageBarType::BottomRight, "执行错误", tip, 1500, this);
    });
    connect(m_recognizer, &IVehRecognizer::sigReconnectFailed, this, [this]() {
        resetConnectionUi();
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "连接失败", "自动重连次数已用尽", 1500, this);
    });
}

void T_VehRecognizer::onConnectServer()
{
    if (m_recognizer && (m_connecting || m_connected || m_reconnecting)) {
        m_userDisconnectRequested = true;
        m_recognizer->disconnectServer();
        resetConnectionUi();
        return;
    }

    QString ip;
    quint16 port = 0;
    if (!parseEndpoint(ip, port))
        return;

    m_userDisconnectRequested = false;
    m_connecting = true;
    m_connected = false;
    m_reconnecting = false;
    m_connectInfoEdit->setEnabled(false);
    m_connectButton->setText("取消连接");
    updateConnectionStatus("连接中", StatusTone::Pending);
    m_recognizer->connectServer(ip, port);
}

void T_VehRecognizer::onVehicleTypeInfoReady(const ST_VehicleTypeInfo &vehicleInfo)
{
    m_plateText->setText(vehicleInfo.vehPlate);
    m_plateColorText->setText(QString::number(vehicleInfo.plateColor));
    m_timeText->setText(vehicleInfo.vehTime.toString("yyyy-MM-dd HH:mm:ss"));
    m_vehicleClassText->setText(BizUtils::getVehClassName(vehicleInfo.vehClass));
    m_axleTypeText->setText(vehicleInfo.axleType.trimmed());
    m_axleCountText->setText(QString::number(vehicleInfo.axleCount));
    m_dimensionText->setText(QString("%1 × %2 × %3").arg(vehicleInfo.totalLength).arg(vehicleInfo.totalWidth).arg(vehicleInfo.totalHeight));
    m_extFlagText->setText(QString("%1").arg(vehicleInfo.extFlag, 8, 16, QLatin1Char('0')));
    m_directionText->setText(QString::number(vehicleInfo.direction));
}

void T_VehRecognizer::onVehicleImageInfoReady(const ST_VehicleImageInfo &imageInfo)
{
    // 视频信息页面不展示
    if (imageInfo.imageType == 4)
        return;

    const QString imageInfoText = QString("%1  %2").arg(imageInfo.vehTime.toString("HH:mm:ss"), imageInfo.vehPlate);

    switch (imageInfo.imageType) {
    case 1:
        showImage(m_headImage, imageInfo.imagePath);
        m_headImageInfo->setText(imageInfoText);
        break;
    case 2:
        showImage(m_tailImage, imageInfo.imagePath);
        m_tailImageInfo->setText(imageInfoText);
        break;
    case 3:
        showImage(m_bodyImage, imageInfo.imagePath);
        m_bodyImageInfo->setText(imageInfoText);
        break;
    default:
        break;
    }
}

void T_VehRecognizer::onSetUploadUrl()
{
    const QString urlText = m_uploadUrlEdit->text().trimmed();
    const QUrl url(urlText);
    if (urlText.isEmpty() || !url.isValid() || url.scheme().isEmpty() || url.host().isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight, "参数有误", "请输入有效的状态上报URL", 1500, this);
        return;
    }
    m_recognizer->setUploadUrl(url.toString(), static_cast<uchar>(m_uploadMinutesSpinBox->value()));
}

void T_VehRecognizer::onShowLed()
{
    const QString text = m_ledTextEdit->text().trimmed();
    if (text.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight, "参数有误", "请输入LED显示内容", 1500, this);
        return;
    }
    m_recognizer->showLED(static_cast<uchar>(m_ledColorSpinBox->value()), text);
}

void T_VehRecognizer::onPlayVoice()
{
    const QString text = m_voiceTextEdit->text().trimmed();
    if (text.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight, "参数有误", "请输入语音播报内容", 1500, this);
        return;
    }
    m_recognizer->playVoice(static_cast<uchar>(m_voiceCountSpinBox->value()), text, m_voiceIntervalSpinBox->value() * 1000);
}

bool T_VehRecognizer::parseEndpoint(QString &ip, quint16 &port)
{
    const QStringList parts = m_connectInfoEdit->text().trimmed().split(':');
    if (parts.size() != 2) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight, "参数有误", "服务地址格式应为 IP:端口", 1500, this);
        return false;
    }

    QHostAddress address;
    ip = parts.at(0).trimmed();
    if (!address.setAddress(ip)) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight, "参数有误", "IP地址格式错误", 1500, this);
        return false;
    }

    bool ok = false;
    const uint value = parts.at(1).trimmed().toUInt(&ok);
    if (!ok || value == 0 || value > 65535) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight, "参数有误", "端口范围应为1-65535", 1500, this);
        return false;
    }
    port = static_cast<quint16>(value);
    return true;
}

void T_VehRecognizer::resetConnectionUi()
{
    m_connecting = false;
    m_connected = false;
    m_reconnecting = false;
    m_userDisconnectRequested = false;
    setCommandButtonsEnabled(false);
    m_connectInfoEdit->setEnabled(true);
    m_connectButton->setText("连接");
    updateConnectionStatus("未连接", StatusTone::Error);
}

void T_VehRecognizer::updateConnectionStatus(const QString &text, StatusTone tone)
{
    m_connectionStatusText->setText(text);
    const char *color = tone == StatusTone::Success ? "#28bf74" : (tone == StatusTone::Pending ? "#d98c00" : "#ff0000");
    m_connectionStatusText->setStyleSheet(QString("color: %1").arg(color));
}

void T_VehRecognizer::showImage(QLabel *label, const QString &path)
{
    QPixmap pixmap(path);
    if (pixmap.isNull()) {
        label->setPixmap(QPixmap());
        label->setText("图片加载失败\n" + path);
        return;
    }
    label->setText(QString());
    label->setPixmap(pixmap.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    label->setToolTip(path);
}

void T_VehRecognizer::resetVehicleWidgets()
{
    const QList<QLabel *> images = {m_headImage, m_tailImage, m_bodyImage};
    for (QLabel *image : images) {
        image->setPixmap(QPixmap());
        image->setText("等待车辆图片");
        image->setToolTip(QString());
    }
    const QList<ElaText *> imageInfos = {m_headImageInfo, m_tailImageInfo, m_bodyImageInfo};
    for (ElaText *imageInfo : imageInfos)
        imageInfo->setText("-");

    const QList<ElaText *> fields = {m_plateText,     m_plateColorText, m_timeText,    m_vehicleClassText, m_axleTypeText,
                                     m_axleCountText, m_dimensionText,  m_extFlagText, m_directionText};
    for (ElaText *field : fields)
        field->setText("-");
}

void T_VehRecognizer::setCommandButtonsEnabled(bool enabled)
{
    m_uploadButton->setEnabled(enabled);
    m_ledButton->setEnabled(enabled);
    m_voiceButton->setEnabled(enabled);
}
