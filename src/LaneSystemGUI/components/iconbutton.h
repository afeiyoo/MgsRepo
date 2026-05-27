#pragma once

#include <QColor>
#include <QWidget>

#include "ElaIconButton.h"
#include "ElaText.h"

class IconButton : public QWidget
{
    Q_OBJECT
public:
    explicit IconButton(ElaIconType::IconName awesome, int pixelSize, QWidget *parent = nullptr);
    explicit IconButton(ElaIconType::IconName awesome, QString text, int pixelSize, QWidget *parent = nullptr);

    void setText(const QString &text);
    void setColor(const QString &color);

private:
    ElaIconButton *m_icon = nullptr;
    ElaText *m_text = nullptr;
};
