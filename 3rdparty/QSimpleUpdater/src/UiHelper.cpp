#include "UiHelper.h"

#include <QPainter>
#include <QPushButton>
#include <QStyleOption>

UiHelper::UiHelper(QObject *parent)
    : QObject{parent}
{}

QMessageBox::StandardButton UiHelper::showMessageBoxInfo(const QString &title, const QString &informativeText, QMessageBox::StandardButtons buttons)
{
    QMessageBox box;
    box.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
    box.setTextFormat(Qt::RichText);
    box.setIcon(QMessageBox::Information);
    box.setText(QString("<span style='font-size:20px; font-weight:bold;'>%1</span>").arg(title));
    box.setInformativeText(QString("<span style='font-size:16px;'>%1</span>").arg(informativeText));

    box.setStandardButtons(buttons);

    QPushButton *okBtn = nullptr;
    QPushButton *cancelBtn = nullptr;

    for (QMessageBox::StandardButton b : {QMessageBox::Ok, QMessageBox::Cancel, QMessageBox::Yes, QMessageBox::No, QMessageBox::Close}) {
        if (buttons.testFlag(b)) {
            if (auto btn = qobject_cast<QPushButton *>(box.button(b))) {
                if (b == QMessageBox::Ok || b == QMessageBox::Yes) {
                    btn->setProperty("color", "blue");
                    btn->setText(QObject::tr("确定"));
                    okBtn = btn;
                } else {
                    btn->setProperty("color", "gray");
                    btn->setText(QObject::tr("取消"));
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

QMessageBox::StandardButton UiHelper::showMessageBoxQuestion(const QString &title,
                                                             const QString &informativeText,
                                                             QMessageBox::StandardButtons buttons)
{
    QMessageBox box;
    box.setAttribute(Qt::WA_TransparentForMouseEvents, true);
    box.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
    box.setTextFormat(Qt::RichText);
    box.setIcon(QMessageBox::Question);
    box.setText(QString("<span style='font-size:20px; font-weight:bold;'>%1</span>").arg(title));
    box.setInformativeText(QString("<span style='font-size:16px;'>%1</span>").arg(informativeText));

    box.setStandardButtons(buttons);

    QPushButton *okBtn = nullptr;
    QPushButton *cancelBtn = nullptr;

    for (QMessageBox::StandardButton b : {QMessageBox::Ok, QMessageBox::Cancel, QMessageBox::Yes, QMessageBox::No, QMessageBox::Close}) {
        if (buttons.testFlag(b)) {
            if (auto btn = qobject_cast<QPushButton *>(box.button(b))) {
                if (b == QMessageBox::Ok || b == QMessageBox::Yes) {
                    btn->setProperty("color", "blue");
                    btn->setText(QObject::tr("确定"));
                    okBtn = btn;
                } else {
                    btn->setProperty("color", "gray");
                    btn->setText(QObject::tr("取消"));
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
                opt.palette.setColor(QPalette::ButtonText, Qt::white);
            } else if (btnColor == "gray") {
                opt.palette.setColor(QPalette::ButtonText, Qt::black);
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
