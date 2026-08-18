#pragma once

#include "ElaWidgetTools/ElaWindow.h"

class EtcPage;
class MtcInPage;
class MtcOutPage;
class BasePage;

class MainWindow : public ElaWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void initMtcIn();
    void initMtcOut(bool isSptInfoShow = false);
    void initEtc();

    BasePage *mtcInPage() const;
    BasePage *mtcOutPage() const;
    BasePage *etcPage() const;

private:
    MtcInPage *m_mainPageIn = nullptr;
    MtcOutPage *m_mainPageOut = nullptr;
    EtcPage *m_mainPageEtc = nullptr;
};
