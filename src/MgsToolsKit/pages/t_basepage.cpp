#include "t_basepage.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "ElaMenu.h"
#include "ElaText.h"
#include "ElaTheme.h"
#include "ElaToolButton.h"

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
    ElaText *subTitleText = new ElaText(this);
    subTitleText->setText("https://www.bing.com");
    subTitleText->setTextInteractionFlags(Qt::TextSelectableByMouse);
    subTitleText->setTextPixelSize(11);

    ElaText *descText = new ElaText(this);
    descText->setText(desText);
    descText->setTextPixelSize(13);

    QVBoxLayout *topLayout = new QVBoxLayout(customWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->addWidget(subTitleText);
    topLayout->addSpacing(5);
    topLayout->addWidget(descText);
    topLayout->addSpacing(2);
    setCustomWidget(customWidget);
}
