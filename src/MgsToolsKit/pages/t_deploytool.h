#pragma once

#include "t_basepage.h"

class ElaLineEdit;
class ElaPlainTextEdit;
class ElaPushButton;
class ElaText;
class StepperWidget;

class T_DeployTool : public T_BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit T_DeployTool(QWidget *parent = nullptr);
    ~T_DeployTool() override;

public slots:
    // 供部署业务层回写当前执行进度，stepIndex 对应界面中从 0 开始的步骤序号。
    void setCurrentDeploymentStep(int stepIndex, const QString &message = QString());
    void setDeploymentStepFailed(int stepIndex, const QString &errorMessage);
    void setDeploymentFinished(bool success, const QString &message = QString());

signals:
    void deploymentRequested(const QString &excelPath,
                             const QString &laneSoftwarePath,
                             const QString &syncScriptPath,
                             const QString &reportPath);
    void deploymentCancelRequested();

private:
    // 页面初始化
    void initContent();
    void initDeploymentSteps();
    void selectExcelFile();
    void selectLaneSoftwarePath();
    void selectSyncScript();
    void selectReportPath();
    void startDeployment();
    void resetDeployment();
    void updateStartButtonState();
    void setInputControlsEnabled(bool enabled);
    void appendLog(const QString &message);

private:
    ElaLineEdit *m_excelPathEdit = nullptr;
    ElaLineEdit *m_laneSoftwarePathEdit = nullptr;
    ElaLineEdit *m_syncScriptPathEdit = nullptr;
    ElaLineEdit *m_reportPathEdit = nullptr;
    ElaText *m_inputStatusText = nullptr;
    StepperWidget *m_stepperWidget = nullptr;
    ElaPlainTextEdit *m_logEdit = nullptr;
    ElaPushButton *m_excelBrowseButton = nullptr;
    ElaPushButton *m_laneSoftwareBrowseButton = nullptr;
    ElaPushButton *m_syncScriptBrowseButton = nullptr;
    ElaPushButton *m_reportBrowseButton = nullptr;
    ElaPushButton *m_startButton = nullptr;
    ElaPushButton *m_cancelButton = nullptr;
    ElaPushButton *m_resetButton = nullptr;
    ElaPushButton *m_clearLogButton = nullptr;
    bool m_isDeploying = false;
};
