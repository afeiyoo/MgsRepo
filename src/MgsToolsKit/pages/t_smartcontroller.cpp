#include "t_smartcontroller.h"

#include <QButtonGroup>
#include <QHBoxLayout>

#include "ElaLineEdit.h"
#include "ElaMessageBar.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"
#include "ElaRadioButton.h"
#include "ElaText.h"
#include "ElaToggleButton.h"
#include "ElaToggleSwitch.h"
#include "Logger.h"
#include "qhostaddress.h"
#include "smartlanecontroller.h"
#include "utils/datadealutils.h"

using namespace Utils;

T_SmartController::T_SmartController(QWidget *parent)
    : T_BasePage{parent}
{
    m_smartController = new SmartLaneController(this);
    m_smartController->enableRetryConnect(true);

    // 预览窗口标题
    setWindowTitle("智能网关测试工具");
    // 顶部元素
    createCustomWidget("用于测试智能网关的车控器操作");

    initContent();

    m_heartTimer = new QTimer(this);
    m_heartTimer->setInterval(5000); // 5秒

    m_heartTimer->setSingleShot(true);
    connect(m_heartTimer, &QTimer::timeout, this, [=]() {
        // 超时：认为心跳异常
        LOG_CWARNING("smartctrl").noquote() << "心跳超时（>5s）";

        m_heartStatusText->setText("设备心跳异常");
        m_heartStatusText->setStyleSheet("color: #ff0000");
    });

    connect(m_connectButton, &ElaPushButton::clicked, this, &T_SmartController::onConnectServer);
    connect(m_sendButton, &ElaPushButton::clicked, this, &T_SmartController::onSendToSmartLaneController);

    connect(m_inputButton, &ElaPushButton::toggled, this, [=](bool checked) {
        Q_UNUSED(checked);
        // 偏移位按钮重置
        m_offsetButton->setIsToggled(false);
        // 控制位按钮重置
        for (auto btn : m_controlButton) {
            btn->setIsToggled(false);
        }
        // 电平位按钮重置
        for (auto btn : m_triggerButton) {
            btn->setIsToggled(false);
        }
    });

    connect(m_smartController, &SmartLaneController::sigNetworkStatusChanged, this, [=](bool isOnline) {
        m_isTcpConnected = isOnline;
        if (!isOnline) {
            m_connectButton->setText("连接");
            m_heartStatusText->setText("设备心跳异常");
            m_heartStatusText->setStyleSheet("color: #ff0000");

            m_heartTimer->stop(); // 断线停止
        } else {
            m_connectButton->setText("断连");
            m_heartTimer->start();
        }
    });

    connect(m_logClearButton, &ElaPushButton::clicked, m_logEdit, &ElaPlainTextEdit::clear);

    connect(m_smartController, &SmartLaneController::sigRecvFromSmartLaneController, this, &T_SmartController::onRecvFromSmartLaneController);

    // 界面显示日志信息
    connect(cuteLogger, &Logger::sigLogWrite, this, [=](Logger::LogLevel level, const QString &log, const QString &cat) {
        if (cat == "smartctrl") {
            m_logEdit->appendPlainText(DataDealUtils::curDateTimeStr() + " | " + log + "\n");
        }
    });
}

T_SmartController::~T_SmartController() {}

void T_SmartController::initContent()
{
    // 局部布局1
    m_connectInfoEdit = new ElaLineEdit(this);
    m_connectInfoEdit->setPlaceholderText("请输入IP地址与端口,用冒号分隔");
    m_connectInfoEdit->setFixedSize(240, 35);
    m_connectButton = new ElaPushButton("连接", this);
    m_sendButton = new ElaPushButton("发送", this);

    ElaText *devCtrlTip = new ElaText("设备开关", this);
    devCtrlTip->setTextPixelSize(15);
    devCtrlTip->setIsWrapAnywhere(false);

    m_devSwitch = new ElaToggleSwitch(this);
    m_devSwitch->setFixedHeight(20);

    ElaText *modeTip = new ElaText("模式", this);
    modeTip->setTextPixelSize(15);
    modeTip->setIsWrapAnywhere(false);

    m_outputButton = new ElaRadioButton("输出", this);
    m_inputButton = new ElaRadioButton("输入", this);
    m_outputButton->setChecked(true);

    QButtonGroup *modeGroup = new QButtonGroup(this);
    modeGroup->addButton(m_outputButton);
    modeGroup->addButton(m_inputButton);

    QHBoxLayout *partHLayout1 = new QHBoxLayout();
    partHLayout1->setContentsMargins(0, 0, 0, 0);
    partHLayout1->addWidget(m_connectInfoEdit);
    partHLayout1->addWidget(m_connectButton);
    partHLayout1->addWidget(m_sendButton);
    partHLayout1->addStretch();
    partHLayout1->addWidget(devCtrlTip);
    partHLayout1->addWidget(m_devSwitch);
    partHLayout1->addSpacing(13);
    partHLayout1->addWidget(modeTip);
    partHLayout1->addWidget(m_outputButton);
    partHLayout1->addWidget(m_inputButton);

    // 局部布局2
    ElaText *offsetTip = new ElaText("偏移位(第1位)", this);
    offsetTip->setTextPixelSize(15);
    offsetTip->setIsWrapAnywhere(false);

    m_offsetButton = new ElaToggleButton("0", this);
    m_offsetButton->setFixedWidth(65);

    QHBoxLayout *partHLayout2 = new QHBoxLayout();
    partHLayout2->setContentsMargins(0, 0, 0, 0);
    partHLayout2->addWidget(offsetTip);
    partHLayout2->addWidget(m_offsetButton);
    partHLayout2->addStretch();

    // 局部布局3
    ElaText *controlTip = new ElaText("控制位(第2位)", this);
    controlTip->setTextPixelSize(15);
    controlTip->setIsWrapAnywhere(false);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(0, 0, 0, 0);
    for (int i = 0; i < 16; ++i) {
        ElaToggleButton *controlButton = new ElaToggleButton(QString::number(i), this);
        controlButton->setFixedWidth(65);
        m_controlButton.append(controlButton);

        int row = i / 8;
        int col = i % 8;
        gridLayout->addWidget(controlButton, row, col);
    }

    QHBoxLayout *partHLayout3 = new QHBoxLayout();
    partHLayout3->setContentsMargins(0, 0, 0, 0);
    partHLayout3->addWidget(controlTip);
    partHLayout3->addLayout(gridLayout); // 这里放 grid
    partHLayout3->addStretch();

    // 局部布局4
    ElaText *triggerTip = new ElaText("电平位(第3位)", this);
    triggerTip->setTextPixelSize(15);
    triggerTip->setIsWrapAnywhere(false);

    for (int i = 0; i < 2; ++i) {
        ElaToggleButton *triggerButton = new ElaToggleButton(QString::number(i), this);
        triggerButton->setFixedWidth(65);
        m_triggerButton.append(triggerButton);

        connect(triggerButton, &ElaToggleButton::toggled, this, [=](bool checked) {
            if (checked) {
                // 选中当前 → 取消其他，如果是取消选中，不做处理（允许全不选）
                for (auto btn : m_triggerButton) {
                    if (btn != triggerButton) {
                        btn->setIsToggled(false);
                    }
                }
            }
        });
    }

    QHBoxLayout *partHLayout4 = new QHBoxLayout();
    partHLayout4->setContentsMargins(0, 0, 0, 0);
    partHLayout4->addWidget(triggerTip);
    partHLayout4->addWidget(m_triggerButton[0]);
    partHLayout4->addWidget(m_triggerButton[1]);
    partHLayout4->addStretch();

    // 局部布局5
    ElaText *logTipText = new ElaText("交互日志显示区", this);
    QFont logTipFont = logTipText->font();
    logTipFont.setBold(true);
    logTipText->setFont(logTipFont);

    m_heartStatusText = new ElaText("设备心跳异常", this);
    m_heartStatusText->setStyleSheet("color: #ff0000");

    QList<ElaText *> texts = {logTipText, m_heartStatusText};
    for (auto text : texts) {
        text->setTextPixelSize(15);
        text->setIsWrapAnywhere(false);
    }

    m_logClearButton = new ElaPushButton("清除", this);

    QHBoxLayout *partHLayout5 = new QHBoxLayout();
    partHLayout5->setContentsMargins(0, 0, 0, 0);
    partHLayout5->addWidget(logTipText);
    partHLayout5->addWidget(m_logClearButton);
    partHLayout5->addStretch();
    partHLayout5->addWidget(m_heartStatusText);

    // 局部控件6
    m_logEdit = new ElaPlainTextEdit(this);
    m_logEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    m_logEdit->setReadOnly(true);
    m_logEdit->setPlaceholderText("交互日志");

    // 整体布局
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("智能网关测试工具");
    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setContentsMargins(0, 0, 5, 0);
    centralLayout->addSpacing(5);
    centralLayout->addLayout(partHLayout1);
    centralLayout->addLayout(partHLayout2);
    centralLayout->addLayout(partHLayout3);
    centralLayout->addLayout(partHLayout4);
    centralLayout->addLayout(partHLayout5);
    centralLayout->addWidget(m_logEdit);

    addCentralWidget(centralWidget, true, true, 0);
}

void T_SmartController::onConnectServer()
{
    if (m_isTcpConnected) {
        m_smartController->disconnectServer();
        return;
    }

    QString info = m_connectInfoEdit->text().simplified();
    if (info.isEmpty())
        return;

    QStringList parts = info.split(":");
    if (parts.size() != 2) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "连接失败", "连接信息填写错误，请检查", 1000, this);
        LOG_CERROR("smartctrl").noquote() << "Tcp连接智能网关失败: 连接信息填写错误";
        return;
    }

    QString ip = parts.at(0).trimmed();
    QHostAddress addr;
    if (!addr.setAddress(ip)) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "连接失败", "IP地址格式错误，请检查", 1000, this);
        LOG_CERROR("smartctrl").noquote() << "Tcp连接智能网关失败: IP地址格式错误";
        return;
    }

    QString portStr = parts.at(1).trimmed();
    bool ok = false;
    int port = portStr.toInt(&ok);
    if (!ok || port <= 0 || port > 65535) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "连接失败", "端口号非法(1-65535)，请检查", 1000, this);
        LOG_CERROR("smartctrl").noquote() << "Tcp连接智能网关失败: 端口号非法(1-65535)";
        return;
    }

    m_smartController->connectServer(ip, static_cast<quint16>(port));
}

void T_SmartController::onRecvFromSmartLaneController(uchar type, QByteArray data)
{
    switch (type) {
    case 0xD2: {
        if (!m_inputButton->isChecked())
            return;

        uchar high = static_cast<uchar>(data.at(1));
        uchar low = static_cast<uchar>(data.at(2));
        quint16 value = (high << 8) | low;
        // 控制位按钮更新
        for (int i = 0; i < m_controlButton.size(); ++i) {
            bool checked = value & (1 << i);
            m_controlButton[i]->setIsToggled(checked);
        }
        // 偏移位按钮更新
        m_offsetButton->setIsToggled(true);
        // 电平位按钮更新  NOTE: 电平位无法得知，默认高电平触发（需要依据现场环境调整）
        m_triggerButton[1]->setIsToggled(true);
    } break;
    case 0xD3:
        // TODO 称重数据
        break;
    case 0xD6: {
        uchar heartStatus = static_cast<uchar>(data.at(1));
        // 每次收到心跳 → 重置计时器
        m_heartTimer->start();

        if (heartStatus != 0x00) {
            m_heartStatusText->setText("设备心跳异常");
            m_heartStatusText->setStyleSheet("color: #ff0000");
        } else {
            m_heartStatusText->setText("设备心跳正常");
            m_heartStatusText->setStyleSheet("color: #28bf74");
        }
    } break;
    default:
        break;
    }
}

void T_SmartController::onSendToSmartLaneController()
{
    if (!m_isTcpConnected) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "发送失败", "与智能网关未建立连接，请注意", 1000, this);
        LOG_CERROR("infoboard").noquote() << "数据发送失败：未与智能网关建立连接";
        return;
    }

    if (!m_outputButton->isChecked()) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "发送失败", "未设置输出模式", 1000, this);
        LOG_CERROR("infoboard").noquote() << "数据发送失败：未设置输出模式";
        return;
    }

    bool isControlButtonSet = false;
    bool isOffsetButtonSet = false;
    bool isTriggerButtonSet = false;
    for (auto btn : m_controlButton) {
        if (btn->getIsToggled()) {
            isControlButtonSet = true;
            break;
        }
    }
    for (auto btn : m_triggerButton) {
        if (btn->getIsToggled()) {
            isTriggerButtonSet = true;
            break;
        }
    }
    isOffsetButtonSet = m_offsetButton->getIsToggled();

    if (!isControlButtonSet || !isOffsetButtonSet || !isTriggerButtonSet) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "发送失败", "输出内容设置错误", 1000, this);
        LOG_CERROR("infoboard").noquote() << "数据发送失败：输出内容设置错误";
        return;
    }

    // 控制第i+1路
    bool devSwitch = m_devSwitch->getIsToggled();
    QMap<int, bool> relayMap;
    for (int i = 0; i < m_controlButton.size(); ++i) {
        relayMap.insert(i + 1, 0); // 所有路默认关闭
        if (i == 0)                // 输出的话，路数从1开始排序（相当于按钮0是废了）
            continue;
        if (m_controlButton[i]->getIsToggled() && devSwitch) {
            relayMap[i] = 1;
        }
    }
    // 0: 低电平触发 1: 高电平触发
    int triggerLevel = 0;
    if (m_triggerButton[0]->getIsToggled()) {
        triggerLevel = 0;
    } else {
        triggerLevel = 1;
    }

    QByteArray sendData = packSendData(relayMap, triggerLevel);

    m_smartController->sendCommand("A1", sendData);
}

QByteArray T_SmartController::packSendData(const QMap<int, bool> &relayMap, int triggerLevel)
{
    QByteArray data;
    data.append(0xA1); // 固定命令 0xa1
    data.append(0x10); // 固定16路，目前只用到前8路，后8路备用
    for (int i = 1; i <= 16; ++i) {
        data.append((char) i); // 路号

        int level = 0;
        if (triggerLevel == 1) { // 高电平触发
            level = (relayMap[i] == 1 ? 1 : 0);
        } else { // 低电平触发
            level = (relayMap[i] == 1 ? 0 : 1);
        }
        data.append((char) level);
    }
    return data;
}
