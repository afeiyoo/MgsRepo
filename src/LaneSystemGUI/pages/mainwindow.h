#pragma once

#include "lanesystemgui_global.h"

#include "ElaWidgetTools/ElaWindow.h"

class MtcInPage;
class MtcOutPage;
class LANESYSTEMGUI_EXPORT MainWindow : public ElaWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void initMtcIn();

    void initMtcOut();

private:
    // 主页面
    MtcInPage *m_mainPageIn = nullptr;   // 混合入口
    MtcOutPage *m_mainPageOut = nullptr; // 混合出口
};
