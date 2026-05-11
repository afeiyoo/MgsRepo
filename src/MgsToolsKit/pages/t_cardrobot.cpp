#include "t_cardrobot.h"

#include <QFileInfo>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>

#include "ElaCheckBox.h"
#include "ElaComboBox.h"
#include "ElaFlowLayout.h"
#include "ElaImageCard.h"
#include "ElaLineEdit.h"
#include "ElaMessageBar.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"
#include "ElaScrollPageArea.h"
#include "ElaText.h"
#include "Logger.h"
#include "NlohmannJson/nlojson.hpp"
#include "QZXing/QZXing.h"
#include "bend/cardrobot/cardrobothandler.h"
#include "dialogs/t_cardrobotstatus.h"
#include "global/constant.h"
#include "global/globalmanager.h"
#include "global/signalmanager.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"

using namespace Utils;

T_CardRobot::T_CardRobot(QWidget *parent)
    : T_BasePage{parent}
{
    // 预览窗口标题
    setWindowTitle("自动发卡机测试工具");
    // 顶部元素
    createCustomWidget("模拟自动发卡机服务端进行命令收发");
    initContent();

    m_player = new QMediaPlayer(this);
    m_playlist = new QMediaPlaylist(this);
    m_playlist->setPlaybackMode(QMediaPlaylist::Sequential);
    m_player->setPlaylist(m_playlist);

    m_robotHandler = new CardRobotHandler(this);
    initRobotHandler();
    // 信息初始化
    connect(m_workStationComboBox, QOverload<int>::of(&ElaComboBox::currentIndexChanged), this, [this](int) { initRobotHandler(); });
    connect(m_upBoxNoComboBox, QOverload<int>::of(&ElaComboBox::currentIndexChanged), this, [this](int) { initRobotHandler(); });
    connect(m_downBoxNoComboBox, QOverload<int>::of(&ElaComboBox::currentIndexChanged), this, [this](int) { initRobotHandler(); });

    m_robotStatusDlg = new T_CardRobotStatus(); // NOTE 卡机状态信息初始化信号发送，所以要放在卡机类的初始化后面初始化
    m_robotStatusDlg->hide();
    connect(m_settingHeartButton, &ElaPushButton::clicked, this, [=](bool checked) {
        Q_UNUSED(checked);
        m_robotStatusDlg->moveToCenter();
        m_robotStatusDlg->show();
    });

    connect(m_logClearButton, &ElaPushButton::clicked, m_logEdit, &ElaPlainTextEdit::clear);
    connect(m_listenButton, &ElaPushButton::clicked, this, &T_CardRobot::onListen);
    connect(m_outCardButton, &ElaPushButton::clicked, this, &T_CardRobot::onOutCardMsgSend);
    connect(m_requireCardButton, &ElaPushButton::clicked, this, &T_CardRobot::onRequireCardMsgSend);
    connect(m_retriveCardButton, &ElaPushButton::clicked, this, &T_CardRobot::onRetriveCardMsgSend);
    connect(m_recycleCardButton, &ElaPushButton::clicked, this, &T_CardRobot::onRecycleCardMsgSend);
    connect(m_ignoreMessageButton, &ElaPushButton::clicked, this, &T_CardRobot::onIgnoreMsgSend);
    connect(m_paperButton, &ElaPushButton::clicked, this, &T_CardRobot::onPaperMsgSend);
    connect(m_powerOnButton, &ElaPushButton::clicked, this, &T_CardRobot::onPowerOnMsgSend);

    connect(GM_INSTANCE->m_signalMan, &SignalManager::sigRobotSendCommandFinish, this, &T_CardRobot::onRobotSendCommandFinish);
    connect(GM_INSTANCE->m_signalMan, &SignalManager::sigRobotReceiveCommand, this, &T_CardRobot::onRobotReceiveCommand);
    connect(GM_INSTANCE->m_signalMan, &SignalManager::sigRobotScreenInit, this, [this]() {
        displayMainScreen("福建高速;祝你平安");
        displaySecondScreen("福建高速;祝你平安");
        displayQRCode(QImage(Constant::APP::ICON_PATH));
    });

    connect(m_autoRespCheckBox, &ElaCheckBox::clicked, this, [=](bool checked) { emit GM_INSTANCE->m_signalMan->sigIsAutoRespCommand52(checked); });

    // 界面显示日志信息
    connect(cuteLogger, &Logger::sigLogWrite, this, [=](Logger::LogLevel level, const QString &log, const QString &cat) {
        if (cat == "cardrobot") {
            if (m_detailLogCheckBox->isChecked()) {
                m_logEdit->appendPlainText(DataDealUtils::curDateTimeStr() + " | " + log + "\n");
            } else {
                if (level < Logger::Info)
                    return;
                m_logEdit->appendPlainText(DataDealUtils::curDateTimeStr() + " | " + log + "\n");
            }
        }
    });

    m_heartTimer = new QTimer(this);
    m_heartTimer->setInterval(5000);
    connect(m_heartTimer, &QTimer::timeout, this, [this]() {
        if (!m_isListening || !m_robotHandler)
            return;

        NloJson nloJson;
        QByteArray json = nloJson.serialize({});
        emit GM_INSTANCE->m_signalMan->sigRobotSendCommand(0x42, json);
    });
}

T_CardRobot::~T_CardRobot()
{
    delete m_robotStatusDlg;
    m_robotStatusDlg = nullptr;
}

void T_CardRobot::initContent()
{
    // 局部布局1
    m_portEdit = new ElaLineEdit(this);
    m_portEdit->setPlaceholderText("请输入监听端口");
    m_portEdit->setFixedSize(130, 35);

    m_listenButton = new ElaPushButton("启动", this);
    m_powerOnButton = new ElaPushButton("上电", this);

    m_detailLogCheckBox = new ElaCheckBox("显示详细日志", this);
    m_autoRespCheckBox = new ElaCheckBox("按键取卡自动响应", this);
    m_allowPlayCheckBox = new ElaCheckBox("声音播放", this);

    QHBoxLayout *partHLayout1 = new QHBoxLayout();
    partHLayout1->setContentsMargins(0, 0, 0, 0);
    partHLayout1->addWidget(m_portEdit);
    partHLayout1->addWidget(m_listenButton);
    partHLayout1->addWidget(m_powerOnButton);
    partHLayout1->addStretch();
    partHLayout1->addWidget(m_detailLogCheckBox);
    partHLayout1->addWidget(m_allowPlayCheckBox);
    partHLayout1->addWidget(m_autoRespCheckBox);

    // 局部布局2
    ElaText *workStationText = new ElaText("默认工位", this);
    ElaText *upBoxNoText = new ElaText("上工位默认卡机", this);
    ElaText *downBoxNoText = new ElaText("下工位默认卡机", this);
    QList<ElaText *> texts1 = {workStationText, upBoxNoText, downBoxNoText};
    for (auto text : texts1) {
        text->setTextPixelSize(15);
        text->setIsWrapAnywhere(false);
    }

    m_workStationComboBox = new ElaComboBox(this);
    m_workStationComboBox->addItems({"上工位", "下工位"});

    m_upBoxNoComboBox = new ElaComboBox(this);
    m_upBoxNoComboBox->addItems({"#1", "#2"});

    m_downBoxNoComboBox = new ElaComboBox(this);
    m_downBoxNoComboBox->addItems({"#3", "#4"});

    m_settingHeartButton = new ElaPushButton("设置心跳状态", this);

    QHBoxLayout *partHLayout2 = new QHBoxLayout();
    partHLayout2->setContentsMargins(0, 0, 0, 0);
    partHLayout2->addWidget(workStationText);
    partHLayout2->addWidget(m_workStationComboBox);
    partHLayout2->addWidget(upBoxNoText);
    partHLayout2->addWidget(m_upBoxNoComboBox);
    partHLayout2->addWidget(downBoxNoText);
    partHLayout2->addWidget(m_downBoxNoComboBox);
    partHLayout2->addStretch();
    partHLayout2->addWidget(m_settingHeartButton);

    // 局部布局3
    ElaScrollPageArea *showArea = new ElaScrollPageArea(this);
    showArea->setFixedHeight(110);

    ElaText *qrCodeTipText = new ElaText("二维码显示", this);
    ElaText *mainScreenTipText = new ElaText("主屏显示", this);
    ElaText *secondScreenTipText = new ElaText("副屏显示", this);
    QList<ElaText *> texts2 = {qrCodeTipText, mainScreenTipText, secondScreenTipText};
    for (auto text : texts2) {
        text->setTextPixelSize(10);
        text->setIsWrapAnywhere(false);
        text->setAlignment(Qt::AlignHCenter);
    }

    // 二维码显示区域
    QWidget *qrCodeWidget = new QWidget(this);
    qrCodeWidget->setFixedWidth(100);

    QVBoxLayout *qrCodeVLayout = new QVBoxLayout(qrCodeWidget);
    qrCodeVLayout->setContentsMargins(0, 5, 0, 1);
    qrCodeVLayout->setSpacing(2);

    m_qrCodeImage = new ElaImageCard(this);
    m_qrCodeImage->setFixedSize(85, 85);
    m_qrCodeImage->setIsPreserveAspectCrop(false);
    m_qrCodeImage->setCardImage(QImage(Constant::APP::ICON_PATH));

    qrCodeVLayout->addWidget(qrCodeTipText);
    qrCodeVLayout->addWidget(m_qrCodeImage, 1);

    // 主屏显示区域
    QWidget *mainScreenWidget = new QWidget(this);
    mainScreenWidget->setFixedWidth(250);

    QVBoxLayout *mainScreenVLayout = new QVBoxLayout(mainScreenWidget);
    mainScreenVLayout->setContentsMargins(0, 5, 0, 1);
    mainScreenVLayout->setSpacing(2);

    m_mainScreenText = new ElaText("福建高速\n祝你平安", this);
    m_mainScreenText->setIsWrapAnywhere(false);
    m_mainScreenText->setTextStyle(ElaTextType::Subtitle);
    m_mainScreenText->setAlignment(Qt::AlignCenter);

    mainScreenVLayout->addWidget(mainScreenTipText);
    mainScreenVLayout->addWidget(m_mainScreenText, 1);

    // 副屏显示区域
    QWidget *secondScreenWidget = new QWidget(this);
    secondScreenWidget->setFixedWidth(250);

    QVBoxLayout *secondScreenVLayout = new QVBoxLayout(secondScreenWidget);
    secondScreenVLayout->setContentsMargins(0, 5, 0, 1);
    secondScreenVLayout->setSpacing(2);

    m_secondScreenText = new ElaText("福建高速\n祝你平安", this);
    m_secondScreenText->setIsWrapAnywhere(false);
    m_secondScreenText->setTextStyle(ElaTextType::Subtitle);
    m_secondScreenText->setAlignment(Qt::AlignCenter);

    secondScreenVLayout->addWidget(secondScreenTipText);
    secondScreenVLayout->addWidget(m_secondScreenText, 1);

    QHBoxLayout *partHLayout3 = new QHBoxLayout(showArea);
    partHLayout3->setContentsMargins(0, 0, 0, 0);
    partHLayout3->setSpacing(50);
    partHLayout3->addStretch();
    partHLayout3->addWidget(mainScreenWidget);
    partHLayout3->addWidget(qrCodeWidget);
    partHLayout3->addWidget(secondScreenWidget);
    partHLayout3->addStretch();

    // 局部布局4
    ElaText *workStationTipText1 = new ElaText("工位信息", this);
    ElaText *boxNoTipText1 = new ElaText("卡机编号", this);
    ElaText *workStationTipText2 = new ElaText("工位信息", this);
    ElaText *boxNoTipText2 = new ElaText("卡机编号", this);
    ElaText *workStationTipText3 = new ElaText("工位信息", this);
    ElaText *boxNoTipText3 = new ElaText("卡机编号", this);
    ElaText *workStationTipText4 = new ElaText("工位信息", this);
    ElaText *boxNoTipText4 = new ElaText("卡机编号", this);
    QList<ElaText *> texts3 = {workStationTipText1, boxNoTipText1, workStationTipText2, boxNoTipText2,
                               workStationTipText3, boxNoTipText3, workStationTipText4, boxNoTipText4};
    for (auto text : texts3) {
        text->setTextPixelSize(15);
        text->setIsWrapAnywhere(false);
    }

    // 按键取卡信息区域
    m_requireCardWorkStationComboBox = new ElaComboBox(this);
    m_requireCardWorkStationComboBox->addItems({"上工位", "下工位", "取卡失败", "求助对讲"});
    m_requireCardBoxNoComboBox = new ElaComboBox(this);
    m_requireCardBoxNoComboBox->addItems({"#1", "#2", "#3", "#4"});
    m_requireCardButton = new ElaPushButton("按键取卡", this);

    QGroupBox *box2 = new QGroupBox("按键取卡信息", this);
    QGridLayout *layout2 = new QGridLayout(box2);
    layout2->setContentsMargins(3, 3, 3, 3);
    layout2->setSpacing(0);
    layout2->addWidget(workStationTipText2, 0, 0);
    layout2->addWidget(m_requireCardWorkStationComboBox, 0, 1);
    layout2->addWidget(boxNoTipText2, 1, 0);
    layout2->addWidget(m_requireCardBoxNoComboBox, 1, 1);
    layout2->addWidget(m_requireCardButton, 2, 0, 1, 2);
    layout2->setAlignment(m_requireCardButton, Qt::AlignRight);

    // 出卡信息区域
    m_outCardWorkStationComboBox = new ElaComboBox(this);
    m_outCardWorkStationComboBox->addItems({"上工位", "下工位", "出卡失败", "响应超时"});
    m_outCardBoxNoComboBox = new ElaComboBox(this);
    m_outCardBoxNoComboBox->addItems({"#1", "#2", "#3", "#4"});
    m_outCardButton = new ElaPushButton("出卡", this);

    QGroupBox *box1 = new QGroupBox("出卡信息", this);
    QGridLayout *layout1 = new QGridLayout(box1);
    layout1->setContentsMargins(3, 3, 3, 3);
    layout1->setSpacing(0);
    layout1->addWidget(workStationTipText1, 0, 0);
    layout1->addWidget(m_outCardWorkStationComboBox, 0, 1);
    layout1->addWidget(boxNoTipText1, 1, 0);
    layout1->addWidget(m_outCardBoxNoComboBox, 1, 1);
    layout1->addWidget(m_outCardButton, 2, 0, 1, 2);
    layout1->setAlignment(m_outCardButton, Qt::AlignRight);

    // 卡被取走信息区域
    m_retriveCardWorkStationComboBox = new ElaComboBox(this);
    m_retriveCardWorkStationComboBox->addItems({"上工位", "下工位"});
    m_retriveCardBoxNoComboBox = new ElaComboBox(this);
    m_retriveCardBoxNoComboBox->addItems({"#1", "#2", "#3", "#4"});
    m_retriveCardButton = new ElaPushButton("卡被取走", this);

    QGroupBox *box3 = new QGroupBox("卡被取走信息", this);
    QGridLayout *layout3 = new QGridLayout(box3);
    layout3->setContentsMargins(3, 3, 3, 3);
    layout3->setSpacing(0);
    layout3->addWidget(workStationTipText3, 0, 0);
    layout3->addWidget(m_retriveCardWorkStationComboBox, 0, 1);
    layout3->addWidget(boxNoTipText3, 1, 0);
    layout3->addWidget(m_retriveCardBoxNoComboBox, 1, 1);
    layout3->addWidget(m_retriveCardButton, 2, 0, 1, 2);
    layout3->setAlignment(m_retriveCardButton, Qt::AlignRight);

    // 卡回收完成信息区域
    m_recycleCardWorkStationComboBox = new ElaComboBox(this);
    m_recycleCardWorkStationComboBox->addItems({"上工位", "下工位", "回收失败"});
    m_recycleCardBoxNoComboBox = new ElaComboBox(this);
    m_recycleCardBoxNoComboBox->addItems({"#1", "#2", "#3", "#4"});
    m_recycleCardButton = new ElaPushButton("卡回收完成", this);

    QGroupBox *box4 = new QGroupBox("卡回收完成信息", this);
    QGridLayout *layout4 = new QGridLayout(box4);
    layout4->setContentsMargins(3, 3, 3, 3);
    layout4->setSpacing(0);
    layout4->addWidget(workStationTipText4, 0, 0);
    layout4->addWidget(m_recycleCardWorkStationComboBox, 0, 1);
    layout4->addWidget(boxNoTipText4, 1, 0);
    layout4->addWidget(m_recycleCardBoxNoComboBox, 1, 1);
    layout4->addWidget(m_recycleCardButton, 2, 0, 1, 2);
    layout4->setAlignment(m_recycleCardButton, Qt::AlignRight);

    // 局部布局5
    ElaText *workStationTipText5 = new ElaText("工位信息", this);
    ElaText *causeTipText1 = new ElaText("原因", this);
    ElaText *workStationTipText6 = new ElaText("工位信息", this);
    ElaText *causeTipText2 = new ElaText("原因", this);
    ElaText *paperNumTipText = new ElaText("纸券号", this);
    ElaText *upPrinterTipText = new ElaText("上工位打印机状态", this);
    ElaText *downPrinterTipText = new ElaText("下工位打印机状态", this);
    QList<ElaText *> texts4 = {workStationTipText5, causeTipText1,    workStationTipText6, causeTipText2,
                               paperNumTipText,     upPrinterTipText, downPrinterTipText};
    for (auto text : texts4) {
        text->setTextPixelSize(15);
        text->setIsWrapAnywhere(false);
    }

    // 按键忽略信息区域
    m_ignoreMessageWorkStationComboBox = new ElaComboBox(this);
    m_ignoreMessageWorkStationComboBox->addItems({"上工位", "下工位"});
    m_ignoreMessageCauseComboBox = new ElaComboBox(this);
    m_ignoreMessageCauseComboBox->addItems({"正出卡/回收", "无卡可发", "卡机故障", "故障并无卡", "降级模式", "取卡未到3s", "连续坏卡"});
    m_ignoreMessageButton = new ElaPushButton("按键忽略", this);

    QGroupBox *box5 = new QGroupBox("按键忽略信息", this);
    QGridLayout *layout5 = new QGridLayout(box5);
    layout5->setContentsMargins(3, 3, 3, 3);
    layout5->setSpacing(0);
    layout5->addWidget(workStationTipText5, 0, 0);
    layout5->addWidget(m_ignoreMessageWorkStationComboBox, 0, 1);
    layout5->addWidget(causeTipText1, 1, 0);
    layout5->addWidget(m_ignoreMessageCauseComboBox, 1, 1);
    layout5->addWidget(m_ignoreMessageButton, 2, 0, 1, 2);
    layout5->setAlignment(m_ignoreMessageButton, Qt::AlignRight);

    // 纸券取走及状态信息
    m_paperWorkStationComboBox = new ElaComboBox(this);
    m_paperWorkStationComboBox->addItems({"默认工位", "上工位", "下工位"});
    m_paperCauseComboBox = new ElaComboBox(this);
    m_paperCauseComboBox->addItems({"正常取走纸券", "设备状态"});
    m_paperStatusUpBoxNoComboBox = new ElaComboBox(this);
    m_paperStatusUpBoxNoComboBox->addItems({"设备正常", "打印机故障", "卡纸", "缺纸", "无设备"});
    m_paperStatusDownBoxNoComboBox = new ElaComboBox(this);
    m_paperStatusDownBoxNoComboBox->addItems({"设备正常", "打印机故障", "卡纸", "缺纸", "无设备"});
    m_paperButton = new ElaPushButton("纸券取走", this);
    m_paperNumEdit = new ElaLineEdit(this);
    m_paperNumEdit->setPlaceholderText("请输入纸券号");
    m_paperNumEdit->setText("1234567");
    m_paperNumEdit->setMaximumWidth(200);

    QGroupBox *box6 = new QGroupBox("纸券取走及状态信息", this);
    QGridLayout *layout6 = new QGridLayout(box6);
    layout6->setContentsMargins(3, 3, 3, 3);
    layout6->setSpacing(0);
    layout6->addWidget(workStationTipText6, 0, 0);
    layout6->addWidget(m_paperWorkStationComboBox, 0, 1);
    layout6->addWidget(causeTipText2, 0, 2);
    layout6->addWidget(m_paperCauseComboBox, 0, 3);
    layout6->addWidget(upPrinterTipText, 1, 0);
    layout6->addWidget(m_paperStatusUpBoxNoComboBox, 1, 1);
    layout6->addWidget(downPrinterTipText, 1, 2);
    layout6->addWidget(m_paperStatusDownBoxNoComboBox, 1, 3);
    layout6->addWidget(paperNumTipText, 2, 0);
    layout6->addWidget(m_paperNumEdit, 2, 1, 1, 2);
    layout6->addWidget(m_paperButton, 2, 1, 1, 3);
    layout6->setAlignment(m_paperButton, Qt::AlignRight);

    ElaFlowLayout *partFLayout3 = new ElaFlowLayout(0, 5, 5);
    partFLayout3->setContentsMargins(0, 0, 0, 0);
    partFLayout3->setIsAnimation(true);
    partFLayout3->addWidget(box2);
    partFLayout3->addWidget(box1);
    partFLayout3->addWidget(box3);
    partFLayout3->addWidget(box4);
    partFLayout3->addWidget(box5);
    partFLayout3->addWidget(box6);

    // 局部布局6
    ElaText *logTipText = new ElaText("交互日志显示区", this);
    QFont font = logTipText->font();
    font.setWeight(QFont::Bold);
    logTipText->setFont(font);
    logTipText->setTextPixelSize(15);
    logTipText->setIsWrapAnywhere(false);

    m_logClearButton = new ElaPushButton("清除", this);

    QHBoxLayout *partHLayout4 = new QHBoxLayout();
    partHLayout4->setContentsMargins(0, 0, 0, 0);
    partHLayout4->addWidget(logTipText);
    partHLayout4->addWidget(m_logClearButton);
    partHLayout4->addStretch();

    // 局部控件7
    m_logEdit = new ElaPlainTextEdit(this);
    m_logEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    m_logEdit->setReadOnly(true);
    m_logEdit->setPlaceholderText("交互日志");

    // 整体布局
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("自动发卡机测试工具");
    QVBoxLayout *centralVLayout = new QVBoxLayout(centralWidget);
    centralVLayout->setContentsMargins(0, 0, 5, 0);
    centralVLayout->addSpacing(5);
    centralVLayout->addLayout(partHLayout1);
    centralVLayout->addLayout(partHLayout2);
    centralVLayout->addWidget(showArea);
    centralVLayout->addLayout(partFLayout3);
    centralVLayout->addLayout(partHLayout4);
    centralVLayout->addWidget(m_logEdit);

    addCentralWidget(centralWidget, true, true, 0);
}

void T_CardRobot::initRobotHandler()
{
    uchar defWorkStation = m_workStationComboBox->currentIndex() + 0x31;
    uchar defUpBoxNo = m_upBoxNoComboBox->currentIndex() + 0x31;
    uchar defDownBoxNo = m_downBoxNoComboBox->currentIndex() + 0x33;

    if (m_robotHandler) {
        m_robotHandler->setDefInfo(defWorkStation, defUpBoxNo, defDownBoxNo);
    }
}

void T_CardRobot::onListen()
{
    if (m_listenButton->text() == QString("启动")) {
        QString portStr = m_portEdit->text().trimmed();
        if (portStr.isEmpty())
            return;

        bool ok = false;
        uint port = portStr.toUInt(&ok);
        if (!ok) {
            ElaMessageBar::error(ElaMessageBarType::BottomRight, "启动失败", "端口号填写错误，请检查", 1000, this);
            LOG_CERROR("cardrobot").noquote() << "启动监听失败: 端口号填写错误";
            return;
        }
        if (port > 65535) {
            ElaMessageBar::error(ElaMessageBarType::BottomRight, "启动失败", "端口号最大不超过65535，请检查", 1000, this);
            LOG_CERROR("cardrobot").noquote() << "启动监听失败: 端口号最大不超过65535";
            return;
        }

        m_isListening = m_robotHandler->startListen(static_cast<quint16>(port));
        if (m_isListening) {
            m_listenButton->setText("断连");
            m_heartTimer->start(); // 启动心跳发送
        }
    } else {
        m_robotHandler->stopListen();
        m_listenButton->setText("启动");
        m_isListening = false;
        m_heartTimer->stop(); // 关闭心跳发送
    }
}

void T_CardRobot::onPowerOnMsgSend()
{
    if (!m_isListening)
        return;

    NloJson nloJson;
    QByteArray json = nloJson.serialize({});
    emit GM_INSTANCE->m_signalMan->sigRobotSendCommand(0x41, json);
}

void T_CardRobot::onOutCardMsgSend()
{
    if (!m_isListening)
        return;

    int curWorkStation = m_outCardWorkStationComboBox->currentIndex();
    int curBoxNo = m_outCardBoxNoComboBox->currentIndex();

    QVariantMap oneMap;
    oneMap["workStation"] = curWorkStation + 0x31;
    oneMap["boxNo"] = curBoxNo + 0x31;

    NloJson nloJson;
    QByteArray json = nloJson.serialize(oneMap);
    emit GM_INSTANCE->m_signalMan->sigRobotSendCommand(0x43, json);
}

void T_CardRobot::onRequireCardMsgSend()
{
    if (!m_isListening)
        return;

    int curWorkStation = m_requireCardWorkStationComboBox->currentIndex();
    int curBoxNo = m_requireCardBoxNoComboBox->currentIndex();

    QVariantMap oneMap;
    oneMap["workStation"] = curWorkStation + 0x31;
    oneMap["boxNo"] = curBoxNo + 0x31;

    NloJson nloJson;
    QByteArray json = nloJson.serialize(oneMap);
    emit GM_INSTANCE->m_signalMan->sigRobotSendCommand(0x44, json);
}

void T_CardRobot::onRetriveCardMsgSend()
{
    if (!m_isListening)
        return;

    int curWorkStation = m_retriveCardWorkStationComboBox->currentIndex();
    int curBoxNo = m_retriveCardBoxNoComboBox->currentIndex();

    QVariantMap oneMap;
    oneMap["workStation"] = curWorkStation + 0x31;
    oneMap["boxNo"] = curBoxNo + 0x31;

    NloJson nloJson;
    QByteArray json = nloJson.serialize(oneMap);
    emit GM_INSTANCE->m_signalMan->sigRobotSendCommand(0x45, json);
}

void T_CardRobot::onRecycleCardMsgSend()
{
    if (!m_isListening)
        return;

    int curWorkStation = m_recycleCardWorkStationComboBox->currentIndex();
    int curBoxNo = m_recycleCardBoxNoComboBox->currentIndex();

    QVariantMap oneMap;
    oneMap["workStation"] = curWorkStation + 0x31;
    oneMap["boxNo"] = curBoxNo + 0x31;

    NloJson nloJson;
    QByteArray json = nloJson.serialize(oneMap);
    emit GM_INSTANCE->m_signalMan->sigRobotSendCommand(0x47, json);
}

void T_CardRobot::onIgnoreMsgSend()
{
    if (!m_isListening)
        return;

    int curWorkStation = m_ignoreMessageWorkStationComboBox->currentIndex();
    int curCause = m_ignoreMessageCauseComboBox->currentIndex();

    QVariantMap oneMap;
    oneMap["workStation"] = curWorkStation + 0x31;
    oneMap["cause"] = curCause + 0x30;

    NloJson nloJson;
    QByteArray json = nloJson.serialize(oneMap);
    emit GM_INSTANCE->m_signalMan->sigRobotSendCommand(0x48, json);
}

void T_CardRobot::onPaperMsgSend()
{
    if (!m_isListening)
        return;

    int curWorkStation = m_paperWorkStationComboBox->currentIndex();
    int curCause = m_paperCauseComboBox->currentIndex();
    int curUpStatus = m_paperStatusUpBoxNoComboBox->currentIndex();
    int curDownStatus = m_paperStatusDownBoxNoComboBox->currentIndex();
    QString curPaperNum = m_paperNumEdit->text().trimmed();

    if (curPaperNum.isEmpty() && curCause == 0) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "发送失败", "纸券号不能为空，请注意", 1000, this);
        return;
    }

    QVariantMap oneMap;
    oneMap["workStation"] = curWorkStation + 0x30;
    oneMap["cause"] = curCause + 0x30;
    oneMap["paperNum"] = curPaperNum;
    oneMap["statusUp"] = curUpStatus + 0x30;
    oneMap["statusDown"] = curDownStatus + 0x30;

    NloJson nloJson;
    QByteArray json = nloJson.serialize(oneMap);
    emit GM_INSTANCE->m_signalMan->sigRobotSendCommand(0x49, json);
}

void T_CardRobot::onRobotSendCommandFinish(uchar cmdType, bool res)
{
    if (res)
        return;

    QString msg;
    switch (cmdType) {
    case 0x31:
        msg = "初始化指令应答失败，请注意";
        break;
    case 0x32:
        msg = "出卡指令应答失败，请注意";
        break;
    case 0x33:
        msg = "坏卡指令应答失败，请注意";
        break;
    case 0x34:
        msg = "触发按键指令应答失败，请注意";
        break;
    case 0x35:
        msg = "卡回收指令应答失败，请注意";
        break;
    case 0x36:
        msg = "车道设备状态指令应答失败，请注意";
        break;
    case 0x37:
        msg = "按键取消指令应答失败，请注意";
        break;
    case 0x39:
        msg = "界面显示指令应答失败，请注意";
        break;
    case 0x3A:
        msg = "语音播放指令应答失败，请注意";
        break;
    case 0x3B:
        msg = "设置状态上报URL指令应答失败，请注意";
        break;
    case 0x3D:
        msg = "打印纸券指令应答失败，请注意";
        break;
    case 0x50:
        msg = "副屏显示指令应答失败，请注意";
        break;
    case 0x51:
        msg = "二维码显示指令应答失败，请注意";
        break;
    case 0x41:
        msg = "上电信息发送失败，请注意";
        break;
    case 0x42:
        msg = "状态信息发送失败，请注意";
        break;
    case 0x43:
        msg = "出卡信息发送失败，请注意";
        break;
    case 0x44:
        msg = "按键取卡信息发送失败，请注意";
        break;
    case 0x45:
        msg = "卡被取走信息发送失败，请注意";
        break;
    case 0x46:
        msg = "卡夹信息发送失败，请注意";
        break;
    case 0x47:
        msg = "卡回收完成信息发送失败，请注意";
        break;
    case 0x48:
        msg = "按键忽略信息发送失败，请注意";
        break;
    case 0x49:
        msg = "纸券取走及状态信息发送失败，请注意";
        break;
    }
    ElaMessageBar::error(ElaMessageBarType::BottomRight, "发送失败", msg, 1000, this);
}

void T_CardRobot::onRobotReceiveCommand(uchar cmdType, QByteArray json)
{
    NloJson nloJson;

    bool ok = false;
    QVariantMap dataMap = nloJson.parse(json, &ok).toMap();
    if (!ok) {
        LOG_CERROR("cardrobot").noquote() << "Json解析失败，数据内容错误";
        return;
    }

    switch (cmdType) {
    case 0x39: {
        QString uiInfo = dataMap["uiInfo"].toString();
        displayMainScreen(uiInfo);
    } break;
    case 0x3A: {
        uchar voiceNum = static_cast<uchar>(dataMap["voiceNum"].toInt());
        uchar playCount = static_cast<uchar>(dataMap["playCount"].toInt());
        if (voiceNum == 10) {
            emit GM_INSTANCE->m_signalMan->sigRobotScreenInit();
        } else {
            playSound(voiceNum, playCount);
        }
    } break;
    case 0x50: {
        QString text = dataMap["text"].toString();
        displaySecondScreen(text);
    } break;
    case 0x51: {
        QString text = dataMap["text"].toString();
        QImage qrCodeImg = genQRCode(text, 300);
        displayQRCode(qrCodeImg);
    } break;
    default:
        break;
    }
}

bool T_CardRobot::playSound(uchar voiceNum, uchar playCount)
{
    if (!m_player || !m_playlist)
        return false;

    if (!m_allowPlayCheckBox->isChecked())
        return false;

    if (playCount <= 0)
        return false;

    QString soundsDir = FileUtils::curApplicationDirPath() + Constant::APP::SOUND_PATH;
    FileName filePath = FileName::fromString(QString("%1/%2.mp3").arg(soundsDir).arg(voiceNum));
    if (!filePath.exists()) {
        LOG_CERROR("cardrobot").noquote() << QString("语音文件不存在: %1").arg(filePath.fileName(1));
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "播放失败", "语音文件不存在", 1000, this);
        return false;
    }

    // 清空播放列表
    m_playlist->clear();
    QUrl url = QUrl::fromLocalFile(filePath.toString());

    // 添加多次
    for (int i = 0; i < playCount; ++i)
        m_playlist->addMedia(url);

    m_playlist->setCurrentIndex(0);

    m_player->stop();
    QTimer::singleShot(0, this, [this]() { m_player->play(); });

    return true;
}

QImage T_CardRobot::genQRCode(const QString &text, int size)
{
    if (text.isEmpty()) {
        return QImage(Constant::APP::ICON_PATH);
    }

    QZXing encoder;
    QImage image = encoder.encodeData(text, QZXing::EncoderFormat_QR_CODE, QSize(size, size), QZXing::EncodeErrorCorrectionLevel_H);

    return image;
}

void T_CardRobot::displaySecondScreen(const QString &text)
{
    QStringList showInfos = text.split(";", Qt::SkipEmptyParts);
    if (showInfos.size() > 3) {
        m_secondScreenText->setTextStyle(ElaTextType::BodyStrong);
    } else {
        m_secondScreenText->setTextStyle(ElaTextType::Subtitle);
    }
    QString showInfo = showInfos.join('\n');
    m_secondScreenText->setText(showInfo);
}

void T_CardRobot::displayMainScreen(const QString &text)
{
    QStringList showInfos = text.split(";", Qt::SkipEmptyParts);
    if (showInfos.size() > 3) {
        m_mainScreenText->setTextStyle(ElaTextType::BodyStrong);
    } else {
        m_mainScreenText->setTextStyle(ElaTextType::Subtitle);
    }
    QString showInfo = showInfos.join('\n');
    m_mainScreenText->setText(showInfo);
}

void T_CardRobot::displayQRCode(const QImage &img)
{
    m_qrCodeImage->setCardImage(img);
    m_qrCodeImage->update();
}
