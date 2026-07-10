#pragma once

#include "ElaWidgetTools/ElaText.h"

class ScrollText : public ElaText
{
    Q_OBJECT
public:
    explicit ScrollText(QWidget *parent = nullptr);
    ~ScrollText() override;

    void setScrollText(const QString &text);

protected:
    void timerEvent(QTimerEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    // 计算是否需要滚动内容
    void updateScrollState();
    void beginScrollWithDelay();
    void beginPause();
    void stopScroll();

private:
    // 定时器
    int m_scrollTimer;
    int m_pauseTimer;

    // 当前文本的横向偏移量（左负右正）
    qreal m_scrollOffset;
    // 当前滚动方向：-1 向左，+1 向右
    int m_direction;

    // 文本内容的真实宽度（像素）
    int m_textWidth;
    // 控件的可视宽度
    int m_labelWidth;

    // 当前绘制的文本
    QString m_scrollText;

    // 是否需要滚动（初始时默认为否）
    bool m_needsScroll = false;
    // 当前是否为暂停状态（由m_pauseTImer控制）
    bool m_paused;

    // 每帧偏移像素
    static constexpr qreal SCROLL_STEP = 0.8;
    // 每16ms刷新一次（约60FPS）
    static constexpr int SCROLL_INTERVAL_MS = 16;
    // 每次到边界时的停顿时间（500ms）
    static constexpr int PAUSE_DURATION_MS = 800;
};
