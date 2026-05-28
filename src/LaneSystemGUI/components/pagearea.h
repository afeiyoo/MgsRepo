#pragma once

#include <QWidget>

class PageArea : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int borderRadius READ getBorderRadius WRITE setBorderRadius NOTIFY borderRadiusChanged)
    Q_PROPERTY(QColor backgroundColor READ getBackgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
public:
    explicit PageArea(QWidget *parent = nullptr);
    virtual ~PageArea() override;

    int getBorderRadius() const;
    void setBorderRadius(int newBorderRadius);

    QColor getBackgroundColor() const;
    void setBackgroundColor(const QColor &newBackgroundColor);

    void setIsUseBorder(bool isUseBorder);

signals:
    void borderRadiusChanged();

    void backgroundColorChanged();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    // 圆角
    int m_borderRadius;
    // 背景颜色
    QColor m_backgroundColor;
    // 是否设置边框
    bool m_isUseBorder = false;
};
