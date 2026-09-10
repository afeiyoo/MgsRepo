#pragma once

#include "ElaWindow.h"
#include <QPersistentModelIndex>

class QTreeView;

class ElaContentDialog;
class ElaText;

class T_DeskPrinter;
class T_InfoBoard;
class T_SmartController;
class T_CardRobot;
class T_MobilePlusTerminal;
class T_DeployTool;
class T_VehRecognizer;

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
#ifndef Q_OS_LINUX
    bool eventFilter(QObject *watched, QEvent *event) override;
    QTreeView *m_navigationView = nullptr;
    QPersistentModelIndex m_deployToolIndex;
#endif
    ElaContentDialog *m_closeDialog = nullptr;
    ElaText *m_statusText = nullptr;

    T_DeskPrinter *m_deskPrinterPage = nullptr;
    T_InfoBoard *m_infoBoardPage = nullptr;
    T_SmartController *m_smartControllerPage = nullptr;
    T_CardRobot *m_cardRobotPage = nullptr;
    T_MobilePlusTerminal *m_mobilePlusTerminalPage = nullptr;
    T_DeployTool *m_deployToolPage = nullptr;
    T_VehRecognizer *m_vehRecognizerPage = nullptr;
};
