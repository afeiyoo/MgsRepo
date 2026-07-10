#include "authdialog.h"

#include "ElaWidgetTools/ElaImageCard.h"
#include "ElaWidgetTools/ElaLineEdit.h"
#include "ElaWidgetTools/ElaPushButton.h"
#include "ElaWidgetTools/ElaText.h"
#include "global/uiconst.h"
#include "utils/uiutils.h"

#include <QCoreApplication>
#include <QDebug>
#include <QEvent>
#include <QFocusEvent>
#include <QHBoxLayout>
#include <QPainter>
#include <QVBoxLayout>

using namespace Utils;

AuthDialog::AuthDialog(QWidget *parent)
    : QDialog{parent}
{
    setWindowTitle("身份认证");
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint);
    setFixedSize(300, 270);

    initUi();
}

AuthDialog::~AuthDialog() {}

QString AuthDialog::getID()
{
    if (!m_idEdit)
        return "";
    return m_idEdit->text();
}

void AuthDialog::setID(const QString &id)
{
    if (!m_idEdit)
        return;
    m_idEdit->setText(id);
}

QString AuthDialog::getName()
{
    if (!m_nameEdit)
        return "";
    return m_nameEdit->text();
}

void AuthDialog::setName(const QString &name)
{
    if (!m_nameEdit)
        return;
    m_nameEdit->clear();
    m_nameEdit->setText(name);
}

QString AuthDialog::getPassWord()
{
    if (!m_passwordEdit)
        return "";
    return m_passwordEdit->text();
}

void AuthDialog::setPassWord(const QString &passwd)
{
    if (!m_passwordEdit)
        return;
    m_passwordEdit->clear();
    m_passwordEdit->setText(passwd);
}

void AuthDialog::initUi()
{
    m_mainWidget = new QWidget(this);
    m_mainWidget->setStyleSheet(QString("background-color: %1;").arg(Color::DIALOG_BG));
    QGridLayout *contentLayout = new QGridLayout(m_mainWidget);
    contentLayout->setContentsMargins(25, 20, 25, 20);
    contentLayout->setHorizontalSpacing(10);
    contentLayout->setVerticalSpacing(15);

    // 主内容
    ElaText *icon1 = new ElaText(m_mainWidget);
    icon1->setElaIcon(ElaIconType::IdCardClip);
    ElaText *icon2 = new ElaText(m_mainWidget);
    icon2->setElaIcon(ElaIconType::ChalkboardUser);
    ElaText *icon3 = new ElaText(m_mainWidget);
    icon3->setElaIcon(ElaIconType::LockKeyhole);
    QList<ElaText *> icons = {icon1, icon2, icon3};
    for (auto *icon : icons) {
        icon->setTextPixelSize(24);
        icon->setFixedSize(30, 30);
    }

    m_idEdit = new ElaLineEdit(m_mainWidget);
    m_idEdit->setPlaceholderText("工号");
    m_idEdit->setDisabled(true);
    m_nameEdit = new ElaLineEdit(m_mainWidget);
    m_nameEdit->setPlaceholderText("姓名");
    m_nameEdit->setDisabled(true);
    m_passwordEdit = new ElaLineEdit(m_mainWidget);
    m_passwordEdit->setPlaceholderText("密码");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    QList<ElaLineEdit *> inputs = {m_idEdit, m_nameEdit, m_passwordEdit};
    for (auto *input : inputs) {
        QFont font = input->font();
        font.setPixelSize(16);
        input->setFont(font);
        input->setFixedHeight(35);
        input->setIsClearButtonEnable(false);
        input->installEventFilter(this);
    }

    // 按钮组
    m_yesButton = new ElaPushButton("确认", m_mainWidget);
    m_yesButton->setLightDefaultColor(Color::CONFIRM_BUTTON_BG);
    m_yesButton->setLightTextColor(Color::BUTTON_TC);

    m_cancelButton = new ElaPushButton("取消", m_mainWidget);
    m_cancelButton->setLightDefaultColor(Color::WARN_BUTTON_BG);
    m_cancelButton->setLightTextColor(Color::BUTTON_TC);

    QList<ElaPushButton *> btnList = {m_yesButton, m_cancelButton};
    for (auto &btn : btnList) {
        QFont font = btn->font();
        font.setPixelSize(20);
        font.setWeight(QFont::DemiBold);
        btn->setFont(font);
        btn->setFixedHeight(40);
        btn->setBorderRadius(5);
        UiUtils::disableMouseEvents(btn);
    }

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setSpacing(0);
    btnLayout->addWidget(m_yesButton);
    btnLayout->addWidget(m_cancelButton);

    // 整体布局
    contentLayout->addWidget(icon1, 1, 0);
    contentLayout->addWidget(m_idEdit, 1, 1, 1, 3);
    contentLayout->addWidget(icon2, 2, 0);
    contentLayout->addWidget(m_nameEdit, 2, 1, 1, 3);
    contentLayout->addWidget(icon3, 3, 0);
    contentLayout->addWidget(m_passwordEdit, 3, 1, 1, 3);
    contentLayout->addLayout(btnLayout, 4, 0, 1, 4);

    // 提示
    QWidget *tipWidget = UiUtils::createTipWidget("输入密码后，按【确认】键登录系统，按【返回】键取消登录");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_mainWidget, 1);
    mainLayout->addWidget(tipWidget);
}

bool AuthDialog::eventFilter(QObject *obj, QEvent *event)
{
    // 屏蔽右键菜单
    if ((obj == m_idEdit || obj == m_nameEdit || obj == m_passwordEdit) && event->type() == QEvent::ContextMenu) {
        return true;
    }

    // 密码框特殊处理
    if (obj == m_passwordEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        int key = keyEvent->key();

        bool isDigit = (key >= Qt::Key_0 && key <= Qt::Key_9);
        bool isControlKey = key == Qt::Key_Backspace || key == Qt::Key_Delete || key == Qt::Key_Left || key == Qt::Key_Right;

        if (!(isDigit || isControlKey)) {
            // 交给父控件处理
            QCoreApplication::sendEvent(this, event);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}

void AuthDialog::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_S) {
        this->accept();
        event->accept();
    } else if (key == Qt::Key_X) {
        this->reject();
        event->accept();
    }

    QDialog::keyPressEvent(event);
}

void AuthDialog::showEvent(QShowEvent *event)
{
    m_idEdit->clear();
    m_nameEdit->clear();
    m_passwordEdit->clear();
    m_passwordEdit->setFocus();

    UiUtils::moveToCenter(this);
    QDialog::showEvent(event);
}
