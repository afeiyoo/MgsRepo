#pragma once

#include "lanesystemgui_global.h"

#include "ElaWidgetTools/ElaWindow.h"

class BasePage;
class LANESYSTEMGUI_EXPORT MainWindow : public ElaWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void initMtcIn();

private:
    // 主页面
    BasePage *m_mainPage = nullptr;
};
