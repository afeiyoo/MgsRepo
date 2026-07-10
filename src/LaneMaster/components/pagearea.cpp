#include "pagearea.h"

#include <QPainter>

#include "global/uiconst.h"

PageArea::PageArea(QWidget *parent)
    : QWidget(parent)
{
    // 允许透明绘制
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);

    m_borderRadius = 6;
    m_backgroundColor = QColor(Color::CUSTOM_AREA_BG);
}

PageArea::~PageArea() {}

void PageArea::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect(1, 1, width() - 1, height() - 1);
    if (m_isUseBorder) {
        painter.setPen(QColor("#D9DEE5"));
    } else {
        painter.setPen(Qt::NoPen);
    }
    // 设置背景刷（根据透明度判断）
    if (m_backgroundColor.alpha() > 0) {
        painter.setBrush(m_backgroundColor);
    } else {
        painter.setBrush(Qt::NoBrush);
    }
    painter.drawRoundedRect(rect, m_borderRadius, m_borderRadius);
    painter.restore();
}

QColor PageArea::getBackgroundColor() const
{
    return m_backgroundColor;
}

void PageArea::setBackgroundColor(const QColor &newBackgroundColor)
{
    if (m_backgroundColor == newBackgroundColor)
        return;
    m_backgroundColor = newBackgroundColor;
    emit backgroundColorChanged();
}

void PageArea::setIsUseBorder(bool isUseBorder)
{
    m_isUseBorder = isUseBorder;
}

int PageArea::getBorderRadius() const
{
    return m_borderRadius;
}

void PageArea::setBorderRadius(int newBorderRadius)
{
    if (m_borderRadius == newBorderRadius)
        return;
    m_borderRadius = newBorderRadius;
    emit borderRadiusChanged();
}
