#include "t_infoboard.h"

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaMessageBar.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"
#include "ElaRadioButton.h"
#include "ElaText.h"
#include "HttpClient/src/http.h"
#include "Logger.h"
#include "NlohmannJson/nlojson.hpp"
#include "bend/infoboard/infoboardhandler.h"
#include "qhostaddress.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"

using namespace Utils;

T_InfoBoard::T_InfoBoard(QWidget *parent)
    : T_BasePage{parent}
{
    m_boardContentMap = {{"01101", "自助/ETC-车道正常(开道)"}, {"02101", "现金/ETC-车道正常(开道)"}, {"04000", "车道关闭-(闭道)"},
                         {"01102", "自助/ETC-车辆退道(开道)"}, {"02102", "现金/ETC-车辆退道(开道)"}, {"01103", "自助/ETC-车道拥堵(开道)"},
                         {"02103", "现金/ETC-车道拥堵(开道)"}, {"02104", "现金/ETC-绿通专用(开道)"}, {"01105", "自助/ETC-客车通道(开道)"},
                         {"04002", "车道关闭-车辆退道(闭道)"}, {"03101", "ETC专用-车道正常(开道)"},  {"03103", "ETC专用-车道拥堵(开道)"},
                         {"03105", "ETC专用-客车通道(开道)"},  {"03106", "ETC专用-货车通道(开道)"},  {"01104", "自助/ETC-绿通专用(开道)"},
                         {"04003", "车道关闭-车道拥堵(闭道)"}};
    m_infoBoardHandler = new InfoBoardHandler(this);
    m_infoBoardHandler->enableRetryConnect(true); // 启用断线重连

    // 预览窗口标题
    setWindowTitle("折叠情报板测试工具");
    // 顶部元素
    createCustomWidget("测试折叠情报板是否正常工作");

    initContent();

    connect(m_httpButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked && m_infoBoardHandler->isConnected()) {
            m_infoBoardHandler->disconnectServer();
        }
        m_connectInfoEdit->clear();
        m_connectButton->setDisabled(checked);
        if (checked) {
            LOG_CINFO("infoboard").noquote() << "选择使用Http协议进行通讯";
            m_connectInfoEdit->setPlaceholderText("请输入折叠情报板控制Url");
        } else {
            LOG_CINFO("infoboard").noquote() << "选择使用Tcp协议进行通讯";
            m_connectInfoEdit->setPlaceholderText("请输入情报板IP地址与端口,用冒号分隔");
        }
    });
    connect(m_connectButton, &ElaPushButton::clicked, this, &T_InfoBoard::onConnectServer);
    connect(m_sendButton, &ElaPushButton::clicked, this, &T_InfoBoard::onSendMsg);
    connect(m_confirmButton, &ElaPushButton::clicked, this, &T_InfoBoard::onSetupUrl);

    connect(m_infoBoardHandler, &InfoBoardHandler::sigNetworkStatusChanged, this, [=](bool isOnline) {
        if (isOnline) {
            updateNetworkStatus("连接正常", true);
            m_isTcpConnected = true;
        } else {
            updateNetworkStatus("连接断开", false);
            m_isTcpConnected = false;
        }
    });
    connect(m_infoBoardHandler, &InfoBoardHandler::sigDevStatusChanged, this, [=](bool isNormal) {
        if (isNormal) {
            updateDevStatus("状态正常");
        } else {
            updateDevStatus("状态异常", true);
        }
    });

    // 界面显示日志信息
    connect(cuteLogger, &Logger::sigLogWrite, this, [=](const QString &log, const QString &cat) {
        if (cat == "infoboard") {
            m_logEdit->appendPlainText(DataDealUtils::curDateTimeStr() + " | " + log + "\n");
        }
    });
}

T_InfoBoard::~T_InfoBoard() {}

void T_InfoBoard::initContent()
{
    // 局部布局1
    m_connectInfoEdit = new ElaLineEdit(this);
    m_connectInfoEdit->setPlaceholderText("请输入情报板IP地址与端口,用冒号分隔");
    m_connectInfoEdit->setFixedSize(350, 35);

    m_connectButton = new ElaPushButton("连接", this);

    ElaText *networkTip = new ElaText("协议类型", this);
    networkTip->setTextPixelSize(15);
    networkTip->setWordWrap(false);

    m_httpButton = new ElaRadioButton("http", this);
    m_tcpButton = new ElaRadioButton("tcp", this);
    m_tcpButton->setChecked(true);

    QButtonGroup *protocolGroup = new QButtonGroup(this);
    protocolGroup->addButton(m_httpButton);
    protocolGroup->addButton(m_tcpButton);

    QHBoxLayout *partHLayout1 = new QHBoxLayout();
    partHLayout1->setContentsMargins(0, 0, 0, 0);
    partHLayout1->addWidget(m_connectInfoEdit);
    partHLayout1->addWidget(m_connectButton);
    partHLayout1->addStretch();
    partHLayout1->addWidget(networkTip);
    partHLayout1->addWidget(m_httpButton);
    partHLayout1->addWidget(m_tcpButton);

    // 局部布局2
    m_monitorUrlEdit = new ElaLineEdit(this);
    m_monitorUrlEdit->setPlaceholderText("设置状态上报Url地址(可选)");
    m_monitorUrlEdit->setFixedSize(350, 35);

    m_confirmButton = new ElaPushButton("设置", this);

    ElaText *devStatusTip = new ElaText("设备状态:", this);
    m_devStatusText = new ElaText("状态异常");
    ElaText *networkStatusTip = new ElaText("TCP连接:", this);
    m_networkStatusText = new ElaText("连接断开");

    QList<ElaText *> texts = {devStatusTip, m_devStatusText, networkStatusTip, m_networkStatusText};
    for (auto text : texts) {
        text->setTextPixelSize(15);
        text->setWordWrap(false);
    }
    m_devStatusText->setStyleSheet("color: #ff0000");
    m_networkStatusText->setStyleSheet("color: #ff0000");

    QHBoxLayout *partHLayout2 = new QHBoxLayout();
    partHLayout2->setContentsMargins(0, 0, 0, 0);
    partHLayout2->addWidget(m_monitorUrlEdit);
    partHLayout2->addWidget(m_confirmButton);
    partHLayout2->addStretch();
    partHLayout2->addWidget(devStatusTip);
    partHLayout2->addWidget(m_devStatusText);
    partHLayout2->addSpacing(10);
    partHLayout2->addWidget(networkStatusTip);
    partHLayout2->addWidget(m_networkStatusText);

    // 局部布局3
    ElaText *fileSelectText = new ElaText("显示内容选择", this);
    fileSelectText->setTextPixelSize(15);
    fileSelectText->setWordWrap(false);

    m_filesComboBox = new ElaComboBox(this);
    m_filesComboBox->addItems(m_boardContentMap.values());

    m_sendButton = new ElaPushButton("发送", this);

    ElaText *infoBoardTypeText = new ElaText("情报板类型", this);
    infoBoardTypeText->setTextPixelSize(15);
    infoBoardTypeText->setWordWrap(false);

    m_hxButton = new ElaRadioButton("昊星", this);
    m_xlButton = new ElaRadioButton("兴陆", this);
    m_hxButton->setChecked(true);

    QButtonGroup *boardTypeGroup = new QButtonGroup(this);
    boardTypeGroup->addButton(m_hxButton);
    boardTypeGroup->addButton(m_xlButton);

    QHBoxLayout *partHLayout3 = new QHBoxLayout();
    partHLayout3->setContentsMargins(0, 0, 0, 0);
    partHLayout3->addWidget(fileSelectText);
    partHLayout3->addWidget(m_filesComboBox);
    partHLayout3->addWidget(m_sendButton);
    partHLayout3->addStretch();
    partHLayout3->addWidget(infoBoardTypeText);
    partHLayout3->addWidget(m_hxButton);
    partHLayout3->addWidget(m_xlButton);

    // 局部控件4
    ElaText *logTipText = new ElaText("交互日志显示区", this);
    logTipText->setTextPixelSize(15);
    logTipText->setWordWrap(false);
    logTipText->setFixedHeight(35);

    // 局部控件5
    m_logEdit = new ElaPlainTextEdit(this);
    m_logEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    m_logEdit->setReadOnly(true);
    m_logEdit->setPlaceholderText("交互日志");

    // 整体布局
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("折叠情报板测试工具");
    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setContentsMargins(0, 0, 5, 0);
    centralLayout->addSpacing(13);
    centralLayout->addLayout(partHLayout1);
    centralLayout->addLayout(partHLayout2);
    centralLayout->addLayout(partHLayout3);
    centralLayout->addWidget(logTipText);
    centralLayout->addWidget(m_logEdit);

    addCentralWidget(centralWidget, true, true, 0);
}

void T_InfoBoard::onConnectServer()
{
    if (m_isTcpConnected) {
        m_infoBoardHandler->disconnectServer();
        return;
    }

    QString info = m_connectInfoEdit->text().simplified();
    if (info.isEmpty()) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "连接失败", "连接信息为空，请检查", 1000, this);
        LOG_CERROR("infoboard").noquote() << "Tcp连接折叠情报板失败: 连接信息为空";
        return;
    }
    QStringList parts = info.split(":");
    if (parts.size() != 2) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "连接失败", "连接信息填写错误，请检查", 1000, this);
        LOG_CERROR("infoboard").noquote() << "Tcp连接折叠情报板失败: 连接信息填写错误";
        return;
    }

    QString ip = parts.at(0).trimmed();
    QHostAddress addr;
    if (!addr.setAddress(ip)) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "连接失败", "IP地址格式错误，请检查", 1000, this);
        LOG_CERROR("infoboard").noquote() << "Tcp连接折叠情报板失败: IP地址格式错误";
        return;
    }

    QString portStr = parts.at(1).trimmed();
    bool ok = false;
    int port = portStr.toInt(&ok);
    if (!ok || port <= 0 || port > 65535) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "连接失败", "端口号非法(1-65535)，请检查", 1000, this);
        LOG_CERROR("infoboard").noquote() << "Tcp连接折叠情报板失败: 端口号非法(1-65535)";
        return;
    }

    // 发起连接
    m_infoBoardHandler->connectServer(ip, static_cast<quint16>(port));
}

void T_InfoBoard::onSendMsg()
{
    QString selectedText = m_filesComboBox->currentText();
    // 反查 key
    QString fileKey;
    for (auto it = m_boardContentMap.begin(); it != m_boardContentMap.end(); ++it) {
        if (it.value() == selectedText) {
            fileKey = it.key();
            break;
        }
    }

    // 拼接获取播放文件路径
    FileName playFilesDir = FileName::fromString(FileUtils::curApplicationDirPath() + "/playfiles");
    if (m_hxButton->isChecked()) {
        playFilesDir = playFilesDir.appendPath("/HX");
    } else {
        playFilesDir = playFilesDir.appendPath("/XL");
    }
    FileName filePath = playFilesDir.appendPath("play" + fileKey + ".lst");
    if (!filePath.exists()) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "发送失败", QString("播放列表文件%1不存在，请注意").arg(filePath.fileName()), 1000, this);
        LOG_CERROR("infoboard").noquote() << "数据发送失败：" << QString("播放列表文件%1不存在").arg(filePath.fileName());
        return;
    }

    // 开始读取文件内容，并发送
    FileReader reader;
    if (!reader.fetch(filePath.toString())) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "发送失败", QString("播放列表文件%1读取失败，请注意").arg(filePath.fileName()), 1000,
                             this);
        LOG_CERROR("infoboard").noquote() << "数据发送失败：播放列表文件" << filePath.fileName() << "读取失败:" << reader.errorString();
        return;
    }

    LOG_CINFO("infoboard").noquote() << "发送内容: \n" << reader.data();

    if (m_httpButton->isChecked()) { // http协议发送
        QString urlStr = m_connectInfoEdit->text().simplified();
        QUrl url(urlStr);
        if (!url.isValid()) {
            ElaMessageBar::error(ElaMessageBarType::BottomRight, "发送失败", "情报板Url无效，请检查", 1000, this);
            LOG_CERROR("infoboard").noquote() << "数据发送失败：情报板Url无效";
            return;
        }

        QByteArray data = m_infoBoardHandler->constructFilePublishAndShowImmediateFrame(reader.data());
        QVariantMap sendMap;
        sendMap["data"] = data;

        NloJson nloJson;
        QByteArray sendData = nloJson.serialize(sendMap);

        Http::instance().post(url, sendData, "application/json");
        // TODO 返回json待确认
    } else { // tcp协议发送
        if (!m_isTcpConnected) {
            ElaMessageBar::error(ElaMessageBarType::BottomRight, "发送失败", "与情报板未建立连接，请注意", 1000, this);
            LOG_CERROR("infoboard").noquote() << "数据发送失败：未与情报板建立连接";
            return;
        }

        m_infoBoardHandler->filePublishAndShowImmediate(reader.data());
    }
}

void T_InfoBoard::onSetupUrl()
{
    QString urlStr = m_monitorUrlEdit->text().simplified();
    QUrl url(urlStr);
    if (!url.isValid()) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "设置失败", "状态上报url无效，请检查", 1000, this);
        LOG_CERROR("infoboard").noquote() << "设置状态上报url失败: 无效url";
        return;
    }

    if (m_httpButton->isChecked()) { // http协议发送
        QString devUrlStr = m_connectInfoEdit->text().simplified();
        QUrl devUrl(devUrlStr);
        if (!devUrl.isValid()) {
            ElaMessageBar::error(ElaMessageBarType::BottomRight, "发送失败", "情报板Url无效，请检查", 1000, this);
            LOG_CERROR("infoboard").noquote() << "数据发送失败：情报板Url无效";
            return;
        }

        QByteArray data = m_infoBoardHandler->constructSetupUrlFrame(devUrlStr, 10, true);
        QVariantMap sendMap;
        sendMap["data"] = data;

        NloJson nloJson;
        QByteArray sendData = nloJson.serialize(sendMap);

        Http::instance().post(url, sendData, "application/json");
        // TODO 返回json待确认
    } else { // tcp协议发送
        if (!m_isTcpConnected) {
            ElaMessageBar::error(ElaMessageBarType::BottomRight, "设置失败", "与情报板未建立连接，请注意", 1000, this);
            LOG_CERROR("infoboard").noquote() << "设置状态上报url失败：未与情报板建立连接";
            return;
        }

        LOG_CINFO("infoboard").noquote() << "设置状态上报Url:" << urlStr;
        m_infoBoardHandler->setupUrl(urlStr, 10, true);
    }
}

// 私有函数
void T_InfoBoard::updateDevStatus(const QString &status, bool isWarning /*= false*/)
{
    m_devStatusText->setText(status);
    if (isWarning) {
        m_devStatusText->setStyleSheet("color: #ff0000");
    } else {
        m_devStatusText->setStyleSheet("color: #28bf74");
    }
}

void T_InfoBoard::updateNetworkStatus(const QString &status, bool isOnline)
{
    m_networkStatusText->setText(status);
    if (!isOnline) {
        m_networkStatusText->setStyleSheet("color: #ff0000");
        m_connectButton->setText("连接");
    } else {
        m_networkStatusText->setStyleSheet("color: #28bf74");
        m_connectButton->setText("断连");
    }
}
