#pragma once

#include "ElaWindow.h"

class ElaContentDialog;
class ElaText;
class ElaSuggestBox;

class T_DeskPrinter;
class T_InfoBoard;
class T_SmartController;

class MainWindow : public ElaWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void initWindow();
    void initEdgeLayout();
    void initContent();

private:
    ElaContentDialog *m_closeDialog = nullptr;
    ElaSuggestBox *m_windowSuggestBox = nullptr;
    ElaText *m_statusText = nullptr;

    T_DeskPrinter *m_deskPrinterPage = nullptr;
    T_InfoBoard *m_infoBoardPage = nullptr;
    T_SmartController *m_smartControllerPage = nullptr;
};
