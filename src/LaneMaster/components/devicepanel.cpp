#include "devicepanel.h"

#include "ElaWidgetTools/ElaIconButton.h"
#include "IPageController.h"

#include <QDebug>
#include <QHBoxLayout>

DevicePanel::DevicePanel(QWidget *parent)
    : PageArea{parent}
{
    initUi();
}

DevicePanel::~DevicePanel() {}

void DevicePanel::setDeviceList(const QList<uint> &devices)
{
    // 清除之前的图标（保留首尾的两个 Stretch）
    for (auto btn : m_devMap.values()) {
        delete btn;
    }
    m_devMap.clear();
    m_devices = devices;

    while (m_iconLayout->count() > 2) {
        QLayoutItem *item = m_iconLayout->takeAt(1);
        if (QWidget *w = item->widget())
            delete w;
        delete item;
    }

    for (uint dev : m_devices) {
        // 设备初始状态为 1
        QString path = iconPath(dev, 1);
        ElaIconButton *btn = new ElaIconButton(QPixmap(path), this);
        btn->setFixedSize(40, 40);
        btn->setAttribute(Qt::WA_TransparentForMouseEvents, true);

        // 保存映射
        m_devMap.insert(dev, btn);
        // 插入布局倒数第二位
        m_iconLayout->insertWidget(m_iconLayout->count() - 1, btn);
    }
}

void DevicePanel::changeDevStatus(uint dev, uint status)
{
    if (auto btn = m_devMap.value(dev, nullptr)) {
        QString path = iconPath(dev, status);
        QPixmap pix(path);
        btn->setPixmap(pix);
        btn->update();
    }
}

void DevicePanel::initUi()
{
    setBorderRadius(8);

    m_iconLayout = new QHBoxLayout(this);
    m_iconLayout->setContentsMargins(5, 5, 5, 5);
    m_iconLayout->setSpacing(10);
    m_iconLayout->addStretch();
    m_iconLayout->addStretch();
}

// 根据 dev 和 status 返回不同图标路径
QString DevicePanel::iconPath(uint dev, uint status) const
{
    QString path;
    switch (static_cast<IPageController::EM_DeviceIcon>(dev)) {
    case IPageController::CAPTURE: {
        if (status == 1 || status == 2)
            path = QString(":/static/images/cap_%1.png").arg(status);
    } break;
    case IPageController::CELLING_LAMP: {
        if (status == 1 || status == 2 || status == 3)
            path = QString(":/static/images/celling_lamp_%1.png").arg(status);
    } break;
    case IPageController::FIRST_COIL: {
        if (status == 1 || status == 2 || status == 3)
            path = QString(":/static/images/first_coil_%1.png").arg(status);
    } break;
    case IPageController::SECOND_COIL: {
        if (status == 1 || status == 2 || status == 3)
            path = QString(":/static/images/second_coil_%1.png").arg(status);
    } break;
    case IPageController::THIRD_COIL: {
        if (status == 1 || status == 2 || status == 3)
            path = QString(":/static/images/third_coil_%1.png").arg(status);
    } break;
    case IPageController::FOURTH_COIL: {
        if (status == 1 || status == 2 || status == 3)
            path = QString(":/static/images/fourth_coil_%1.png").arg(status);
    } break;
    case IPageController::CAP_COIL: {
        if (status == 1 || status == 2 || status == 3)
            path = QString(":/static/images/capcoil_%1.png").arg(status);
    } break;
    case IPageController::RAILLING_COIL: {
        if (status == 1 || status == 2 || status == 3)
            path = QString(":/static/images/railcoil_%1.png").arg(status);
    } break;
    case IPageController::PASSING_LAMP: {
        if (status == 1 || status == 2 || status == 3)
            path = QString(":/static/images/passing_lamp_%1.png").arg(status);
    } break;
    case IPageController::RAILING_MACHINE: {
        if (status == 1 || status == 2 || status == 3)
            path = QString(":/static/images/railing_machine_%1.png").arg(status);
    } break;
    case IPageController::RSU: {
        if (status == 1 || status == 2 || status == 3)
            path = QString(":/static/images/rsu_%1.png").arg(status);
    } break;
    case IPageController::WEIGHT: {
        if (status == 1 || status == 2)
            path = QString(":/static/images/weight_%1.png").arg(status);
    } break;
    default:
        return QString();
    }

    return path;
}
