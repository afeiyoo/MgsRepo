#include "uiutils.h"

#if defined(USE_ELAWIDGETTOOLS)
    #include "ElaWidgetTools/ElaText.h"
#endif

#include "utils/fileutils.h"

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFrame>
#include <QGraphicsBlurEffect>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <QPushButton>
#include <QScreen>
#include <QStyleOption>

using namespace Utils;

UiUtils::UiUtils(QObject *parent)
    : QObject(parent)
{}

UiUtils::~UiUtils() {}

void UiUtils::addLine(QBoxLayout *layout, int thickness /*= 3*/, int height /*= 10*/, QString color /*= #cdcdcd*/)
{
    if (!layout)
        return;

    QFrame *line = new QFrame(layout->parentWidget());
    line->setFrameShape(QFrame::NoFrame);
    line->setFrameShadow(QFrame::Raised);

    // 设置线条颜色
    QString style = QString("background-color: %1;").arg(color);
    line->setStyleSheet(style);

    // 设置粗细和大小策略
    line->setFixedWidth(thickness);
    line->setFixedHeight(height);
    line->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    layout->addWidget(line);
}

void UiUtils::applyShadow(QWidget *widget)
{
    if (!widget)
        return;

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(widget);
    shadow->setBlurRadius(8);
    shadow->setOffset(0, 2);
    shadow->setColor(QColor(0, 0, 0, 50));

    widget->setGraphicsEffect(shadow);
}

#if defined(USE_ELAWIDGETTOOLS)
QHBoxLayout *UiUtils::createTipWidget(const QString &tip)
{
    ElaText *tipIcon = new ElaText();
    tipIcon->setElaIcon(ElaIconType::MessageExclamation);
    tipIcon->setTextStyle(ElaTextType::Caption);
    tipIcon->setFixedWidth(15);

    ElaText *tipInfo = new ElaText();
    tipInfo->setTextStyle(ElaTextType::Caption);
    tipInfo->setIsWrapAnywhere(true);
    tipInfo->setText(tip);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(5);
    layout->addWidget(tipIcon);
    layout->addWidget(tipInfo, 1);

    return layout;
}
#endif

void UiUtils::moveToCenter(QWidget *widget)
{
    if (!widget || widget->isMaximized() || widget->isFullScreen()) {
        return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    // Qt 5.14+ 直接用 widget->screen()
    auto geometry = widget->screen()->availableGeometry();
#else
    // 老版本 Qt 需要用 qApp->screenAt() 找屏幕
    auto geometry = qApp->screenAt(widget->geometry().center())->geometry();
#endif

    int newX = (geometry.left() + geometry.right() - widget->width()) / 2;
    int newY = (geometry.top() + geometry.bottom() - widget->height()) / 2;
    widget->setGeometry(newX, newY, widget->width(), widget->height());
}

void UiUtils::disableMouseEvents(QWidget *widget)
{
    widget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    const auto children = widget->findChildren<QWidget *>();
    for (QWidget *child : children) {
        child->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    }
}

bool UiUtils::screenShot(FileName filePath, QString *error /*= nullptr*/)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) {
        if (error)
            *error = "No primary screen found!";
        return false;
    }

    QPixmap pixmap = screen->grabWindow(0);
    if (pixmap.isNull()) {
        if (error)
            *error = "Failed to capture screen!";
        return false;
    }

    FileUtils::makeSureDirExist(filePath);
    FileName savePath = filePath.appendPath(QString("%1.png").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss")));

    FileSaver saver(savePath.toString());
    if (saver.hasError()) {
        if (error)
            *error = saver.errorString();
        return false;
    }

    if (!pixmap.save(saver.file(), "PNG")) {
        if (error)
            *error = tr("Failed to write screenshot data!");
        return false;
    }

    if (!saver.finalize()) {
        if (error)
            *error = saver.errorString();
        return false;
    }
    return true;
}

QMessageBox::StandardButton UiUtils::showMessageBoxInfo(const QString &title, const QString &informativeText, QMessageBox::StandardButtons buttons,
                                                        bool switchLine /* = true*/)
{
    QMessageBox box;
    box.setWindowTitle(QObject::tr("提示"));
    box.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint);
    box.setTextFormat(Qt::RichText);
    box.setIcon(QMessageBox::Information);

    box.setText(QString("<span style='font-size:20px; font-weight:bold;'>%1</span>").arg(title));
    if (switchLine) {
        box.setInformativeText(QString("<span style='font-size:16px;'>%1</span>").arg(informativeText));
    } else {
        box.setInformativeText(QString("<div style='font-size:16px; white-space: pre;'>%1</div>").arg(informativeText));
    }

    box.setStandardButtons(buttons);

    QPushButton *okBtn = nullptr;
    QPushButton *cancelBtn = nullptr;
    for (QMessageBox::StandardButton b : {QMessageBox::Ok, QMessageBox::Cancel, QMessageBox::Yes, QMessageBox::No, QMessageBox::Close}) {
        if (buttons.testFlag(b)) {
            if (auto btn = qobject_cast<QPushButton *>(box.button(b))) {
                if (b == QMessageBox::Ok || b == QMessageBox::Yes) {
                    btn->setProperty("color", "blue");
                    btn->setText(QObject::tr("确认"));
                    okBtn = btn;
                } else {
                    btn->setProperty("color", "gray");
                    btn->setText(QObject::tr("返回"));
                    cancelBtn = btn;
                }
                btn->setStyle(new ButtonStyle(btn->style()));
                btn->setProperty("stdButton", int(b));
                btn->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            }
        }
    }

    KeyPressFilter *filter = new KeyPressFilter(&box, okBtn, cancelBtn);
    box.installEventFilter(filter);

    return static_cast<QMessageBox::StandardButton>(box.exec());
}

QMessageBox::StandardButton UiUtils::showMessageBoxQuestion(const QString &title, const QString &informativeText,
                                                            QMessageBox::StandardButtons buttons, bool switchLine /* = true*/)
{
    QMessageBox box;
    box.setWindowTitle(QObject::tr("选择"));
    box.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint);
    box.setTextFormat(Qt::RichText);
    box.setIcon(QMessageBox::Question);

    box.setText(QString("<span style='font-size:20px; font-weight:bold;'>%1</span>").arg(title));
    if (switchLine) {
        box.setInformativeText(QString("<span style='font-size:16px;'>%1</span>").arg(informativeText));
    } else {
        box.setInformativeText(QString("<div style='font-size:16px; white-space: pre;'>%1</div>").arg(informativeText));
    }

    box.setStandardButtons(buttons);

    QPushButton *okBtn = nullptr;
    QPushButton *cancelBtn = nullptr;
    for (QMessageBox::StandardButton b : {QMessageBox::Ok, QMessageBox::Cancel, QMessageBox::Yes, QMessageBox::No, QMessageBox::Close}) {
        if (buttons.testFlag(b)) {
            if (auto btn = qobject_cast<QPushButton *>(box.button(b))) {
                if (b == QMessageBox::Ok || b == QMessageBox::Yes) {
                    btn->setProperty("color", "blue");
                    btn->setText(QObject::tr("确认"));
                    okBtn = btn;
                } else {
                    btn->setProperty("color", "gray");
                    btn->setText(QObject::tr("返回"));
                    cancelBtn = btn;
                }
                btn->setStyle(new ButtonStyle(btn->style()));
                btn->setProperty("stdButton", int(b));
                btn->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            }
        }
    }

    KeyPressFilter *filter = new KeyPressFilter(&box, okBtn, cancelBtn);
    box.installEventFilter(filter);

    return static_cast<QMessageBox::StandardButton>(box.exec());
}

QMessageBox::StandardButton UiUtils::showMessageBoxWarning(const QString &title, const QString &informativeText, QMessageBox::StandardButtons buttons,
                                                           bool switchLine /* = true*/)
{
    QMessageBox box;
    box.setWindowTitle(QObject::tr("警告"));
    box.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint);
    box.setTextFormat(Qt::RichText);
    box.setIcon(QMessageBox::Warning);

    box.setText(QString("<span style='font-size:20px; font-weight:bold;'>%1</span>").arg(title));
    if (switchLine) {
        box.setInformativeText(QString("<span style='font-size:16px;'>%1</span>").arg(informativeText));
    } else {
        box.setInformativeText(QString("<div style='font-size:16px; white-space: pre;'>%1</div>").arg(informativeText));
    }

    box.setStandardButtons(buttons);

    QPushButton *okBtn = nullptr;
    QPushButton *cancelBtn = nullptr;
    for (QMessageBox::StandardButton b : {QMessageBox::Ok, QMessageBox::Cancel, QMessageBox::Yes, QMessageBox::No, QMessageBox::Close}) {
        if (buttons.testFlag(b)) {
            if (auto btn = qobject_cast<QPushButton *>(box.button(b))) {
                if (b == QMessageBox::Ok || b == QMessageBox::Yes) {
                    btn->setProperty("color", "blue");
                    btn->setText(QObject::tr("确认"));
                    okBtn = btn;
                } else {
                    btn->setProperty("color", "gray");
                    btn->setText(QObject::tr("返回"));
                    cancelBtn = btn;
                }
                btn->setStyle(new ButtonStyle(btn->style()));
                btn->setProperty("stdButton", int(b));
                btn->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            }
        }
    }

    KeyPressFilter *filter = new KeyPressFilter(&box, okBtn, cancelBtn);
    box.installEventFilter(filter);

    return static_cast<QMessageBox::StandardButton>(box.exec());
}

QMessageBox::StandardButton UiUtils::showMessageBoxError(const QString &title, const QString &informativeText, QMessageBox::StandardButtons buttons,
                                                         bool switchLine /* = true*/)
{
    QMessageBox box;
    box.setWindowTitle(QObject::tr("错误"));
    box.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint);
    box.setTextFormat(Qt::RichText);
    box.setIcon(QMessageBox::Critical);

    box.setText(QString("<span style='font-size:20px; font-weight:bold;'>%1</span>").arg(title));
    if (switchLine) {
        box.setInformativeText(QString("<span style='font-size:16px;'>%1</span>").arg(informativeText));
    } else {
        box.setInformativeText(QString("<div style='font-size:16px; white-space: pre;'>%1</div>").arg(informativeText));
    }

    box.setStandardButtons(buttons);

    QPushButton *okBtn = nullptr;
    QPushButton *cancelBtn = nullptr;
    for (QMessageBox::StandardButton b : {QMessageBox::Ok, QMessageBox::Cancel, QMessageBox::Yes, QMessageBox::No, QMessageBox::Close}) {
        if (buttons.testFlag(b)) {
            if (auto btn = qobject_cast<QPushButton *>(box.button(b))) {
                if (b == QMessageBox::Ok || b == QMessageBox::Yes) {
                    btn->setProperty("color", "blue");
                    btn->setText(QObject::tr("确认"));
                    okBtn = btn;
                } else {
                    btn->setProperty("color", "gray");
                    btn->setText(QObject::tr("返回"));
                    cancelBtn = btn;
                }
                btn->setStyle(new ButtonStyle(btn->style()));
                btn->setProperty("stdButton", int(b));
                btn->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            }
        }
    }

    KeyPressFilter *filter = new KeyPressFilter(&box, okBtn, cancelBtn);
    box.installEventFilter(filter);

    return static_cast<QMessageBox::StandardButton>(box.exec());
}

// --------------------------- ButtonStyle ----------------------

ButtonStyle::ButtonStyle(QStyle *style)
    : QProxyStyle(style)
{}

ButtonStyle::~ButtonStyle() {}

void ButtonStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == CE_PushButtonBevel) {
        if (const QPushButton *btn = qobject_cast<const QPushButton *>(widget)) {
            QString btnColor = btn->property("color").toString();
            if (btnColor == "blue" || btnColor == "gray") {
                painter->save();
                painter->setRenderHint(QPainter::Antialiasing, true);

                QRect r = option->rect.adjusted(1, 1, -1, -1);
                QColor bg;

                if (btnColor == "blue") {
                    bg = QColor("#007bff");
                } else if (btnColor == "gray") {
                    bg = QColor("#cdcdcd");
                }

                painter->setBrush(bg);
                painter->setPen(Qt::NoPen);
                painter->drawRoundedRect(r, 4, 4);
                painter->restore();

                return;
            }
        }
    } else if (element == CE_PushButtonLabel) {
        if (const QPushButton *btn = qobject_cast<const QPushButton *>(widget)) {
            QString btnColor = btn->property("color").toString();
            QStyleOptionButton opt(*qstyleoption_cast<const QStyleOptionButton *>(option));

            if (btnColor == "blue") {
                opt.palette.setColor(QPalette::ButtonText, QColor("#ffffff"));
            } else if (btnColor == "gray") {
                opt.palette.setColor(QPalette::ButtonText, QColor("#000000"));
            }

            QProxyStyle::drawControl(element, &opt, painter, widget);
            return;
        }
    }

    QProxyStyle::drawControl(element, option, painter, widget);
}

int ButtonStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    return QProxyStyle::pixelMetric(metric, option, widget);
}

// --------------------------- KeyPressFilter ----------------------

bool KeyPressFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        auto keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_S && okBtn) {
            box->done(QMessageBox::StandardButton(okBtn->property("stdButton").toInt()));
            return true;
        } else if (keyEvent->key() == Qt::Key_X && cancelBtn) {
            box->done(QMessageBox::StandardButton(cancelBtn->property("stdButton").toInt()));
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}
