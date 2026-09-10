#include "stepperwidget.h"

#include <QCursor>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QStringList>
#include <QToolTip>
#include <QtGlobal>

#include <algorithm>

StepperWidget::StepperWidget(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

QSize StepperWidget::sizeHint() const
{
    const int count = std::max(1, static_cast<int>(m_steps.size()));
    return m_orientation == Orientation::Horizontal ? QSize(std::max(750, count * 150), 220) : QSize(560, std::max(360, count * 96));
}

QSize StepperWidget::minimumSizeHint() const
{
    return m_orientation == Orientation::Horizontal ? QSize(420, 170) : QSize(340, 260);
}

QVector<StepperWidget::Step> StepperWidget::steps() const
{
    return m_steps;
}

int StepperWidget::currentIndex() const
{
    return m_currentIndex;
}

StepperWidget::Orientation StepperWidget::orientation() const
{
    return m_orientation;
}

bool StepperWidget::interactive() const
{
    return m_interactive;
}

bool StepperWidget::showStepIcons() const
{
    return m_showStepIcons;
}

void StepperWidget::setSteps(const QVector<Step> &steps)
{
    m_steps = steps;
    if (m_steps.isEmpty()) {
        m_currentIndex = -1;
    } else {
        m_currentIndex = qBound(0, m_currentIndex, static_cast<int>(m_steps.size()) - 1);
        setCurrentIndex(m_currentIndex);
    }
    updateGeometry();
    update();
}

void StepperWidget::setCurrentIndex(int index)
{
    if (m_steps.isEmpty()) {
        index = -1;
    } else {
        index = qBound(0, index, static_cast<int>(m_steps.size()) - 1);
    }

    const bool changed = index != m_currentIndex;
    m_currentIndex = index;

    for (int i = 0; i < m_steps.size(); ++i) {
        if (m_steps[i].state == StepState::Error) {
            continue;
        }
        if (i < m_currentIndex) {
            m_steps[i].state = StepState::Completed;
        } else if (i == m_currentIndex) {
            m_steps[i].state = StepState::Current;
        } else {
            m_steps[i].state = StepState::Pending;
        }
    }

    if (changed) {
        emit currentIndexChanged(m_currentIndex);
    }
    update();
}

void StepperWidget::setOrientation(Orientation orientation)
{
    if (m_orientation == orientation) {
        return;
    }
    m_orientation = orientation;
    updateGeometry();
    update();
}

void StepperWidget::setInteractive(bool enabled)
{
    if (m_interactive == enabled) {
        return;
    }
    m_interactive = enabled;
    unsetCursor();
    update();
}

void StepperWidget::setShowStepIcons(bool enabled)
{
    if (m_showStepIcons == enabled) {
        return;
    }
    m_showStepIcons = enabled;
    update();
}

void StepperWidget::setStepState(int index, StepState state)
{
    if (index < 0 || index >= m_steps.size()) {
        return;
    }
    if (m_steps[index].state == state) {
        return;
    }
    m_steps[index].state = state;
    emit stepStateChanged(index, state);
    update();
}

void StepperWidget::setStep(int index, const Step &step)
{
    if (index < 0 || index >= m_steps.size()) {
        return;
    }
    m_steps[index] = step;
    updateGeometry();
    update();
}

void StepperWidget::appendStep(const Step &step)
{
    insertStep(m_steps.size(), step);
}

void StepperWidget::insertStep(int index, const Step &step)
{
    index = qBound(0, index, static_cast<int>(m_steps.size()));
    m_steps.insert(index, step);
    if (m_currentIndex < 0) {
        m_currentIndex = 0;
    } else if (index <= m_currentIndex) {
        ++m_currentIndex;
    }
    setCurrentIndex(m_currentIndex);
    emit stepAdded(index);
    updateGeometry();
    update();
}

void StepperWidget::removeStep(int index)
{
    if (index < 0 || index >= m_steps.size()) {
        return;
    }
    m_steps.removeAt(index);
    if (m_steps.isEmpty()) {
        m_currentIndex = -1;
    } else if (index < m_currentIndex) {
        --m_currentIndex;
    } else if (m_currentIndex >= m_steps.size()) {
        m_currentIndex = m_steps.size() - 1;
    }
    setCurrentIndex(m_currentIndex);
    emit stepRemoved(index);
    updateGeometry();
    update();
}

void StepperWidget::clearSteps()
{
    if (m_steps.isEmpty()) {
        return;
    }
    m_steps.clear();
    m_currentIndex = -1;
    m_hoverIndex = -1;
    updateGeometry();
    update();
}

void StepperWidget::setAccentColor(const QColor &color)
{
    if (!color.isValid() || m_accentColor == color) {
        return;
    }
    m_accentColor = color;
    update();
}

void StepperWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    const QRectF area = contentRect();
    const QVector<QPointF> centers = nodeCenters(area);
    if (centers.isEmpty()) {
        painter.setPen(QColor("#687782"));
        painter.drawText(rect(), Qt::AlignCenter, QStringLiteral("没有步骤数据"));
        return;
    }

    // 先画连接线，再画节点。这样节点永远盖在线条上方，状态更清楚。
    for (int i = 0; i + 1 < centers.size(); ++i) {
        painter.setPen(QPen(lineColorForSegment(i), 4, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(centers[i], centers[i + 1]);
    }

    QFont titleFont = font();
    titleFont.setBold(true);
    QFont descFont = font();
    descFont.setPointSize(std::max(8, descFont.pointSize() - 1));
    const QFontMetrics titleMetrics(titleFont);
    const QFontMetrics descMetrics(descFont);

    for (int i = 0; i < centers.size(); ++i) {
        const Step &step = m_steps[i];
        const QPointF center = centers[i];
        const QColor color = step.enabled ? stateColor(step.state) : QColor("#C6CDD3");
        const bool active = i == m_currentIndex || step.state == StepState::Current;
        const bool hover = i == m_hoverIndex;
        const qreal radius = active || hover ? 18.0 : 16.0;

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, hover ? 34 : 22));
        painter.drawEllipse(center + QPointF(1.5, 2.0), radius, radius);

        painter.setBrush(color);
        painter.setPen(QPen(active ? color.darker(120) : QColor("#ffffff"), active ? 3.0 : 2.0));
        painter.drawEllipse(center, radius, radius);

        painter.setPen(Qt::white);
        painter.setFont(titleFont);
        painter.drawText(QRectF(center.x() - radius, center.y() - radius, radius * 2, radius * 2), Qt::AlignCenter, nodeText(i));

        QRectF textRect;
        if (m_orientation == Orientation::Horizontal) {
            // 文字宽度不能超过相邻节点间距，窄窗口下保留间隔并换行/省略。
            const qreal nodeSpacing = centers.size() > 1 ? area.width() / (centers.size() - 1) : width();
            const qreal textWidth = qMax(0.0, qMin(164.0, nodeSpacing - 12.0));
            textRect = QRectF(center.x() - textWidth / 2.0, center.y() + 28.0, textWidth, 70.0);
            painter.save();
            painter.setClipRect(textRect, Qt::IntersectClip);
            painter.setFont(titleFont);
            painter.setPen(step.enabled ? QColor("#2d3740") : QColor("#8A96A0"));
            painter.drawText(textRect.adjusted(0, 0, 0, -38), Qt::AlignTop | Qt::AlignHCenter,
                             elidedText(titleMetrics, step.title, static_cast<int>(textRect.width())));
            painter.setFont(descFont);
            painter.setPen(step.enabled ? QColor("#66727c") : QColor("#9AA5AE"));
            painter.drawText(textRect.adjusted(0, 24, 0, 0), Qt::AlignTop | Qt::AlignHCenter,
                             elidedDescription(descMetrics, step.description, static_cast<int>(textRect.width()), 2));
            painter.restore();
        } else {
            textRect = QRectF(center.x() + 34.0, center.y() - 24.0, area.right() - center.x() - 48.0, 64.0);
            painter.setFont(titleFont);
            painter.setPen(step.enabled ? QColor("#2d3740") : QColor("#8A96A0"));
            painter.drawText(textRect.adjusted(0, 0, 0, -34), Qt::AlignLeft | Qt::AlignTop,
                             elidedText(titleMetrics, step.title, static_cast<int>(textRect.width())));
            painter.setFont(descFont);
            painter.setPen(step.enabled ? QColor("#66727c") : QColor("#9AA5AE"));
            painter.drawText(textRect.adjusted(0, 23, 0, 0), Qt::AlignLeft | Qt::AlignTop,
                             elidedDescription(descMetrics, step.description, static_cast<int>(textRect.width()), 2));
        }
    }
}

void StepperWidget::mouseMoveEvent(QMouseEvent *event)
{
    updateHover(hitTest(event->localPos()));
}

void StepperWidget::mousePressEvent(QMouseEvent *event)
{
    if (!m_interactive || event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }

    const int index = hitTest(event->localPos());
    if (index >= 0 && index < m_steps.size() && m_steps[index].enabled) {
        setCurrentIndex(index);
        emit stepClicked(index);
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void StepperWidget::leaveEvent(QEvent *)
{
    updateHover(-1);
    QToolTip::hideText();
}

QVector<QPointF> StepperWidget::nodeCenters(const QRectF &content) const
{
    QVector<QPointF> result;
    if (m_steps.isEmpty()) {
        return result;
    }

    if (m_steps.size() == 1) {
        result.push_back(content.center());
        return result;
    }

    for (int i = 0; i < m_steps.size(); ++i) {
        const qreal ratio = static_cast<qreal>(i) / static_cast<qreal>(m_steps.size() - 1);
        if (m_orientation == Orientation::Horizontal) {
            result.push_back(QPointF(content.left() + ratio * content.width(), content.top() + 16.0));
        } else {
            result.push_back(QPointF(content.left() + 18.0, content.top() + ratio * content.height()));
        }
    }
    return result;
}

QRectF StepperWidget::contentRect() const
{
    if (m_orientation == Orientation::Horizontal) {
        return rect().adjusted(82, 22, -82, -30);
    }
    return rect().adjusted(28, 42, -28, -42);
}

int StepperWidget::hitTest(const QPointF &point) const
{
    const QVector<QPointF> centers = nodeCenters(contentRect());
    for (int i = 0; i < centers.size(); ++i) {
        const QPointF delta = point - centers[i];
        if (delta.x() * delta.x() + delta.y() * delta.y() <= 24.0 * 24.0) {
            return i;
        }
    }
    return -1;
}

QColor StepperWidget::stateColor(StepState state) const
{
    switch (state) {
    case StepState::Completed:
        return QColor("#22A06B");
    case StepState::Current:
        return m_accentColor;
    case StepState::Error:
        return QColor("#D64545");
    case StepState::Pending:
    default:
        return QColor("#A7B3BD");
    }
}

QColor StepperWidget::lineColorForSegment(int leftIndex) const
{
    if (leftIndex < 0 || leftIndex + 1 >= m_steps.size()) {
        return QColor("#D9E0E5");
    }
    if (!m_steps[leftIndex].enabled || !m_steps[leftIndex + 1].enabled) {
        return QColor("#E3E8EC");
    }
    if (m_steps[leftIndex].state == StepState::Error || m_steps[leftIndex + 1].state == StepState::Error) {
        return QColor("#E7B8B8");
    }
    if (m_steps[leftIndex].state == StepState::Completed && m_steps[leftIndex + 1].state != StepState::Pending) {
        return QColor("#8BD2B2");
    }
    if (leftIndex < m_currentIndex) {
        return m_accentColor.lighter(145);
    }
    return QColor("#D9E0E5");
}

QString StepperWidget::nodeText(int index) const
{
    if (index < 0 || index >= m_steps.size()) {
        return QString();
    }
    if (m_showStepIcons && !m_steps[index].iconText.isEmpty()) {
        return m_steps[index].iconText.left(2).toUpper();
    }
    if (m_steps[index].state == StepState::Error) {
        return QStringLiteral("!");
    }
    return QString::number(index + 1);
}

QString StepperWidget::elidedText(const QFontMetrics &metrics, const QString &text, int width) const
{
    return metrics.elidedText(text.simplified(), Qt::ElideRight, std::max(12, width));
}

QString StepperWidget::elidedDescription(const QFontMetrics &metrics, const QString &text, int width, int maxLines) const
{
    QString remaining = text.simplified();
    if (remaining.isEmpty() || maxLines <= 0) {
        return QString();
    }

    QStringList lines;
    const int lineWidth = std::max(12, width);
    for (int line = 0; line < maxLines && !remaining.isEmpty(); ++line) {
        if (line == maxLines - 1) {
            lines.push_back(metrics.elidedText(remaining, Qt::ElideRight, lineWidth));
            break;
        }

        int lastFit = 0;
        for (int i = 1; i <= remaining.size(); ++i) {
            if (metrics.horizontalAdvance(remaining.left(i)) > lineWidth) {
                break;
            }
            lastFit = i;
        }

        if (lastFit <= 0) {
            lines.push_back(metrics.elidedText(remaining, Qt::ElideRight, lineWidth));
            break;
        }

        int breakAt = lastFit;
        for (int i = lastFit; i > 0; --i) {
            const QChar ch = remaining.at(i - 1);
            if (ch.isSpace() || QStringLiteral("，。,.；;、").contains(ch)) {
                breakAt = i;
                break;
            }
        }

        lines.push_back(remaining.left(breakAt).trimmed());
        remaining = remaining.mid(breakAt).trimmed();
    }
    return lines.join(QLatin1Char('\n'));
}

void StepperWidget::updateHover(int index)
{
    if (!m_interactive) {
        index = -1;
    }
    if (m_hoverIndex == index) {
        return;
    }
    m_hoverIndex = index;
    if (index >= 0 && index < m_steps.size()) {
        if (m_steps[index].enabled) {
            setCursor(Qt::PointingHandCursor);
        } else {
            unsetCursor();
        }
        QToolTip::showText(QCursor::pos(), m_steps[index].title + "\n" + m_steps[index].description, this);
    } else {
        unsetCursor();
    }
    update();
}
