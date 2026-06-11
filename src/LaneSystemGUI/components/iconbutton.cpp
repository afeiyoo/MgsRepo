#include "iconbutton.h"

#include <QDebug>
#include <QHBoxLayout>

IconButton::IconButton(ElaIconType::IconName awesome, int pixelSize, QFont::Weight weight, QWidget *parent)
    : QWidget(parent)
{
    m_text = new ElaText(this);
    m_text->setTextPixelSize(pixelSize);
    m_text->setContentsMargins(0, 0, 0, 0);
    QFont textFont = m_text->font();
    textFont.setWeight(weight);
    m_text->setFont(textFont);
    m_text->setIsWrapAnywhere(false);

    m_icon = new ElaIconButton(awesome, pixelSize, pixelSize, pixelSize, this);
    QFont iconFont = m_icon->font();
    iconFont.setWeight(weight);
    m_icon->setFont(iconFont);
    m_icon->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    layout->addWidget(m_icon);
    layout->addWidget(m_text);
    layout->addStretch();
}

IconButton::IconButton(ElaIconType::IconName awesome, QString text, int pixelSize, QFont::Weight weight, QWidget *parent)
    : IconButton(awesome, pixelSize, weight, parent)
{
    m_text->setText(text);
}

void IconButton::setText(const QString &text)
{
    m_text->setText(text);
}

void IconButton::setColor(const QString &color)
{
    m_icon->setLightIconColor(color);

    QString style = QString("color: %1;").arg(color);
    m_text->setStyleSheet(style);
}
