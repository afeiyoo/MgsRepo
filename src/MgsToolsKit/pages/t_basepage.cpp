#include "t_basepage.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "ElaText.h"
#include "ElaTheme.h"

T_BasePage::T_BasePage(QWidget *parent)
    : ElaScrollPage{parent}
{
    connect(eTheme, &ElaTheme::themeModeChanged, this, [=]() {
        if (!parent) {
            update();
        }
    });
    setContentsMargins(20, 5, 0, 0);
}

T_BasePage::~T_BasePage() {}

void T_BasePage::createCustomWidget(QString desText)
{
    QWidget *customWidget = new QWidget(this);

    ElaText *descText = new ElaText(this);
    descText->setText(desText);
    descText->setTextPixelSize(12);

    QVBoxLayout *topLayout = new QVBoxLayout(customWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->addSpacing(2);
    topLayout->addWidget(descText);
    topLayout->addSpacing(2);
    setCustomWidget(customWidget);
}
