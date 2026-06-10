#pragma once

#include "ElaWidgetTools/ElaWindow.h"

class EtcPage;
class MtcInPage;
class MtcOutPage;

class MainWindow : public ElaWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void initMtcIn();
    void initMtcOut(bool isSptInfoShow = false);
    void initEtc();

    MtcInPage *mtcInPage() const;
    MtcOutPage *mtcOutPage() const;
    EtcPage *etcPage() const;

private:
    MtcInPage *m_mainPageIn = nullptr;
    MtcOutPage *m_mainPageOut = nullptr;
    EtcPage *m_mainPageEtc = nullptr;
};
