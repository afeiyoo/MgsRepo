#include "t_mobileplusterminal.h"

#include <QFile>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHostAddress>
#include <QUrl>
#include <QVBoxLayout>

#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaMessageBar.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"
#include "ElaSpinBox.h"
#include "ElaText.h"
#include "Logger.h"
#include "imobileplusterminal.h"
#include "utils/datadealutils.h"

using namespace Utils;

namespace {
constexpr qint64 MAX_PICTURE_SIZE = 1 * 1024 * 1024;

ElaText *createLabel(const QString &text, QWidget *parent = nullptr)
{
    auto *label = new ElaText(text, parent);
    label->setTextPixelSize(14);
    label->setWordWrap(false);
    return label;
}
} // namespace

T_MobilePlusTerminal::T_MobilePlusTerminal(QWidget *parent)
    : T_BasePage{parent}
{
    setWindowTitle("手机+自助终端测试工具");
    createCustomWidget("测试手机+自助终端的连接、展码、LED、图片显示及状态上传配置");
    initContent();

    connect(m_connectButton, &ElaPushButton::clicked, this, &T_MobilePlusTerminal::onConnectServer);
    connect(m_qrCodeButton, &ElaPushButton::clicked, this, &T_MobilePlusTerminal::onShowQrCode);
    connect(m_ledButton, &ElaPushButton::clicked, this, &T_MobilePlusTerminal::onShowLed);
    connect(m_pictureSelectButton, &ElaPushButton::clicked, this, &T_MobilePlusTerminal::onSelectPicture);
    connect(m_pictureButton, &ElaPushButton::clicked, this, &T_MobilePlusTerminal::onShowPicture);
    connect(m_uploadUrlButton, &ElaPushButton::clicked, this, &T_MobilePlusTerminal::onSetUploadUrl);
    connect(m_resetDisplayButton, &ElaPushButton::clicked, this, &T_MobilePlusTerminal::onResetDisplay);
    connect(m_logClearButton, &ElaPushButton::clicked, m_logEdit, &ElaPlainTextEdit::clear);

    connect(cuteLogger, &Logger::sigLogWrite, this, [this](Logger::LogLevel, const QString &log, const QString &category) {
        if (category == "MobilePlusTerminal")
            m_logEdit->appendPlainText(DataDealUtils::curDateTimeStr() + " | " + log + "\n");
    });

    setCommandButtonsEnabled(false);
}

T_MobilePlusTerminal::~T_MobilePlusTerminal()
{
    if (m_terminal)
        destroyMobilePlusTerminal(m_terminal);
}

void T_MobilePlusTerminal::initContent()
{
    auto *connectionBox = new QGroupBox("设备初始化", this);
    auto *connectionLayout = new QVBoxLayout(connectionBox);
    connectionLayout->setContentsMargins(12, 12, 12, 10);
    connectionLayout->setSpacing(8);

    m_connectInfoEdit = new ElaLineEdit(this);
    m_connectInfoEdit->setPlaceholderText("IP地址:端口，例如 192.168.1.10:9000");
    m_connectInfoEdit->setText("127.0.0.1:9588");
    m_stationIdEdit = new ElaLineEdit(this);
    m_stationIdEdit->setPlaceholderText("站编号");
    m_laneIdSpinBox = new ElaSpinBox(this);
    m_laneIdSpinBox->setRange(0, 99);
    m_laneIdSpinBox->setValue(1);
    m_deviceSeqSpinBox = new ElaSpinBox(this);
    m_deviceSeqSpinBox->setRange(0, 99);
    m_deviceSeqSpinBox->setValue(1);
    m_versionComboBox = new ElaComboBox(this);
    m_versionComboBox->addItem("0x01", 1);
    m_connectButton = new ElaPushButton("连接", this);
    m_resetDisplayButton = new ElaPushButton("重置界面", this);
    m_connectionStatusText = createLabel("未连接", this);
    m_connectionStatusText->setStyleSheet("color: #ff0000");
    m_helpTypeText = createLabel("-", this);

    auto *endpointLayout = new QHBoxLayout();
    endpointLayout->setContentsMargins(0, 0, 0, 0);
    endpointLayout->setSpacing(8);
    endpointLayout->addWidget(createLabel("服务地址", this));
    endpointLayout->addWidget(m_connectInfoEdit, 1);
    endpointLayout->addWidget(m_connectButton);
    endpointLayout->addWidget(m_resetDisplayButton);
    endpointLayout->addSpacing(16);
    endpointLayout->addWidget(createLabel("设备状态", this));
    endpointLayout->addWidget(m_connectionStatusText);
    endpointLayout->addSpacing(16);
    endpointLayout->addWidget(createLabel("求助类型", this));
    endpointLayout->addWidget(m_helpTypeText);

    auto *deviceLayout = new QHBoxLayout();
    deviceLayout->setContentsMargins(0, 0, 0, 0);
    deviceLayout->setSpacing(8);
    deviceLayout->addWidget(createLabel("站编号", this));
    deviceLayout->addWidget(m_stationIdEdit, 1);
    deviceLayout->addSpacing(12);
    deviceLayout->addWidget(createLabel("车道号", this));
    deviceLayout->addWidget(m_laneIdSpinBox);
    deviceLayout->addSpacing(12);
    deviceLayout->addWidget(createLabel("设备序号", this));
    deviceLayout->addWidget(m_deviceSeqSpinBox);
    deviceLayout->addSpacing(12);
    deviceLayout->addWidget(createLabel("协议版本", this));
    deviceLayout->addWidget(m_versionComboBox);

    connectionLayout->addLayout(endpointLayout);
    connectionLayout->addLayout(deviceLayout);

    auto *qrCodeBox = new QGroupBox("二维码显示", this);
    auto *qrCodeLayout = new QGridLayout(qrCodeBox);
    qrCodeLayout->setSpacing(8);
    m_stationNameEdit = new ElaLineEdit(this);
    m_stationNameEdit->setPlaceholderText("收费站名称");
    m_vehicleClassEdit = new ElaLineEdit(this);
    m_vehicleClassEdit->setPlaceholderText("客一");
    m_vehiclePlateEdit = new ElaLineEdit(this);
    m_vehiclePlateEdit->setPlaceholderText("车牌号码");
    m_barCodeEdit = new ElaLineEdit(this);
    m_barCodeEdit->setPlaceholderText("二维码内容");
    m_qrCodeButton = new ElaPushButton("发送展码", this);

    qrCodeLayout->addWidget(createLabel("收费站", this), 0, 0);
    qrCodeLayout->addWidget(m_stationNameEdit, 0, 1);
    qrCodeLayout->addWidget(createLabel("车型", this), 0, 2);
    qrCodeLayout->addWidget(m_vehicleClassEdit, 0, 3);
    qrCodeLayout->addWidget(createLabel("车牌", this), 1, 0);
    qrCodeLayout->addWidget(m_vehiclePlateEdit, 1, 1);
    qrCodeLayout->addWidget(createLabel("二维码", this), 1, 2);
    qrCodeLayout->addWidget(m_barCodeEdit, 1, 3);
    qrCodeLayout->addWidget(m_qrCodeButton, 0, 4, 2, 1);
    qrCodeLayout->setColumnStretch(1, 1);
    qrCodeLayout->setColumnStretch(3, 2);

    auto *commandBox = new QGroupBox("设备指令", this);
    auto *commandLayout = new QGridLayout(commandBox);
    commandLayout->setSpacing(8);
    m_ledTextEdit = new ElaLineEdit(this);
    m_ledTextEdit->setPlaceholderText("请输入LED显示内容");
    m_ledButton = new ElaPushButton("显示LED", this);
    m_picturePathEdit = new ElaLineEdit(this);
    m_picturePathEdit->setReadOnly(true);
    m_picturePathEdit->setPlaceholderText("请选择不超过2MB的图片");
    m_pictureSelectButton = new ElaPushButton("选择图片", this);
    m_pictureButton = new ElaPushButton("显示图片", this);
    m_uploadUrlEdit = new ElaLineEdit(this);
    m_uploadUrlEdit->setPlaceholderText("例如 http://127.0.0.1/status");
    m_uploadIntervalSpinBox = new ElaSpinBox(this);
    m_uploadIntervalSpinBox->setRange(1, 3600);
    m_uploadIntervalSpinBox->setValue(10);
    m_uploadIntervalSpinBox->setSuffix("s");
    m_uploadUrlButton = new ElaPushButton("设置上传", this);

    commandLayout->addWidget(createLabel("LED文字", this), 0, 0);
    commandLayout->addWidget(m_ledTextEdit, 0, 1, 1, 3);
    commandLayout->addWidget(m_ledButton, 0, 4);
    commandLayout->addWidget(createLabel("显示图片", this), 1, 0);
    commandLayout->addWidget(m_picturePathEdit, 1, 1, 1, 2);
    commandLayout->addWidget(m_pictureSelectButton, 1, 3);
    commandLayout->addWidget(m_pictureButton, 1, 4);
    commandLayout->addWidget(createLabel("状态上传", this), 2, 0);
    commandLayout->addWidget(m_uploadUrlEdit, 2, 1, 1, 2);
    commandLayout->addWidget(m_uploadIntervalSpinBox, 2, 3);
    commandLayout->addWidget(m_uploadUrlButton, 2, 4);
    commandLayout->setColumnStretch(1, 1);
    commandLayout->setColumnStretch(2, 1);

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
    m_logEdit->setPlaceholderText("设备交互日志将在这里显示");
    m_logEdit->setMinimumHeight(150);

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("手机+自助终端测试工具");
    auto *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setContentsMargins(0, 5, 5, 0);
    centralLayout->setSpacing(5);
    centralLayout->addWidget(connectionBox);
    centralLayout->addWidget(qrCodeBox);
    centralLayout->addWidget(commandBox);
    centralLayout->addLayout(logTitleLayout);
    centralLayout->addWidget(m_logEdit, 1);
    addCentralWidget(centralWidget, true, true, 0);
}

void T_MobilePlusTerminal::createTerminal()
{
    if (m_terminal) {
        destroyMobilePlusTerminal(m_terminal);
        m_terminal = nullptr;
    }

    m_terminal = createMobilePlusTerminal(m_stationIdEdit->text().trimmed(), static_cast<uint>(m_laneIdSpinBox->value()),
                                          static_cast<uint>(m_deviceSeqSpinBox->value()));
    if (!m_terminal) {
        showInputError("设备参数无效，无法创建设备对象");
        return;
    }

    // 设置协议版本号
    m_terminal->setVersion(static_cast<uchar>(m_versionComboBox->currentData().toUInt()));

    connect(m_terminal, &IMobilePlusTerminal::sigRequestHelp, this,
            [this](uint, int helpType) { m_helpTypeText->setText(QString::number(helpType)); });
    connect(m_terminal, &IMobilePlusTerminal::sigConnectionStateChanged, this, [this](uint, bool connected) {
        m_connecting = false;
        m_connected = connected;
        m_reconnecting = !connected && !m_userDisconnectRequested;
        setCommandButtonsEnabled(false); // 初始化成功后才能发指令

        if (connected) {
            m_userDisconnectRequested = false;
            setConnectionFieldsEnabled(false);
            m_connectButton->setText("断开");
            updateConnectionStatus("初始化中", StatusTone::Pending);
        } else if (m_userDisconnectRequested) {
            resetConnectionUi();
        } else {
            setConnectionFieldsEnabled(false);
            m_connectButton->setText("停止重连");
            updateConnectionStatus("重连中", StatusTone::Pending);
        }
    });
    connect(m_terminal, &IMobilePlusTerminal::sigInitStateChanged, this, [this](uint, bool initialized) {
        setCommandButtonsEnabled(initialized);
        updateConnectionStatus(initialized ? "已就绪" : "初始化失败", initialized ? StatusTone::Success : StatusTone::Error);
    });
    connect(m_terminal, &IMobilePlusTerminal::sigCmdFinished, this, [this](uint, uchar type, bool success) {
        static const QStringList commandNames = {"设备初始化", "二维码显示", "LED显示", "图片显示", "状态上传配置", "界面重置"};
        const QString name = type < commandNames.size() ? commandNames.at(type) : QString("Type %1").arg(type);
        const QString title = success ? "指令成功" : "指令失败";
        if (success)
            ElaMessageBar::success(ElaMessageBarType::BottomRight, title, name + "执行成功", 1200, this);
        else
            ElaMessageBar::error(ElaMessageBarType::BottomRight, title, name + "执行失败或响应超时", 1800, this);
    });
    connect(m_terminal, &IMobilePlusTerminal::sigReconnectFailed, this, [this](uint) {
        m_connecting = false;
        m_connected = false;
        m_reconnecting = false;
        m_userDisconnectRequested = false;
        setConnectionFieldsEnabled(true);
        setCommandButtonsEnabled(false);
        m_connectButton->setText("连接");
        updateConnectionStatus("未连接", StatusTone::Error);
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "连接失败", "自动重连次数已用尽", 1800, this);
    });
}

void T_MobilePlusTerminal::onConnectServer()
{
    if (m_terminal && (m_connecting || m_connected || m_reconnecting)) {
        m_userDisconnectRequested = true;
        m_terminal->disconnectServer();
        resetConnectionUi();
        return;
    }
    if (m_stationIdEdit->text().trimmed().isEmpty()) {
        showInputError("请输入站编号");
        return;
    }

    QString ip;
    quint16 port = 0;
    if (!parseEndpoint(ip, port))
        return;

    createTerminal();
    if (!m_terminal)
        return;

    m_userDisconnectRequested = false;
    m_connecting = true;
    m_connected = false;
    m_reconnecting = false;
    updateConnectionStatus("连接中", StatusTone::Pending);
    setConnectionFieldsEnabled(false);
    m_connectButton->setText("取消连接");
    m_terminal->connectServer(ip, port);
}

void T_MobilePlusTerminal::onShowQrCode()
{
    const QString stationName = m_stationNameEdit->text().trimmed();
    const QString vehicleClass = m_vehicleClassEdit->text().trimmed();
    const QString vehiclePlate = m_vehiclePlateEdit->text().trimmed();
    const QString barCode = m_barCodeEdit->text().trimmed();
    if (stationName.isEmpty() || vehicleClass.isEmpty() || vehiclePlate.isEmpty() || barCode.isEmpty()) {
        showInputError("请完整填写收费站、车型、车牌和二维码内容");
        return;
    }
    m_terminal->showQRCode(stationName, vehicleClass, vehiclePlate, barCode);
}

void T_MobilePlusTerminal::onShowLed()
{
    const QString text = m_ledTextEdit->text().trimmed();
    if (text.isEmpty()) {
        showInputError("请输入LED显示内容");
        return;
    }
    m_terminal->showLED(text);
}

void T_MobilePlusTerminal::onSelectPicture()
{
    const QString path = QFileDialog::getOpenFileName(this, "选择显示图片", QString(), "图片文件 (*.png *.jpg *.jpeg *.bmp);;所有文件 (*.*)");
    if (!path.isEmpty())
        m_picturePathEdit->setText(path);
}

void T_MobilePlusTerminal::onShowPicture()
{
    QFile file(m_picturePathEdit->text());
    if (!file.exists()) {
        showInputError("请选择有效的图片文件");
        return;
    }
    if (file.size() > MAX_PICTURE_SIZE) {
        showInputError("图片不能超过1MB");
        return;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        showInputError("图片文件读取失败");
        return;
    }
    m_terminal->showPics(file.readAll());
}

void T_MobilePlusTerminal::onSetUploadUrl()
{
    const QString urlText = m_uploadUrlEdit->text().trimmed();
    const QUrl url = QUrl::fromUserInput(urlText);
    if (urlText.isEmpty() || !url.isValid() || url.scheme().isEmpty() || url.host().isEmpty()) {
        showInputError("请输入有效的状态上传URL");
        return;
    }
    m_terminal->setUploadUrl(url.toString(), m_uploadIntervalSpinBox->value());
}

void T_MobilePlusTerminal::onResetDisplay()
{
    m_terminal->resetDisplay();
}

void T_MobilePlusTerminal::setConnectionFieldsEnabled(bool enabled)
{
    m_connectInfoEdit->setEnabled(enabled);
    m_stationIdEdit->setEnabled(enabled);
    m_laneIdSpinBox->setEnabled(enabled);
    m_deviceSeqSpinBox->setEnabled(enabled);
    m_versionComboBox->setEnabled(enabled);
}

void T_MobilePlusTerminal::setCommandButtonsEnabled(bool enabled)
{
    m_qrCodeButton->setEnabled(enabled);
    m_ledButton->setEnabled(enabled);
    m_pictureButton->setEnabled(enabled);
    m_uploadUrlButton->setEnabled(enabled);
    m_resetDisplayButton->setEnabled(enabled);
}

void T_MobilePlusTerminal::updateConnectionStatus(const QString &text, StatusTone tone)
{
    m_connectionStatusText->setText(text);
    switch (tone) {
    case StatusTone::Success:
        m_connectionStatusText->setStyleSheet("color: #28bf74");
        break;
    case StatusTone::Pending:
        m_connectionStatusText->setStyleSheet("color: #d98c00");
        break;
    case StatusTone::Error:
        m_connectionStatusText->setStyleSheet("color: #ff0000");
        break;
    }
}

void T_MobilePlusTerminal::resetConnectionUi()
{
    m_connecting = false;
    m_connected = false;
    m_reconnecting = false;
    setConnectionFieldsEnabled(true);
    setCommandButtonsEnabled(false);
    m_connectButton->setText("连接");
    updateConnectionStatus("未连接", StatusTone::Error);
    m_helpTypeText->setText("-");
}

bool T_MobilePlusTerminal::parseEndpoint(QString &ip, quint16 &port)
{
    const QStringList parts = m_connectInfoEdit->text().trimmed().split(':');
    if (parts.size() != 2) {
        showInputError("服务地址格式应为 IP:端口");
        return false;
    }

    QHostAddress address;
    ip = parts.at(0).trimmed();
    if (!address.setAddress(ip)) {
        showInputError("IP地址格式错误");
        return false;
    }

    bool ok = false;
    const uint value = parts.at(1).trimmed().toUInt(&ok);
    if (!ok || value == 0 || value > 65535) {
        showInputError("端口范围应为1-65535");
        return false;
    }
    port = static_cast<quint16>(value);
    return true;
}

void T_MobilePlusTerminal::showInputError(const QString &message)
{
    ElaMessageBar::warning(ElaMessageBarType::BottomRight, "参数有误", message, 1500, this);
}
