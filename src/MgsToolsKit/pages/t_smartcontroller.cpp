#include "t_smartcontroller.h"

#include <QButtonGroup>
#include <QDateTime>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHostAddress>
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

QString hex16(quint16 value)
{
    return QString("0x%1").arg(value, 4, 16, QLatin1Char('0')).toUpper();
}
} // namespace

T_SmartController::T_SmartController(QWidget *parent)
    : T_BasePage{parent}
{
    setWindowTitle("智能网关测试工具");
    createCustomWidget("测试IO车道控制器的A1/A2指令发送，以及D2/D6状态接收");
    initContent();

    connect(m_connectButton, &ElaPushButton::clicked, this, &T_SmartController::onConnectServer);
    connect(m_sendA1Button, &ElaPushButton::clicked, this, &T_SmartController::onSendA1);
    connect(m_sendA2Button, &ElaPushButton::clicked, this, &T_SmartController::onSendA2);
    connect(m_allOffButton, &ElaPushButton::clicked, this, [this]() {
        for (auto *button : m_relayButtons)
            button->setIsToggled(false);
    });
    connect(m_logClearButton, &ElaPushButton::clicked, m_logEdit, &ElaPlainTextEdit::clear);

    connect(cuteLogger, &Logger::sigLogWrite, this, [this](Logger::LogLevel, const QString &log, const QString &category) {
        if (category == "SmartLaneController")
            m_logEdit->appendPlainText(DataDealUtils::curDateTimeStr() + " | " + log + "\n");
    });

    m_smartController = createSmartLaneController();
    if (!m_smartController) {
        updateConnectionStatus("组件创建失败", StatusTone::Error);
        m_connectButton->setEnabled(false);
        setCommandControlsEnabled(false);
        return;
    }

    m_smartController->setVersion(0x00);
    connect(m_smartController, &ISmartLaneController::sigRecvD2Cmd, this, &T_SmartController::onRecvD2);
    connect(m_smartController, &ISmartLaneController::sigRecvD6Cmd, this, &T_SmartController::onRecvD6);
    connect(m_smartController, &ISmartLaneController::sigConnectionStateChanged, this, [this](bool connected) {
        m_connecting = false;
        m_connected = connected;
        m_reconnecting = !connected && !m_userDisconnectRequested;
        setCommandControlsEnabled(connected);

        if (connected) {
            m_userDisconnectRequested = false;
            setConnectionFieldsEnabled(false);
            m_connectButton->setText("断开");
            updateConnectionStatus("已连接", StatusTone::Success);
        } else if (m_userDisconnectRequested) {
            resetConnectionUi();
        } else {
            setConnectionFieldsEnabled(false);
            m_connectButton->setText("停止重连");
            updateConnectionStatus("重连中", StatusTone::Pending);
        }
    });
    connect(m_smartController, &ISmartLaneController::sigReconnectFailed, this, [this]() {
        m_userDisconnectRequested = false;
        resetConnectionUi();
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "连接失败", "自动重连次数已用尽", 1800, this);
    });

    setCommandControlsEnabled(false);
}

T_SmartController::~T_SmartController()
{
    if (m_smartController)
        destroySmartLaneController(m_smartController);
}

void T_SmartController::initContent()
{
    auto *connectionBox = new QGroupBox("设备连接", this);
    auto *connectionLayout = new QHBoxLayout(connectionBox);
    connectionLayout->setContentsMargins(12, 12, 12, 10);
    connectionLayout->setSpacing(8);

    m_connectInfoEdit = new ElaLineEdit(this);
    m_connectInfoEdit->setPlaceholderText("IP地址:端口，例如 192.168.0.1:9588");
    m_connectInfoEdit->setText("192.168.0.1:9588");
    m_connectButton = new ElaPushButton("连接", this);
    m_connectionStatusText = createLabel("未连接", this);
    m_connectionStatusText->setStyleSheet("color: #ff0000");

    connectionLayout->addWidget(createLabel("服务地址", this));
    connectionLayout->addWidget(m_connectInfoEdit, 1);
    connectionLayout->addWidget(m_connectButton);
    connectionLayout->addSpacing(16);
    connectionLayout->addWidget(createLabel("连接状态", this));
    connectionLayout->addWidget(m_connectionStatusText);

    auto *a1Box = new QGroupBox("A1 - 继电器控制", this);
    auto *a1Layout = new QVBoxLayout(a1Box);
    a1Layout->setContentsMargins(12, 12, 12, 10);
    a1Layout->setSpacing(8);

    auto *relayGrid = new QGridLayout();
    relayGrid->setContentsMargins(0, 0, 0, 0);
    relayGrid->setHorizontalSpacing(8);
    relayGrid->setVerticalSpacing(6);
    for (int i = 0; i < 16; ++i) {
        auto *button = new ElaToggleButton(QString("%1").arg(i + 1, 2, 10, QLatin1Char('0')), this);
        button->setFixedWidth(58);
        m_relayButtons.append(button);
        relayGrid->addWidget(button, i / 8, i % 8);
    }

    m_lowLevelButton = new ElaRadioButton("低电平触发", this);
    m_highLevelButton = new ElaRadioButton("高电平触发", this);
    m_highLevelButton->setChecked(true);
    auto *levelGroup = new QButtonGroup(this);
    levelGroup->addButton(m_lowLevelButton);
    levelGroup->addButton(m_highLevelButton);
    m_allOffButton = new ElaPushButton("全部关闭", this);
    m_sendA1Button = new ElaPushButton("发送 A1", this);

    auto *a1OperationLayout = new QHBoxLayout();
    a1OperationLayout->setContentsMargins(0, 0, 0, 0);
    a1OperationLayout->setSpacing(8);
    a1OperationLayout->addWidget(createLabel("触发电平", this));
    a1OperationLayout->addWidget(m_lowLevelButton);
    a1OperationLayout->addWidget(m_highLevelButton);
    a1OperationLayout->addStretch();
    a1OperationLayout->addWidget(m_allOffButton);
    a1OperationLayout->addWidget(m_sendA1Button);

    a1Layout->addWidget(createLabel("选择需要开启的IO通道（01-16）", this));
    a1Layout->addLayout(relayGrid);
    a1Layout->addLayout(a1OperationLayout);

    auto *a2Box = new QGroupBox("A2 - 状态上传配置", this);
    auto *a2Layout = new QHBoxLayout(a2Box);
    a2Layout->setContentsMargins(12, 12, 12, 10);
    a2Layout->setSpacing(8);
    m_uploadUrlEdit = new ElaLineEdit(this);
    m_uploadUrlEdit->setPlaceholderText("例如 http://192.168.0.10:8080/device/status");
    m_uploadIntervalSpinBox = new ElaSpinBox(this);
    m_uploadIntervalSpinBox->setRange(1, 255);
    m_uploadIntervalSpinBox->setValue(1);
    m_uploadIntervalSpinBox->setSuffix(" 分钟");
    m_sendA2Button = new ElaPushButton("发送 A2", this);

    a2Layout->addWidget(createLabel("上传URL", this));
    a2Layout->addWidget(m_uploadUrlEdit, 1);
    a2Layout->addWidget(createLabel("上传间隔", this));
    a2Layout->addWidget(m_uploadIntervalSpinBox);
    a2Layout->addWidget(m_sendA2Button);

    auto *receiveBox = new QGroupBox("设备上报", this);
    auto *receiveLayout = new QVBoxLayout(receiveBox);
    receiveLayout->setContentsMargins(12, 12, 12, 10);
    receiveLayout->setSpacing(8);

    m_d2StatusText = createLabel("-", this);
    auto *d2TitleLayout = new QHBoxLayout();
    d2TitleLayout->setContentsMargins(0, 0, 0, 0);
    d2TitleLayout->addWidget(createLabel("D2 IO状态", this));
    d2TitleLayout->addWidget(m_d2StatusText);
    d2TitleLayout->addStretch();

    auto *d2Grid = new QGridLayout();
    d2Grid->setContentsMargins(0, 0, 0, 0);
    d2Grid->setHorizontalSpacing(8);
    d2Grid->setVerticalSpacing(6);
    for (int i = 0; i < 16; ++i) {
        auto *button = new ElaToggleButton(QString("%1").arg(i + 1, 2, 10, QLatin1Char('0')), this);
        button->setFixedWidth(58);
        button->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        button->setFocusPolicy(Qt::NoFocus);
        m_d2StatusButtons.append(button);
        d2Grid->addWidget(button, i / 8, i % 8);
    }

    m_deviceStatusText = createLabel("未收到", this);
    m_d6IoStatusText = createLabel("-", this);
    m_heartbeatTimeText = createLabel("-", this);
    auto *d6Layout = new QHBoxLayout();
    d6Layout->setContentsMargins(0, 0, 0, 0);
    d6Layout->setSpacing(8);
    d6Layout->addWidget(createLabel("D6 设备状态", this));
    d6Layout->addWidget(m_deviceStatusText);
    d6Layout->addSpacing(20);
    d6Layout->addWidget(createLabel("IO状态", this));
    d6Layout->addWidget(m_d6IoStatusText);
    d6Layout->addSpacing(20);
    d6Layout->addWidget(createLabel("最近心跳", this));
    d6Layout->addWidget(m_heartbeatTimeText);
    d6Layout->addStretch();

    receiveLayout->addLayout(d2TitleLayout);
    receiveLayout->addLayout(d2Grid);
    receiveLayout->addLayout(d6Layout);

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
    m_logEdit->setPlaceholderText("A1/A2发送及D2/D6接收日志将在这里显示");
    m_logEdit->setMinimumHeight(160);

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("智能网关测试工具");
    auto *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setContentsMargins(0, 5, 5, 0);
    centralLayout->setSpacing(5);
    centralLayout->addWidget(connectionBox);
    centralLayout->addWidget(a1Box);
    centralLayout->addWidget(a2Box);
    centralLayout->addWidget(receiveBox);
    centralLayout->addLayout(logTitleLayout);
    centralLayout->addWidget(m_logEdit, 1);
    addCentralWidget(centralWidget, true, true, 0);
}

void T_SmartController::onConnectServer()
{
    if (!m_smartController)
        return;

    if (m_connecting || m_connected || m_reconnecting) {
        m_userDisconnectRequested = true;
        m_smartController->disconnectServer();
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
    setConnectionFieldsEnabled(false);
    setCommandControlsEnabled(false);
    m_connectButton->setText("取消连接");
    updateConnectionStatus("连接中", StatusTone::Pending);
    m_smartController->connectServer(ip, port);
}

void T_SmartController::onSendA1()
{
    if (!m_connected) {
        showInputError("请先连接IO车道控制器");
        return;
    }

    m_smartController->sendA1Cmd(relayMap(), levelMap());
    ElaMessageBar::success(ElaMessageBarType::BottomRight, "A1已入队", "继电器控制指令已加入同步发送队列", 1200, this);
}

void T_SmartController::onSendA2()
{
    if (!m_connected) {
        showInputError("请先连接IO车道控制器");
        return;
    }

    const QString urlText = m_uploadUrlEdit->text().trimmed();
    const QUrl url = QUrl::fromUserInput(urlText);
    if (urlText.isEmpty() || !url.isValid() || url.scheme().isEmpty() || url.host().isEmpty()) {
        showInputError("请输入有效的上传URL");
        return;
    }

    m_smartController->sendA2Cmd(urlText.toUtf8(), static_cast<uchar>(m_uploadIntervalSpinBox->value()));
    ElaMessageBar::success(ElaMessageBarType::BottomRight, "A2已入队", "状态上传配置已加入同步发送队列", 1200, this);
}

void T_SmartController::onRecvD2(QByteArray cmd)
{
    if (cmd.size() < 3) {
        m_logEdit->appendPlainText(DataDealUtils::curDateTimeStr() + " | D2数据长度不足\n");
        return;
    }

    const quint16 high = static_cast<uchar>(cmd.at(1));
    const quint16 low = static_cast<uchar>(cmd.at(2));
    updateD2Status(static_cast<quint16>((high << 8) | low));
}

void T_SmartController::onRecvD6(QByteArray cmd)
{
    if (cmd.size() < 4) {
        m_logEdit->appendPlainText(DataDealUtils::curDateTimeStr() + " | D6数据长度不足\n");
        return;
    }

    const uchar deviceStatus = static_cast<uchar>(cmd.at(1));
    const quint16 high = static_cast<uchar>(cmd.at(2));
    const quint16 low = static_cast<uchar>(cmd.at(3));
    const quint16 ioStatus = static_cast<quint16>((high << 8) | low);

    if (deviceStatus == 0x00) {
        m_deviceStatusText->setText("正常");
        m_deviceStatusText->setStyleSheet("color: #28bf74");
    } else {
        m_deviceStatusText->setText(QString("异常(0x%1)").arg(deviceStatus, 2, 16, QLatin1Char('0')).toUpper());
        m_deviceStatusText->setStyleSheet("color: #ff0000");
    }
    m_d6IoStatusText->setText(hex16(ioStatus));
    m_heartbeatTimeText->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
}

void T_SmartController::setConnectionFieldsEnabled(bool enabled)
{
    m_connectInfoEdit->setEnabled(enabled);
}

void T_SmartController::setCommandControlsEnabled(bool enabled)
{
    for (auto *button : m_relayButtons)
        button->setEnabled(enabled);
    m_lowLevelButton->setEnabled(enabled);
    m_highLevelButton->setEnabled(enabled);
    m_allOffButton->setEnabled(enabled);
    m_sendA1Button->setEnabled(enabled);
    m_uploadUrlEdit->setEnabled(enabled);
    m_uploadIntervalSpinBox->setEnabled(enabled);
    m_sendA2Button->setEnabled(enabled);
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

void T_SmartController::resetConnectionUi()
{
    m_connecting = false;
    m_connected = false;
    m_reconnecting = false;
    setConnectionFieldsEnabled(true);
    setCommandControlsEnabled(false);
    m_connectButton->setText("连接");
    updateConnectionStatus("未连接", StatusTone::Error);
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

void T_SmartController::updateD2Status(quint16 status)
{
    m_d2StatusText->setText(hex16(status));
    for (int i = 0; i < m_d2StatusButtons.size(); ++i)
        m_d2StatusButtons.at(i)->setIsToggled((status & (quint16(1) << i)) != 0);
}

QMap<int, int> T_SmartController::relayMap() const
{
    QMap<int, int> values;
    for (int i = 0; i < m_relayButtons.size(); ++i)
        values.insert(i + 1, m_relayButtons.at(i)->getIsToggled() ? 1 : 0);
    return values;
}

QMap<int, int> T_SmartController::levelMap() const
{
    QMap<int, int> values;
    const int level = m_highLevelButton->isChecked() ? 1 : 0;
    for (int i = 1; i <= 16; ++i)
        values.insert(i, level);
    return values;
}
