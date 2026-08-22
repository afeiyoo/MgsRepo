#include "t_smartcontroller.h"

#include <QButtonGroup>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHostAddress>
#include <QSignalBlocker>
#include <QUrl>
#include <QVBoxLayout>

#include "ElaLineEdit.h"
#include "ElaMessageBar.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"
#include "ElaRadioButton.h"
#include "ElaSpinBox.h"
#include "ElaText.h"
#include "ElaToggleButton.h"
#include "Logger.h"
#include "ismartlanecontroller.h"
#include "utils/datadealutils.h"

using namespace Utils;

namespace {
ElaText *createLabel(const QString &text, QWidget *parent = nullptr)
{
    auto *label = new ElaText(text, parent);
    label->setTextPixelSize(14);
    label->setWordWrap(false);
    return label;
}

ElaText *createSectionTitle(const QString &text, QWidget *parent = nullptr)
{
    auto *title = createLabel(text, parent);
    QFont font = title->font();
    font.setBold(true);
    title->setFont(font);
    return title;
}
} // namespace

T_SmartController::T_SmartController(QWidget *parent)
    : T_BasePage{parent}
{
    setWindowTitle("智能网关测试工具");
    createCustomWidget("测试智能网关（基于TCP）的指令收发");
    initContent();

    connect(m_connectButton, &ElaPushButton::clicked, this, &T_SmartController::onConnectServer);
    connect(m_sendA2Button, &ElaPushButton::clicked, this, &T_SmartController::onSendA2);
    connect(m_logClearButton, &ElaPushButton::clicked, m_logEdit, &ElaPlainTextEdit::clear);

    connect(m_inputModeButton, &ElaRadioButton::toggled, this, &T_SmartController::onModeToggled);
    connect(m_outputModeButton, &ElaRadioButton::toggled, this, &T_SmartController::onModeToggled);
    connect(m_offsetButton, &ElaToggleButton::toggled, this, &T_SmartController::onOffsetToggled);
    for (int i = 0; i < m_controlButtons.size(); ++i)
        connect(m_controlButtons.at(i), &ElaToggleButton::toggled, this, [this, i](bool checked) { onControlToggled(i, checked); });
    for (int i = 0; i < m_levelButtons.size(); ++i)
        connect(m_levelButtons.at(i), &ElaToggleButton::toggled, this, [this, i](bool checked) { onLevelToggled(i, checked); });

    connect(cuteLogger, &Logger::sigLogWrite, this, [this](Logger::LogLevel, const QString &log, const QString &category) {
        if (category == "SmartLaneController")
            m_logEdit->appendPlainText(DataDealUtils::curDateTimeStr() + " | " + log + "\n");
    });

    setConnectionState(ConnectionState::Disconnected);
    initSmartController();
}

T_SmartController::~T_SmartController()
{
    if (m_smartController)
        destroySmartLaneController(m_smartController);
}

void T_SmartController::initSmartController()
{
    m_smartController = createSmartLaneController();
    m_smartController->setVersion(0x00);

    connect(m_smartController, &ISmartLaneController::sigRecvD2Cmd, this, &T_SmartController::onRecvD2Cmd);
    connect(m_smartController, &ISmartLaneController::sigConnectionStateChanged, this, &T_SmartController::onConnectionStateChanged);
    connect(m_smartController, &ISmartLaneController::sigHeartbeatStateChanged, this, &T_SmartController::onHeartbeatStateChanged);
    connect(m_smartController, &ISmartLaneController::sigReconnectFailed, this, &T_SmartController::onReconnectFailed);
}

void T_SmartController::initContent()
{
    // 设备连接
    auto *connectionBox = new QGroupBox("设备连接", this);
    auto *connectionLayout = new QHBoxLayout(connectionBox);
    connectionLayout->setContentsMargins(12, 8, 12, 8);
    connectionLayout->setSpacing(8);

    m_connectInfoEdit = new ElaLineEdit(this);
    m_connectInfoEdit->setPlaceholderText("IP地址:端口，例如 127.0.0.1:9588");
    m_connectButton = new ElaPushButton("连接", this);
    m_connectionStatusText = createLabel("未连接", this);
    m_connectionStatusText->setStyleSheet("color: #ff0000");
    m_heartBeatStatusText = createLabel("心跳异常", this);
    m_heartBeatStatusText->setStyleSheet("color: #ff0000");

    connectionLayout->addWidget(m_connectInfoEdit, 1);
    connectionLayout->addWidget(m_connectButton);
    connectionLayout->addSpacing(16);
    connectionLayout->addWidget(createLabel("连接状态", this));
    connectionLayout->addWidget(m_connectionStatusText);
    connectionLayout->addWidget(createLabel("心跳状态", this));
    connectionLayout->addWidget(m_heartBeatStatusText);

    // 状态上传配置
    auto *a2Box = new QGroupBox("状态上传配置", this);
    auto *a2Layout = new QHBoxLayout(a2Box);
    a2Layout->setContentsMargins(12, 8, 12, 8);
    a2Layout->setSpacing(8);
    m_uploadUrlEdit = new ElaLineEdit(this);
    m_uploadUrlEdit->setPlaceholderText("例如 http://192.168.0.10:8080/device/status");
    m_uploadIntervalSpinBox = new ElaSpinBox(this);
    m_uploadIntervalSpinBox->setRange(1, 60);
    m_uploadIntervalSpinBox->setValue(1);
    m_uploadIntervalSpinBox->setSuffix(" min");
    m_sendA2Button = new ElaPushButton("发送", this);
    a2Layout->addWidget(createLabel("上传URL", this));
    a2Layout->addWidget(m_uploadUrlEdit, 1);
    a2Layout->addWidget(createLabel("上传间隔", this));
    a2Layout->addWidget(m_uploadIntervalSpinBox);
    a2Layout->addWidget(m_sendA2Button);

    // 输入输出控制
    auto *ioLayout = new QVBoxLayout();
    ioLayout->setContentsMargins(0, 8, 0, 8);
    ioLayout->setSpacing(8);

    // - 标题及模式选择
    m_inputModeButton = new ElaRadioButton("输入", this);
    m_outputModeButton = new ElaRadioButton("输出", this);
    m_outputModeButton->setChecked(true);
    auto *modeGroup = new QButtonGroup(this);
    modeGroup->addButton(m_inputModeButton);
    modeGroup->addButton(m_outputModeButton);

    auto *ioTitleLayout = new QHBoxLayout();
    ioTitleLayout->setContentsMargins(0, 0, 0, 0);
    ioTitleLayout->setSpacing(8);
    ioTitleLayout->addWidget(createSectionTitle("输入/输出控制", this));
    ioTitleLayout->addStretch();
    ioTitleLayout->addWidget(createLabel("模式", this));
    ioTitleLayout->addWidget(m_inputModeButton);
    ioTitleLayout->addWidget(m_outputModeButton);
    ioLayout->addLayout(ioTitleLayout);

    // - 偏移位
    auto *offsetLayout = new QHBoxLayout();
    offsetLayout->setContentsMargins(0, 0, 0, 0);
    offsetLayout->setSpacing(8);

    m_offsetButton = new ElaToggleButton("0", this);
    m_offsetButton->setFixedWidth(60);
    offsetLayout->addWidget(createLabel("偏移位(第1位)", this));
    offsetLayout->addWidget(m_offsetButton);
    offsetLayout->addStretch();
    ioLayout->addLayout(offsetLayout);

    // - 控制位
    auto *controlLayout = new QHBoxLayout();
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->setSpacing(8);

    for (int i = 0; i < 8; ++i) {
        auto *button = new ElaToggleButton(QString::number(i + 1), this);
        button->setFixedWidth(60);
        m_controlButtons.append(button);
        controlLayout->addWidget(button);
    }
    controlLayout->addStretch();

    controlLayout->insertWidget(0, createLabel("控制位(第2位)", this));
    ioLayout->addLayout(controlLayout);

    // - 电平位
    auto *levelLayout = new QHBoxLayout();
    levelLayout->setContentsMargins(0, 0, 0, 0);
    levelLayout->setSpacing(8);

    for (int i = 0; i < 2; ++i) {
        auto *button = new ElaToggleButton(QString::number(i), this);
        button->setFixedWidth(60);
        m_levelButtons.append(button);
        levelLayout->addWidget(button);
    }
    levelLayout->addStretch();

    levelLayout->insertWidget(0, createLabel("电平位(第3位)", this));
    ioLayout->addLayout(levelLayout);

    // 日志区
    auto *logTitle = createSectionTitle("交互日志", this);
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
    centralWidget->setWindowTitle("智能网关测试工具");
    auto *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setContentsMargins(0, 5, 5, 0);
    centralLayout->setSpacing(5);
    centralLayout->addWidget(connectionBox);
    centralLayout->addWidget(a2Box);
    centralLayout->addLayout(ioLayout);
    centralLayout->addLayout(logTitleLayout);
    centralLayout->addWidget(m_logEdit, 1);
    addCentralWidget(centralWidget, true, true, 0);
}

void T_SmartController::onConnectServer()
{
    if (!m_smartController)
        return;

    // 断开连接
    if (m_connectionState != ConnectionState::Disconnected) {
        m_userDisconnectRequested = true;
        m_smartController->disconnectServer();
        setConnectionState(ConnectionState::Disconnected);
        return;
    }

    QString ip;
    quint16 port = 0;
    if (!parseEndpoint(ip, port))
        return;

    m_userDisconnectRequested = false;
    setConnectionState(ConnectionState::Connecting);
    m_smartController->connectServer(ip, port);
}

void T_SmartController::onConnectionStateChanged(bool connected)
{
    if (connected) {
        m_userDisconnectRequested = false;
        setConnectionState(ConnectionState::Connected);
        return;
    }

    if (m_userDisconnectRequested) {
        setConnectionState(ConnectionState::Disconnected);
        return;
    }

    setConnectionState(ConnectionState::Reconnecting);
}

void T_SmartController::onHeartbeatStateChanged(bool normal)
{
    updateHeartbeatStatus(normal ? "心跳正常" : "心跳异常", normal ? StatusTone::Success : StatusTone::Error);
}

void T_SmartController::onReconnectFailed()
{
    m_userDisconnectRequested = false;
    setConnectionState(ConnectionState::Disconnected);
    ElaMessageBar::error(ElaMessageBarType::BottomRight, "连接失败", "与智能网关断开连接", 1500, this);
}

void T_SmartController::onModeToggled(bool checked)
{
    if (!checked)
        return;
    refreshControlButtons();
}

void T_SmartController::onOffsetToggled(bool checked)
{
    if (!isOutputMode())
        return;

    m_outputOffset = checked ? 1 : 0;

    sendA1Command();
}

void T_SmartController::onControlToggled(int index, bool checked)
{
    if (!isOutputMode())
        return;

    const quint16 mask = static_cast<quint16>(1u << index);
    if (checked) {
        m_outputStatus = static_cast<quint16>(m_outputStatus | mask);
    } else {
        m_outputStatus = static_cast<quint16>(m_outputStatus & ~mask);
    }
    sendA1Command();
}

void T_SmartController::onLevelToggled(int level, bool checked)
{
    if (!isOutputMode())
        return;

    if (checked) {
        const int otherLevel = level == 0 ? 1 : 0;
        const QSignalBlocker blocker(m_levelButtons.at(otherLevel));
        m_levelButtons.at(otherLevel)->setIsToggled(false); // 熄灭另一个电平按钮
        m_outputLevel = level;
    } else if (m_outputLevel == level) {
        m_outputLevel = -1;
    }
    sendA1Command();
}

void T_SmartController::refreshControlButtons()
{
    const bool outputMode = isOutputMode();
    const bool connected = isConnected();
    const quint16 status = outputMode ? m_outputStatus : m_inputStatus;
    const bool showInputConfiguration = !outputMode && m_hasInputStatus && m_inputStatus != 0;

    // 控制位
    for (int i = 0; i < m_controlButtons.size(); ++i) {
        auto *button = m_controlButtons.at(i);
        const QSignalBlocker blocker(button);
        button->setText(QString::number(outputMode ? i + 1 : i));

        button->setIsToggled((status & static_cast<quint16>(1u << i)) != 0);
        setToggleReadOnly(button, !outputMode ? true : !connected);
    }

    // 电平位
    for (int i = 0; i < m_levelButtons.size(); ++i) {
        auto *button = m_levelButtons.at(i);
        const QSignalBlocker blocker(button);
        const bool selected = outputMode ? m_outputLevel == i : showInputConfiguration && i == 1;

        button->setIsToggled(selected);
        setToggleReadOnly(button, !outputMode ? true : !connected);
    }

    // 偏移位
    const QSignalBlocker blocker(m_offsetButton);
    m_offsetButton->setIsToggled(outputMode ? m_outputOffset == 1 : showInputConfiguration);
    setToggleReadOnly(m_offsetButton, !outputMode ? true : !connected);
}

bool T_SmartController::isConnected() const
{
    return m_connectionState == ConnectionState::Connected;
}

bool T_SmartController::isOutputMode() const
{
    return m_outputModeButton->isChecked();
}

void T_SmartController::sendA1Command()
{
    // 已连接，输出模式下，电平位，偏移位有点亮，按下
    if (!isConnected() || !isOutputMode() || m_outputOffset == 0 || m_outputLevel == -1)
        return;
    m_smartController->sendA1Cmd(relayMap(), levelMap());
}

void T_SmartController::onSendA2()
{
    if (!isConnected())
        return;

    const QString urlText = m_uploadUrlEdit->text().trimmed();
    const QUrl url = QUrl::fromUserInput(urlText);
    if (urlText.isEmpty() || !url.isValid() || url.scheme().isEmpty() || url.host().isEmpty()) {
        showInputError("请输入有效的上传URL");
        return;
    }

    m_smartController->sendA2Cmd(urlText.toUtf8(), static_cast<uchar>(m_uploadIntervalSpinBox->value()));
}

void T_SmartController::onRecvD2Cmd(QByteArray cmd)
{
    const quint16 high = static_cast<uchar>(cmd.at(1));
    const quint16 low = static_cast<uchar>(cmd.at(2));
    const quint16 status = static_cast<quint16>((high << 8) | low);
    m_hasInputStatus = true;
    m_inputStatus = static_cast<quint16>(status & 0x00FF); // 目前输入只用到8路，故使用0x00FF

    // 处于输入模式下，则刷新控制按钮
    if (!isOutputMode())
        refreshControlButtons();
}

void T_SmartController::setConnectionState(ConnectionState state)
{
    m_connectionState = state;
    if (state == ConnectionState::Disconnected || state == ConnectionState::Reconnecting)
        resetIoControls();

    // 界面上控制按钮刷新
    setCommandControlsEnabled(state == ConnectionState::Connected);

    // 未连接状态下可以修改连接信息
    m_connectInfoEdit->setEnabled(state == ConnectionState::Disconnected);

    switch (state) {
    case ConnectionState::Disconnected:
        m_connectButton->setText("连接");
        updateConnectionStatus("未连接", StatusTone::Error);
        updateHeartbeatStatus("心跳异常", StatusTone::Error);
        break;
    case ConnectionState::Connecting:
        m_connectButton->setText("取消连接");
        updateConnectionStatus("未连接", StatusTone::Error);
        updateHeartbeatStatus("心跳异常", StatusTone::Error);
        break;
    case ConnectionState::Connected:
        m_connectButton->setText("断开");
        updateConnectionStatus("已连接", StatusTone::Success);
        updateHeartbeatStatus("等待心跳", StatusTone::Pending);
        break;
    case ConnectionState::Reconnecting:
        m_connectButton->setText("停止重连");
        updateConnectionStatus("未连接", StatusTone::Error);
        updateHeartbeatStatus("心跳异常", StatusTone::Error);
        break;
    }
}

void T_SmartController::setCommandControlsEnabled(bool enabled)
{
    m_inputModeButton->setEnabled(enabled);
    m_outputModeButton->setEnabled(enabled);
    m_uploadUrlEdit->setEnabled(enabled);
    m_uploadIntervalSpinBox->setEnabled(enabled);
    m_sendA2Button->setEnabled(enabled);

    refreshControlButtons();
}

void T_SmartController::setToggleReadOnly(ElaToggleButton *button, bool readOnly)
{
    button->setAttribute(Qt::WA_TransparentForMouseEvents, readOnly);
    button->setFocusPolicy(readOnly ? Qt::NoFocus : Qt::StrongFocus);
}

void T_SmartController::resetIoControls()
{
    m_hasInputStatus = false;
    m_inputStatus = 0;
    m_outputStatus = 0;
    m_outputOffset = 0;
    m_outputLevel = -1;

    // 电平位按钮
    for (int i = 0; i < m_levelButtons.size(); ++i) {
        auto *button = m_levelButtons.at(i);
        const QSignalBlocker blocker(button);
        button->setIsToggled(false);
    }

    // 控制位按钮
    for (int i = 0; i < m_controlButtons.size(); ++i) {
        auto *button = m_controlButtons.at(i);
        const QSignalBlocker blocker(button);
        button->setIsToggled(false);
    }

    // 偏移位按钮
    const QSignalBlocker offsetBlocker(m_offsetButton);
    m_offsetButton->setIsToggled(false);
}

void T_SmartController::updateConnectionStatus(const QString &text, StatusTone tone)
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

void T_SmartController::updateHeartbeatStatus(const QString &text, StatusTone tone)
{
    m_heartBeatStatusText->setText(text);
    switch (tone) {
    case StatusTone::Success:
        m_heartBeatStatusText->setStyleSheet("color: #28bf74");
        break;
    case StatusTone::Pending:
        m_heartBeatStatusText->setStyleSheet("color: #d98c00");
        break;
    case StatusTone::Error:
        m_heartBeatStatusText->setStyleSheet("color: #ff0000");
        break;
    }
}

bool T_SmartController::parseEndpoint(QString &ip, quint16 &port)
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

void T_SmartController::showInputError(const QString &message)
{
    ElaMessageBar::warning(ElaMessageBarType::BottomRight, "参数有误", message, 1500, this);
}

QMap<int, int> T_SmartController::relayMap() const
{
    // 16路，但是生产环境只会使用前8路
    QMap<int, int> values;
    for (int i = 0; i < 16; ++i)
        values.insert(i + 1, (m_outputStatus & static_cast<quint16>(1u << i)) != 0 ? 1 : 0);
    return values;
}

QMap<int, int> T_SmartController::levelMap() const
{
    QMap<int, int> values;
    for (int i = 0; i < 16; ++i)
        values.insert(i + 1, m_outputLevel);
    return values;
}
