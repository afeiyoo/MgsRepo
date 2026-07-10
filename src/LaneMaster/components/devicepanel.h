#pragma once

#include "components/pagearea.h"

class ElaIconButton;
class QHBoxLayout;
class DevicePanel : public PageArea
{
    Q_OBJECT
public:
    explicit DevicePanel(QWidget *parent = nullptr);
    ~DevicePanel() override;

    void setDeviceList(const QList<uint> &devices);

    void changeDevStatus(uint dev, uint status);

private:
    void initUi();
    QString iconPath(uint dev, uint status) const;

private:
    QList<uint> m_devices;
    QHBoxLayout *m_iconLayout = nullptr;
    // 设备与图标的映射关系
    QHash<uint, ElaIconButton *> m_devMap;
};
