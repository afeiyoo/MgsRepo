#pragma once

#include <QColor>
#include <QWidget>
#include <QVector>

class QFontMetrics;

class StepperWidget final : public QWidget
{
    Q_OBJECT

public:
    enum class Orientation
    {
        Horizontal,
        Vertical
    };

    enum class StepState
    {
        Pending,
        Current,
        Completed,
        Error
    };
    Q_ENUM(Orientation)
    Q_ENUM(StepState)

    // 一个 Step 对应流程里的一个节点：state 决定颜色，enabled 决定能否点击跳转。
    struct Step
    {
        QString title;
        QString description;
        StepState state = StepState::Pending;
        bool enabled = true;
        QString iconText;
    };

    explicit StepperWidget(QWidget *parent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    QVector<Step> steps() const;
    int currentIndex() const;
    Orientation orientation() const;
    bool interactive() const;
    bool showStepIcons() const;

    void setSteps(const QVector<Step> &steps);
    void setCurrentIndex(int index);
    void setOrientation(Orientation orientation);
    void setInteractive(bool enabled);
    void setShowStepIcons(bool enabled);
    void setStepState(int index, StepState state);
    void setStep(int index, const Step &step);
    // 这些动态编辑接口方便把控件接入运行期流程配置或向导页面。
    void appendStep(const Step &step);
    void insertStep(int index, const Step &step);
    void removeStep(int index);
    void clearSteps();
    void setAccentColor(const QColor &color);

signals:
    void currentIndexChanged(int index);
    void stepClicked(int index);
    void stepStateChanged(int index, StepState state);
    void stepAdded(int index);
    void stepRemoved(int index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QVector<QPointF> nodeCenters(const QRectF &contentRect) const;
    QRectF contentRect() const;
    int hitTest(const QPointF &point) const;
    QColor stateColor(StepState state) const;
    QColor lineColorForSegment(int leftIndex) const;
    QString nodeText(int index) const;
    QString elidedText(const QFontMetrics &metrics, const QString &text, int width) const;
    // 描述文字可能很长，统一在这里做多行省略，避免绘制阶段到处散落截断逻辑。
    QString elidedDescription(const QFontMetrics &metrics, const QString &text, int width, int maxLines) const;
    void updateHover(int index);

    QVector<Step> m_steps;
    int m_currentIndex = 0;
    int m_hoverIndex = -1;
    Orientation m_orientation = Orientation::Horizontal;
    bool m_interactive = true;
    bool m_showStepIcons = true;
    QColor m_accentColor = QColor("#2E7DD1");
};
