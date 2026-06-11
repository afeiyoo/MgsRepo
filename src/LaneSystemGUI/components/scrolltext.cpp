#include "scrolltext.h"

#include <QFontMetricsF>
#include <QPainter>
#include <QResizeEvent>
#include <QTimer>
#include <QTimerEvent>

ScrollText::ScrollText(QWidget *parent)
    : ElaText(parent)
{
    setIsWrapAnywhere(false);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

    // 默认值初始化
    m_scrollOffset = 0;
    m_direction = -1;
    m_paused = true;
    m_scrollTimer = 0;
    m_pauseTimer = 0;
    m_textWidth = 0;
    m_labelWidth = 0;
}

ScrollText::~ScrollText()
{
    stopScroll();
}

void ScrollText::setScrollText(const QString &text)
{
    if (text == this->text())
        return;

    ElaText::setText(text); // 设置文本
    updateScrollState();    // 重新计算滚动状态

    if (m_needsScroll)
        beginPause(); // 初始暂停再滚动
    else
        update(); // 立即重绘
}

void ScrollText::updateScrollState()
{
    m_scrollText = text();
    QFontMetricsF fm(font());
    m_textWidth = fm.horizontalAdvance(m_scrollText);
    m_labelWidth = width();
    m_needsScroll = (m_textWidth > m_labelWidth);

    m_scrollOffset = 0;
    m_direction = -1;
    m_paused = true;

    if (!m_needsScroll)
        stopScroll();
}

void ScrollText::beginScrollWithDelay()
{
    if (m_scrollTimer == 0)
        m_scrollTimer = startTimer(SCROLL_INTERVAL_MS);
    m_paused = false;
}

void ScrollText::beginPause()
{
    if (m_pauseTimer > 0)
        killTimer(m_pauseTimer);
    m_pauseTimer = startTimer(PAUSE_DURATION_MS);
    m_paused = true;
}

void ScrollText::stopScroll()
{
    if (m_scrollTimer > 0) {
        killTimer(m_scrollTimer);
        m_scrollTimer = 0;
    }
    if (m_pauseTimer > 0) {
        killTimer(m_pauseTimer);
        m_pauseTimer = 0;
    }
}

void ScrollText::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_pauseTimer) {
        killTimer(m_pauseTimer);
        m_pauseTimer = 0;
        beginScrollWithDelay();
        return;
    }

    if (event->timerId() != m_scrollTimer || m_paused)
        return;

    if (!m_needsScroll) {
        update();
        return;
    }

    if (m_direction == -1 && m_scrollOffset <= -(m_textWidth - m_labelWidth)) {
        m_scrollOffset = -(m_textWidth - m_labelWidth);
        m_direction = 1;
        beginPause();
        return;
    } else if (m_direction == 1 && m_scrollOffset >= 0) {
        m_scrollOffset = 0;
        m_direction = -1;
        beginPause();
        return;
    }

    m_scrollOffset += m_direction * SCROLL_STEP;
    update();
}

void ScrollText::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(palette().color(QPalette::Text));

    QRectF textRect = rect();
    textRect.setLeft(textRect.left() + m_scrollOffset);
    textRect.setWidth(m_textWidth);

    painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, m_scrollText);
}

void ScrollText::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateScrollState();

    if (m_needsScroll)
        beginPause();
}
