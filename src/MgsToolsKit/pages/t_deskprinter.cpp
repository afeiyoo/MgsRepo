#include "t_deskprinter.h"

#include <QHBoxLayout>
#include <QJsonParseError>
#include <QVBoxLayout>

#include "ElaLineEdit.h"
#include "ElaMessageBar.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"
#include "ElaRadioButton.h"
#include "ElaText.h"
#include "HttpClient/src/http.h"
#include "Logger.h"
#include "utils/datadealutils.h"

using namespace Utils;

T_DeskPrinter::T_DeskPrinter(QWidget *parent)
    : T_BasePage{parent}
{
    // 预览窗口标题
    setWindowTitle("桌面打印机测试工具");
    // 顶部元素
    createCustomWidget("测试桌面打印机（基于Http协议）是否正常工作");

    initContent();

    connect(m_inputButton, &ElaPushButton::clicked, this, [=]() {
        QString defPackage = QString(
            R"({"enNetID":"3501","enStationID":"6701","enStationName":"福州西收费站","vehclass":"客一","vehPlate":"蓝闽A123456","enTime":"2023-01-04 16:44:43","ticketNum":"35240017100001","barCode":"35016701"})");
        m_inputEdit->setPlainText(defPackage);
    });

    connect(m_sendButton, &ElaPushButton::clicked, this, &T_DeskPrinter::onPostToPrinter);
}

T_DeskPrinter::~T_DeskPrinter() {}

void T_DeskPrinter::initContent()
{
    // 局部布局1
    m_urlEdit = new ElaLineEdit(this);
    m_urlEdit->setPlaceholderText("请输入桌面打印机url");
    m_urlEdit->setFixedSize(350, 35);

    m_sendButton = new ElaPushButton("发送", this);

    ElaText *codingTypeText = new ElaText("报文编码类型", this);
    codingTypeText->setTextPixelSize(15);
    codingTypeText->setWordWrap(false);

    m_gbkButton = new ElaRadioButton("GBK", this);
    m_utfButton = new ElaRadioButton("UTF-8", this);
    m_gbkButton->setChecked(true);

    QHBoxLayout *partHLayout1 = new QHBoxLayout();
    partHLayout1->setContentsMargins(0, 0, 0, 0);
    partHLayout1->addWidget(m_urlEdit);
    partHLayout1->addWidget(m_sendButton);
    partHLayout1->addStretch();
    partHLayout1->addWidget(codingTypeText);
    partHLayout1->addWidget(m_gbkButton);
    partHLayout1->addWidget(m_utfButton);

    // 局部布局2
    ElaText *inputTipText = new ElaText("发送报文输入区", this);
    inputTipText->setTextPixelSize(15);
    inputTipText->setWordWrap(false);
    m_inputButton = new ElaPushButton("默认报文", this);
    m_inputButton->setFixedHeight(35);

    QHBoxLayout *partHLayout2 = new QHBoxLayout();
    partHLayout2->setContentsMargins(0, 0, 0, 0);
    partHLayout2->addWidget(inputTipText);
    partHLayout2->addStretch();
    partHLayout2->addWidget(m_inputButton);

    // 局部控件3
    m_inputEdit = new ElaPlainTextEdit(this);
    m_inputEdit->setFixedHeight(100);
    m_inputEdit->setPlaceholderText("请输入发送报文内容");

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
    centralWidget->setWindowTitle("桌面打印机测试工具");
    QVBoxLayout *centralVLayout = new QVBoxLayout(centralWidget);
    centralVLayout->setContentsMargins(0, 0, 5, 0);
    centralVLayout->addSpacing(13);
    centralVLayout->addLayout(partHLayout1);
    centralVLayout->addLayout(partHLayout2);
    centralVLayout->addWidget(m_inputEdit);
    centralVLayout->addWidget(logTipText);
    centralVLayout->addWidget(m_logEdit);

    addCentralWidget(centralWidget, true, true, 0);
}

void T_DeskPrinter::onPostToPrinter(bool checked)
{
    Q_UNUSED(checked);

    QString urlStr = m_urlEdit->text();
    QUrl url(urlStr);
    if (!url.isValid()) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "发送失败", "桌面打印机url无效，请检查", 1000, this);
        LOG_ERROR().noquote() << "发送失败，桌面打印机url无效";
        return;
    }

    QString sendContent = m_inputEdit->toPlainText();
    if (sendContent.isEmpty()) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "发送失败", "发送报文为空，请检查", 1000, this);
        LOG_ERROR().noquote() << "发送失败，发送报文为空";
        return;
    }

    // 检查是否是合法的json
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(sendContent.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "发送失败", "JSON格式错误，请检查", 1000, this);
        LOG_ERROR().noquote() << "发送失败，JSON格式错误";
        return;
    }

    // 避免连续点击
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (now - m_lastSendTime < 1000) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight, "警告消息", "操作过快，请勿连续点击发送", 1000, this);
        LOG_WARNING().noquote() << "操作过快警告，用户连续点击发送按钮";
        return;
    }
    m_lastSendTime = now;

    QByteArray encodingContent;
    if (m_gbkButton->isChecked()) {
        encodingContent = DataDealUtils::encodeString(sendContent, 1);
    } else {
        encodingContent = DataDealUtils::encodeString(sendContent, 2);
    }

    m_logEdit->appendPlainText(DataDealUtils::curDateTimeStr() + " | 发送内容: " + sendContent + "\n");
    LOG_INFO().noquote() << "发送内容: " << sendContent;

    auto reply = Http::instance().post(url, encodingContent, "application/json");
    connect(reply, &HttpReply::data, this, [=](const QByteArray &bytes) {
        m_logEdit->appendPlainText(DataDealUtils::curDateTimeStr() + " | 返回成功: " + bytes + "\n");
        LOG_INFO().noquote() << "返回成功: " << bytes;
    });
    connect(reply, &HttpReply::error, this, [=](const QString &message) {
        m_logEdit->appendPlainText(DataDealUtils::curDateTimeStr() + " | 返回错误: " + message + "\n");
        LOG_ERROR().noquote() << "返回错误: " << message;
    });
}
