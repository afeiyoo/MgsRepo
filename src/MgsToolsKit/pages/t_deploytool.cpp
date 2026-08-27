#include "t_deploytool.h"

#include <QDateTime>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "ElaLineEdit.h"
#include "ElaMessageBar.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "utils/widgets/stepperwidget.h"

T_DeployTool::T_DeployTool(QWidget *parent)
    : T_BasePage{parent}
{
    setWindowTitle("信创车道系统部署工具");
    createCustomWidget("加载车道信息采集表，并按流程完成车道数据库、配置文件、同步脚本和系统服务部署");
    initContent();
}

T_DeployTool::~T_DeployTool() {}

void T_DeployTool::initContent()
{
    m_excelPathEdit = new ElaLineEdit(this);
    m_excelPathEdit->setReadOnly(true);
    m_excelPathEdit->setPlaceholderText("请选择车道信息采集表（.xlsx）");
    m_excelBrowseButton = new ElaPushButton("打开文件", this);

    m_laneSoftwarePathEdit = new ElaLineEdit(this);
    m_laneSoftwarePathEdit->setReadOnly(true);
    m_laneSoftwarePathEdit->setPlaceholderText("请选择车道软件根目录");
    m_laneSoftwareBrowseButton = new ElaPushButton("选择目录", this);

    m_syncScriptPathEdit = new ElaLineEdit(this);
    m_syncScriptPathEdit->setReadOnly(true);
    m_syncScriptPathEdit->setPlaceholderText("请选择需要复制到 upload 目录的同步脚本");
    m_syncScriptBrowseButton = new ElaPushButton("选择脚本", this);

    m_reportPathEdit = new ElaLineEdit(this);
    m_reportPathEdit->setReadOnly(true);
    m_reportPathEdit->setPlaceholderText("请选择部署报告保存目录");
    m_reportBrowseButton = new ElaPushButton("选择目录", this);

    const QList<ElaLineEdit *> pathEdits = {
        m_excelPathEdit, m_laneSoftwarePathEdit, m_syncScriptPathEdit, m_reportPathEdit};
    for (ElaLineEdit *edit : pathEdits) {
        edit->setMinimumHeight(35);
        edit->setClearButtonEnabled(false);
    }

    ElaText *excelLabel = new ElaText("车道采集表", this);
    ElaText *softwareLabel = new ElaText("车道软件目录", this);
    ElaText *scriptLabel = new ElaText("同步脚本", this);
    ElaText *reportLabel = new ElaText("报告输出目录", this);
    const QList<ElaText *> inputLabels = {excelLabel, softwareLabel, scriptLabel, reportLabel};
    for (ElaText *label : inputLabels) {
        label->setTextPixelSize(14);
        label->setIsWrapAnywhere(false);
    }

    m_inputStatusText = new ElaText("请先准备全部部署输入", this);
    m_inputStatusText->setTextPixelSize(13);

    QGroupBox *inputGroup = new QGroupBox("部署输入", this);
    QGridLayout *inputLayout = new QGridLayout(inputGroup);
    inputLayout->setContentsMargins(12, 16, 12, 12);
    inputLayout->setHorizontalSpacing(10);
    inputLayout->setVerticalSpacing(10);
    inputLayout->addWidget(excelLabel, 0, 0);
    inputLayout->addWidget(m_excelPathEdit, 0, 1);
    inputLayout->addWidget(m_excelBrowseButton, 0, 2);
    inputLayout->addWidget(softwareLabel, 1, 0);
    inputLayout->addWidget(m_laneSoftwarePathEdit, 1, 1);
    inputLayout->addWidget(m_laneSoftwareBrowseButton, 1, 2);
    inputLayout->addWidget(scriptLabel, 2, 0);
    inputLayout->addWidget(m_syncScriptPathEdit, 2, 1);
    inputLayout->addWidget(m_syncScriptBrowseButton, 2, 2);
    inputLayout->addWidget(reportLabel, 3, 0);
    inputLayout->addWidget(m_reportPathEdit, 3, 1);
    inputLayout->addWidget(m_reportBrowseButton, 3, 2);
    inputLayout->addWidget(m_inputStatusText, 4, 1, 1, 2);
    inputLayout->setColumnStretch(1, 1);

    ElaText *progressTitle = new ElaText("部署进度", this);
    progressTitle->setTextPixelSize(16);
    QFont progressFont = progressTitle->font();
    progressFont.setBold(true);
    progressTitle->setFont(progressFont);

    m_stepperWidget = new StepperWidget(this);
    m_stepperWidget->setOrientation(StepperWidget::Orientation::Vertical);
    m_stepperWidget->setInteractive(false);
    m_stepperWidget->setShowStepIcons(true);
    initDeploymentSteps();

    ElaText *logTitle = new ElaText("部署日志", this);
    logTitle->setTextPixelSize(16);
    QFont logFont = logTitle->font();
    logFont.setBold(true);
    logTitle->setFont(logFont);

    m_clearLogButton = new ElaPushButton("清除日志", this);
    QHBoxLayout *logTitleLayout = new QHBoxLayout();
    logTitleLayout->setContentsMargins(0, 0, 0, 0);
    logTitleLayout->addWidget(logTitle);
    logTitleLayout->addStretch();
    logTitleLayout->addWidget(m_clearLogButton);

    m_logEdit = new ElaPlainTextEdit(this);
    m_logEdit->setReadOnly(true);
    m_logEdit->setMinimumHeight(160);
    m_logEdit->setPlaceholderText("部署过程、执行结果和错误信息将显示在这里");

    m_resetButton = new ElaPushButton("重置流程", this);
    m_cancelButton = new ElaPushButton("取消部署", this);
    m_startButton = new ElaPushButton("开始部署", this);
    m_cancelButton->setEnabled(false);
    m_startButton->setEnabled(false);

    QHBoxLayout *operationLayout = new QHBoxLayout();
    operationLayout->setContentsMargins(0, 0, 0, 0);
    operationLayout->addWidget(m_resetButton);
    operationLayout->addStretch();
    operationLayout->addWidget(m_cancelButton);
    operationLayout->addWidget(m_startButton);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("信创车道系统部署工具");
    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setContentsMargins(0, 0, 5, 0);
    centralLayout->setSpacing(10);
    centralLayout->addSpacing(5);
    centralLayout->addWidget(inputGroup);
    centralLayout->addWidget(progressTitle);
    centralLayout->addWidget(m_stepperWidget);
    centralLayout->addLayout(logTitleLayout);
    centralLayout->addWidget(m_logEdit);
    centralLayout->addLayout(operationLayout);
    addCentralWidget(centralWidget, true, true, 0);

    connect(m_excelBrowseButton, &ElaPushButton::clicked, this, &T_DeployTool::selectExcelFile);
    connect(m_laneSoftwareBrowseButton, &ElaPushButton::clicked, this, &T_DeployTool::selectLaneSoftwarePath);
    connect(m_syncScriptBrowseButton, &ElaPushButton::clicked, this, &T_DeployTool::selectSyncScript);
    connect(m_reportBrowseButton, &ElaPushButton::clicked, this, &T_DeployTool::selectReportPath);
    connect(m_startButton, &ElaPushButton::clicked, this, &T_DeployTool::startDeployment);
    connect(m_cancelButton, &ElaPushButton::clicked, this, [this]() {
        appendLog("已请求取消部署，正在等待当前操作结束");
        m_cancelButton->setEnabled(false);
        emit deploymentCancelRequested();
    });
    connect(m_resetButton, &ElaPushButton::clicked, this, &T_DeployTool::resetDeployment);
    connect(m_clearLogButton, &ElaPushButton::clicked, m_logEdit, &ElaPlainTextEdit::clear);
}

void T_DeployTool::initDeploymentSteps()
{
    QVector<StepperWidget::Step> steps;
    steps.reserve(10);

    const auto addStep = [&steps](const QString &title, const QString &description, const QString &iconText) {
        StepperWidget::Step step;
        step.title = title;
        step.description = description;
        step.iconText = iconText;
        steps.push_back(step);
    };

    addStep("加载车道信息采集表", "读取并校验 Excel 中的收费站、车道、数据库和设备配置", "表");
    addStep("写入车道数据库", "更新 t_laneconfig 与 t_lanebaseenv 表", "库");
    addStep("生成 LaneBaseConfig.ini", "生成车道基础配置并写入指定目录", "INI");
    addStep("生成 DeviceCtrl.json", "生成车道设备控制配置并写入指定目录", "DEV");
    addStep("生成 LaneUI.json", "生成车道界面配置并写入指定目录", "UI");
    addStep("生成 DtpAgent.cfg", "生成 DtpAgent 配置并写入指定目录", "DTP");
    addStep("生成 Start123", "生成车道服务启动配置并写入指定目录", "123");
    addStep("同步部署脚本", "复制同步脚本到车道软件 upload 目录", "脚本");
    addStep("重启车道服务", "依次重启 start123 与 DtpAgent 服务", "服务");
    addStep("生成部署报告", "汇总各步骤结果并生成可追溯的部署报告", "报告");

    m_stepperWidget->setSteps(steps);
    m_stepperWidget->setCurrentIndex(0);
}

void T_DeployTool::selectExcelFile()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this, "打开车道信息采集表", QString(), "Excel 工作簿 (*.xlsx)");
    if (filePath.isEmpty()) {
        return;
    }

    const QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isReadable()) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "打开失败", "采集表不存在或不可读取", 2000, this);
        return;
    }

    m_excelPathEdit->setText(fileInfo.absoluteFilePath());
    appendLog(QString("已选择车道信息采集表：%1").arg(fileInfo.fileName()));
    updateStartButtonState();
}

void T_DeployTool::selectLaneSoftwarePath()
{
    const QString path = QFileDialog::getExistingDirectory(this, "选择车道软件根目录");
    if (!path.isEmpty()) {
        m_laneSoftwarePathEdit->setText(path);
        appendLog(QString("车道软件目录：%1").arg(path));
        updateStartButtonState();
    }
}

void T_DeployTool::selectSyncScript()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this, "选择同步脚本", QString(), "Shell 脚本 (*.sh);;所有文件 (*)");
    if (!filePath.isEmpty()) {
        m_syncScriptPathEdit->setText(QFileInfo(filePath).absoluteFilePath());
        appendLog(QString("同步脚本：%1").arg(QFileInfo(filePath).fileName()));
        updateStartButtonState();
    }
}

void T_DeployTool::selectReportPath()
{
    const QString path = QFileDialog::getExistingDirectory(this, "选择部署报告保存目录");
    if (!path.isEmpty()) {
        m_reportPathEdit->setText(path);
        appendLog(QString("部署报告目录：%1").arg(path));
        updateStartButtonState();
    }
}

void T_DeployTool::startDeployment()
{
    if (!m_startButton->isEnabled() || m_isDeploying) {
        return;
    }

    m_isDeploying = true;
    initDeploymentSteps();
    setInputControlsEnabled(false);
    m_startButton->setEnabled(false);
    m_resetButton->setEnabled(false);
    m_cancelButton->setEnabled(true);
    appendLog("开始部署，正在加载并校验车道信息采集表");

    emit deploymentRequested(m_excelPathEdit->text(),
                             m_laneSoftwarePathEdit->text(),
                             m_syncScriptPathEdit->text(),
                             m_reportPathEdit->text());
}

void T_DeployTool::resetDeployment()
{
    if (m_isDeploying) {
        return;
    }
    initDeploymentSteps();
    appendLog("部署流程已重置");
    updateStartButtonState();
}

void T_DeployTool::setCurrentDeploymentStep(int stepIndex, const QString &message)
{
    if (!m_stepperWidget || stepIndex < 0 || stepIndex >= m_stepperWidget->steps().size()) {
        return;
    }
    m_stepperWidget->setCurrentIndex(stepIndex);
    if (!message.isEmpty()) {
        appendLog(message);
    }
}

void T_DeployTool::setDeploymentStepFailed(int stepIndex, const QString &errorMessage)
{
    if (!m_stepperWidget || stepIndex < 0 || stepIndex >= m_stepperWidget->steps().size()) {
        return;
    }

    m_stepperWidget->setCurrentIndex(stepIndex);
    m_stepperWidget->setStepState(stepIndex, StepperWidget::StepState::Error);
    appendLog(QString("部署失败：%1").arg(errorMessage));
    setDeploymentFinished(false, QString());
}

void T_DeployTool::setDeploymentFinished(bool success, const QString &message)
{
    if (!m_stepperWidget) {
        return;
    }

    if (success && !m_stepperWidget->steps().isEmpty()) {
        const int lastIndex = m_stepperWidget->steps().size() - 1;
        m_stepperWidget->setCurrentIndex(lastIndex);
        m_stepperWidget->setStepState(lastIndex, StepperWidget::StepState::Completed);
    }
    if (!message.isEmpty()) {
        appendLog(message);
    }

    m_isDeploying = false;
    setInputControlsEnabled(true);
    m_cancelButton->setEnabled(false);
    m_resetButton->setEnabled(true);
    updateStartButtonState();

    if (success) {
        ElaMessageBar::success(ElaMessageBarType::BottomRight, "部署完成", "车道系统部署流程已完成", 2500, this);
    } else {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "部署失败", "请查看部署日志并处理失败原因", 2500, this);
    }
}

void T_DeployTool::updateStartButtonState()
{
    const bool inputsReady = !m_excelPathEdit->text().isEmpty()
                             && !m_laneSoftwarePathEdit->text().isEmpty()
                             && !m_syncScriptPathEdit->text().isEmpty()
                             && !m_reportPathEdit->text().isEmpty();
    m_startButton->setEnabled(inputsReady && !m_isDeploying);
    m_inputStatusText->setText(inputsReady ? "部署输入已准备完成，可以开始部署" : "请先准备全部部署输入");
}

void T_DeployTool::setInputControlsEnabled(bool enabled)
{
    m_excelBrowseButton->setEnabled(enabled);
    m_laneSoftwareBrowseButton->setEnabled(enabled);
    m_syncScriptBrowseButton->setEnabled(enabled);
    m_reportBrowseButton->setEnabled(enabled);
}

void T_DeployTool::appendLog(const QString &message)
{
    if (!m_logEdit || message.isEmpty()) {
        return;
    }
    m_logEdit->appendPlainText(
        QString("%1 | %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), message));
}
