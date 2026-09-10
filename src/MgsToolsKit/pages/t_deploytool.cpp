#include "t_deploytool.h"

#include <memory>
#include <QFileDialog>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIntValidator>
#include <QNetworkInterface>
#include <QRegularExpressionValidator>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrentRun>

#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaMessageBar.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "ElaTreeView.h"
#include "Logger.h"
#include "global/appdefs.h"
#include "ideploytool.h"
#include "utils/datadealutils.h"
#include "utils/widgets/stepperwidget.h"

using namespace Utils;
using namespace DeployToolDef;

namespace {
ElaText *createLabel(const QString &text, QWidget *parent = nullptr)
{
    auto *label = new ElaText(text, parent);
    label->setTextPixelSize(14);
    label->setWordWrap(false);
    return label;
}

ElaText *createSectionTitle(const QString &text, QWidget *parent = nullptr)
{
    auto *title = createLabel(text, parent);
    QFont font = title->font();
    font.setBold(true);
    title->setFont(font);
    return title;
}

} // namespace

T_DeployTool::T_DeployTool(QWidget *parent)
    : T_BasePage{parent}
{
    setWindowTitle("信创车道系统部署工具");
    createCustomWidget("读取车道信息采集表中的部署信息，进行信创车道系统部署");
    initContent();

    connect(m_browseButton, &ElaPushButton::clicked, this, &T_DeployTool::onSelectExcelFile);
    connect(m_loadButton, &ElaPushButton::clicked, this, &T_DeployTool::onLoadDeploymentInfo);

    connect(m_logClearButton, &ElaPushButton::clicked, m_logEdit, &ElaPlainTextEdit::clear);

    // 执行步骤按钮绑定
    for (int step = 0; step < static_cast<int>(m_deployButtons.size()); ++step)
        connect(m_deployButtons[step], &ElaPushButton::clicked, this, [this, step]() { executeDeploymentStep(step); });

    connect(m_expandButton, &ElaPushButton::clicked, this, &T_DeployTool::onToggleInfoExpanded);
    connect(m_infoTree, &ElaTreeView::expanded, this, &T_DeployTool::refreshExpandButton);
    connect(m_infoTree, &ElaTreeView::collapsed, this, &T_DeployTool::refreshExpandButton);
    connect(m_infoModel, &QStandardItemModel::rowsInserted, this, &T_DeployTool::refreshExpandButton);
    connect(m_infoModel, &QStandardItemModel::rowsRemoved, this, &T_DeployTool::refreshExpandButton);

    connect(cuteLogger, &Logger::sigLogWrite, this, [this](Logger::LogLevel, const QString &log, const QString &category) {
        if (category == "DeployTool")
            appendLog(log);
    });

    refreshControlButtons();
}

T_DeployTool::~T_DeployTool() {}

void T_DeployTool::initContent()
{
    // 部署信息输入
    auto *inputGroup = new QGroupBox("部署信息加载", this);
    auto *inputLayout = new QGridLayout(inputGroup);
    inputLayout->setContentsMargins(12, 8, 12, 8);
    inputLayout->setSpacing(8);

    m_excelPathEdit = new ElaLineEdit(this);
    m_excelPathEdit->setReadOnly(true);
    m_excelPathEdit->setClearButtonEnabled(false);
    m_excelPathEdit->setPlaceholderText("请选择车道信息采集表（.xlsx）");
    m_browseButton = new ElaPushButton("选择采集表", this);
    m_stationEdit = new ElaLineEdit(this);
    m_stationEdit->setPlaceholderText("输入站代码");
    m_stationEdit->setValidator(new QIntValidator(1000, 9999, m_stationEdit)); // 站代码取值范围:1000~9999
    m_laneEdit = new ElaLineEdit(this);
    m_laneEdit->setPlaceholderText("输入车道号");
    m_laneEdit->setValidator(new QIntValidator(1, 99, m_laneEdit)); // 车道号取值范围:1~99
    m_loadButton = new ElaPushButton("加载信息", this);
    m_loadButton->setEnabled(false);

    inputLayout->addWidget(createLabel("采集表", this), 0, 0);
    inputLayout->addWidget(m_excelPathEdit, 0, 1, 1, 3);
    inputLayout->addWidget(m_browseButton, 0, 4);
    inputLayout->addWidget(createLabel("站代码", this), 1, 0);
    inputLayout->addWidget(m_stationEdit, 1, 1);
    inputLayout->addWidget(createLabel("车道号", this), 1, 2);
    inputLayout->addWidget(m_laneEdit, 1, 3);
    inputLayout->addWidget(m_loadButton, 1, 4);
    inputLayout->setColumnStretch(1, 1);
    inputLayout->setColumnStretch(3, 1);

    // 部署步骤指引，由实际部署结果推进步骤。
    m_deployStepper = new StepperWidget(this);
    m_deployStepper->setOrientation(StepperWidget::Orientation::Horizontal);
    m_deployStepper->setInteractive(false);
    m_deployStepper->setShowStepIcons(false); // 不显示图标
    m_deployStepper->setFocusPolicy(Qt::NoFocus);
    m_deployStepper->setFixedHeight(140);
    QFont stepFont = m_deployStepper->font();
    stepFont.setPointSize(10);
    m_deployStepper->setFont(stepFont);
    m_deployStepper->setSteps({{"收费软件", "配置文件、数据库初始化", StepperWidget::StepState::Pending, true, {}},
                               {"DtpAgent", "初始化配置并重启", StepperWidget::StepState::Pending, true, {}},
                               {"Start123", "初始化配置并重启", StepperWidget::StepState::Pending, true, {}},
                               {"费率", "同步费率参数", StepperWidget::StepState::Pending, true, {}},
                               {"网络配置", "更新网络配置", StepperWidget::StepState::Pending, true, {}},
                               {"完成", "部署结束", StepperWidget::StepState::Pending, true, {}}});
    m_deployStepper->setCurrentIndex(0);

    // 部署执行：上排填写参数，下排依次执行五个步骤。
    auto *executeGroup = new QGroupBox("部署执行", this);
    auto *executeLayout = new QVBoxLayout(executeGroup);
    executeLayout->setContentsMargins(12, 8, 12, 8);
    executeLayout->setSpacing(8);

    m_dtpVersionEdit = new ElaLineEdit(this);
    m_dtpVersionEdit->setPlaceholderText("输入全量版本");
    m_dtpVersionEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{8}"), m_dtpVersionEdit));
    m_softVersionEdit = new ElaLineEdit(this);
    m_softVersionEdit->setPlaceholderText("输入软件版本");
    m_softVersionEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+"), m_softVersionEdit));
    m_interfaceCombo = new ElaComboBox(this);
    for (const auto &interface : QNetworkInterface::allInterfaces()) {
        if (!(interface.flags() & QNetworkInterface::IsLoopBack))
            m_interfaceCombo->addItem(interface.humanReadableName() + " (" + interface.name() + ")", interface.name());
    }
    m_interfaceCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    m_interfaceCombo->setMinimumContentsLength(8);
    auto *parameterLayout = new QHBoxLayout();
    parameterLayout->setSpacing(8);
    parameterLayout->addWidget(createLabel("全量版本", this));
    parameterLayout->addWidget(m_dtpVersionEdit, 1);
    parameterLayout->addWidget(createLabel("软件版本", this));
    parameterLayout->addWidget(m_softVersionEdit, 1);
    parameterLayout->addWidget(createLabel("网卡", this));
    parameterLayout->addWidget(m_interfaceCombo, 2);
    executeLayout->addLayout(parameterLayout);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(8);
    for (int step = 0; step < static_cast<int>(m_deployButtons.size()); ++step) {
        m_deployButtons[step] = new ElaPushButton(deploymentNames[step], this);
        buttonLayout->addWidget(m_deployButtons[step], 1);
    }
    executeLayout->addLayout(buttonLayout);

    // 部署信息展示
    m_infoTitleText = createSectionTitle("部署信息", this);
    m_statusText = createLabel("尚未加载", this);
    m_expandButton = new ElaPushButton("展开全部", this);
    m_expandButton->setEnabled(false);
    auto *infoTitleLayout = new QHBoxLayout();
    infoTitleLayout->setContentsMargins(0, 0, 0, 0);
    infoTitleLayout->setSpacing(8);
    infoTitleLayout->addWidget(m_infoTitleText);
    infoTitleLayout->addWidget(m_statusText);
    infoTitleLayout->addWidget(m_expandButton);
    infoTitleLayout->addStretch();
    m_infoModel = new QStandardItemModel(0, 2, this);
    m_infoTree = new ElaTreeView(this);
    m_infoTree->setModel(m_infoModel);
    m_infoTree->setHeaderHidden(true);
    m_infoTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_infoTree->setRootIsDecorated(true);
    m_infoTree->setMinimumHeight(200);
    m_infoTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_infoTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_infoTree->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_infoTree->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 日志区
    auto *logTitle = createSectionTitle("交互日志", this);
    m_logClearButton = new ElaPushButton("清除", this);
    auto *logTitleLayout = new QHBoxLayout();
    logTitleLayout->setContentsMargins(0, 0, 0, 0);
    logTitleLayout->addWidget(logTitle);
    logTitleLayout->addWidget(m_logClearButton);
    logTitleLayout->addStretch();
    m_logEdit = new ElaPlainTextEdit(this);
    m_logEdit->setReadOnly(true);
    m_logEdit->setMaximumBlockCount(2000);
    m_logEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_logEdit->setMinimumHeight(150);
    m_logEdit->setPlaceholderText("采集表加载、查询结果和错误信息将显示在这里");

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle(windowTitle());
    auto *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setContentsMargins(0, 5, 5, 0);
    centralLayout->setSpacing(5);
    centralLayout->addWidget(inputGroup);
    centralLayout->addWidget(m_deployStepper);
    centralLayout->addWidget(executeGroup);
    centralLayout->addLayout(infoTitleLayout);
    centralLayout->addWidget(m_infoTree, 1);
    centralLayout->addLayout(logTitleLayout);
    centralLayout->addWidget(m_logEdit, 2);
    addCentralWidget(centralWidget, true, true, 0);
}

void T_DeployTool::resetFront()
{
    m_deployInfo.reset();
    m_deployStep = 0;
    for (int step = 0; step < m_deployStepper->steps().size(); ++step)
        m_deployStepper->setStepState(step, StepperWidget::StepState::Pending);
    m_deployStepper->setCurrentIndex(0);

    m_infoModel->removeRows(0, m_infoModel->rowCount());
    m_statusText->setText("尚未加载");
    refreshControlButtons();
}

void T_DeployTool::onSelectExcelFile()
{
    const QString path = QFileDialog::getOpenFileName(this, "选择车道信息采集表", m_excelPathEdit->text(), "*.xlsx");
    if (path.isEmpty())
        return;

    QFileInfo fileInfo(path);
    if (fileInfo.suffix().compare("xlsx", Qt::CaseInsensitive) != 0) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight, "文件无效", "请选择 XLSX 格式的车道信息采集表", 2000, this);
        appendLog(QString("文件无效：%1").arg(path));
        return;
    }

    m_excelPathEdit->setText(fileInfo.absoluteFilePath());
    appendLog(QString("已选择采集表：%1").arg(path));

    resetFront();
}

bool T_DeployTool::isAllInfoExpanded() const
{
    bool hasGroup = false;
    // 当前模型为“配置分组 / 配置项”两层，仅分组需要展开。
    for (int row = 0; row < m_infoModel->rowCount(); ++row) {
        const QModelIndex index = m_infoModel->index(row, 0);
        if (!m_infoModel->hasChildren(index))
            continue;
        hasGroup = true;
        if (!m_infoTree->isExpanded(index))
            return false;
    }
    return hasGroup;
}

void T_DeployTool::refreshExpandButton()
{
    m_expandButton->setEnabled(m_infoModel->rowCount() > 0);
    m_expandButton->setText(isAllInfoExpanded() ? "收起全部" : "展开全部");
}

void T_DeployTool::onToggleInfoExpanded()
{
    if (isAllInfoExpanded()) {
        m_infoTree->collapseAll();
    } else {
        m_infoTree->expandAll();
    }
    refreshExpandButton();
}

void T_DeployTool::onLoadDeploymentInfo()
{
    if (m_isLoading || m_isDeploying)
        return;
    resetFront(); // 重置界面

    const QString path = m_excelPathEdit->text();
    const QString stationID = m_stationEdit->text().trimmed();
    const QString laneID = m_laneEdit->text().trimmed();
    QString error;
    const QFileInfo file(path);
    if (path.isEmpty()) {
        error = "请先选择车道信息采集表";
    } else if (!file.isFile() || !file.isReadable()) {
        error = "采集表不存在或不可读取，请重新选择";
    } else if (stationID.isEmpty()) {
        error = "请输入站代码";
    } else if (laneID.isEmpty()) {
        error = "请输入 1～99 范围内的车道号";
    }
    if (!error.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight, "参数有误", error, 2000, this);
        appendLog(error);
        return;
    }

    appendLog(QString("开始加载：站代码 %1，车道号 %2").arg(stationID).arg(laneID));
    m_isLoading = true;
    refreshControlButtons();

    // watcher 负责监视任务，真正执行任务的是后面的 QtConcurrent::run()
    auto *watcher = new QFutureWatcher<ST_DeployInfoLoadResult>(this);
    connect(watcher, &QFutureWatcher<ST_DeployInfoLoadResult>::finished, this, [this, watcher]() {
        const ST_DeployInfoLoadResult result = watcher->result();
        watcher->deleteLater();
        m_isLoading = false;
        refreshControlButtons();
        if (!result.error.isEmpty()) {
            ElaMessageBar::error(ElaMessageBarType::BottomRight, "加载失败", result.error, 2500, this);
            appendLog("加载失败：" + result.error);
            return;
        }
        showDeploymentInfo(result.info);
        m_deployInfo = std::make_shared<ST_DeployInfo>(result.info);
        refreshControlButtons();

        const auto &base = result.info.baseInfo;
        const QString message = QString("%1（%2），车道 %3").arg(base.stationName, base.stationID).arg(base.laneID);
        m_statusText->setText(message);
        appendLog(message);
    });
    // 后台任务只捕获输入值，页面关闭后也不会访问已销毁的控件。
    watcher->setFuture(QtConcurrent::run([path, stationID, laneID]() {
        ST_DeployInfoLoadResult result;
        std::unique_ptr<IDeployTool, decltype(&destroyDeployTool)> tool(createDeployTool(), &destroyDeployTool);

        if (!tool->loadDeployInfo(path, result.error)) {
            if (result.error.isEmpty())
                result.error = "采集表加载失败";
            return result;
        }
        result.info = tool->getCurDeployInfo(stationID, laneID.toInt());
        if (result.info.baseInfo.stationID != stationID || result.info.baseInfo.laneID != laneID.toInt())
            result.error = QString("采集表中未找到站代码 %1、车道号 %2 的部署信息").arg(stationID).arg(laneID);
        return result;
    }));
}

void T_DeployTool::refreshControlButtons()
{
    const bool busy = m_isLoading || m_isDeploying;
    m_browseButton->setEnabled(!busy);
    m_stationEdit->setEnabled(!busy);
    m_laneEdit->setEnabled(!busy);
    m_loadButton->setEnabled(!busy && !m_excelPathEdit->text().isEmpty());

    // 对应步骤完成之后，参数就会锁定
    m_dtpVersionEdit->setEnabled(!busy && m_deployStep <= 1);
    m_softVersionEdit->setEnabled(!busy && m_deployStep <= 2);
    m_interfaceCombo->setEnabled(!busy && m_deployStep <= 4);

    // 按钮可操作: 1. 当前没有任务在执行 2. m_deployInfo 非空，已经加载了部署信息 3. 按钮对应的步骤就是当前步骤。
    for (int step = 0; step < static_cast<int>(m_deployButtons.size()); ++step) {
        m_deployButtons[step]->setEnabled(!busy && m_deployInfo && step == m_deployStep);
    }
}

void T_DeployTool::executeDeploymentStep(int step)
{
    if (m_isLoading || m_isDeploying || !m_deployInfo || step != m_deployStep || step < 0 || step >= 5)
        return;

    QString error;
    if (step == 1 && !m_dtpVersionEdit->hasAcceptableInput()) {
        error = "请输入数字格式的全量版本号";
    } else if (step == 2 && !m_softVersionEdit->hasAcceptableInput()) {
        error = "请输入有效的软件版本号（正整数）";
    } else if (step == 4 && m_interfaceCombo->currentData().toString().isEmpty()) {
        error = "请选择需要修改网络配置的网卡";
    }
    if (!error.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight, "参数有误", error, 2500, this);
        appendLog(error);
        return;
    }

    QString feePath;
    if (step == 3) {
        feePath = QFileDialog::getOpenFileName(this, "选择590费率文件", QString(), "*.xml");
        if (feePath.isEmpty())
            return;
        const QFileInfo file(feePath);
        if (file.suffix().compare("xml", Qt::CaseInsensitive) != 0) {
            ElaMessageBar::warning(ElaMessageBarType::BottomRight, "文件无效", "请选择 XML 格式的费率文件", 2000, this);
            appendLog("文件无效：" + feePath);
            return;
        }
    }

    m_isDeploying = true;
    m_deployStepper->setStepState(step, StepperWidget::StepState::Current);
    refreshControlButtons();
    appendLog("开始执行：" + deploymentNames[step]);

    const auto info = m_deployInfo;
    const QString dtpVersion = m_dtpVersionEdit->text().trimmed();
    const QString softVersion = m_softVersionEdit->text().trimmed();
    const QString interfaceName = m_interfaceCombo->currentData().toString();
    auto *watcher = new QFutureWatcher<ST_DeployResult>(this);
    connect(watcher, &QFutureWatcher<ST_DeployResult>::finished, this, [this, watcher, step]() {
        const ST_DeployResult result = watcher->result();
        watcher->deleteLater();
        for (const auto &log : result.logs)
            appendLog(log);
        finishDeploymentStep(step, result.success, result.message);
    });
    // 只捕获参数快照；数据库连接及工具在同一工作线程内创建、使用、销毁。
    watcher->setFuture(QtConcurrent::run([info, step, dtpVersion, softVersion, feePath, interfaceName]() {
        ST_DeployResult result;
        std::unique_ptr<IDeployTool, decltype(&destroyDeployTool)> tool(createDeployTool(), &destroyDeployTool);

        switch (step) {
        case 0:
            result.success = runDeploymentOperation(result, "收费软件初始化", [&](QString &e) { return tool->initLaneSoftware(*info, e); });
            break;
        case 1:
            result.success = runDeploymentOperation(result, "保存DtpAgent.cfg并重启Dtp服务",
                                                    [&](QString &e) { return tool->initDtpAgent(*info, dtpVersion, e); });
            break;
        case 2:
            result.success = runDeploymentOperation(result, "保存Start123.json并重启Start123服务",
                                                    [&](QString &e) { return tool->initStart123(*info, softVersion, e); });
            break;
        case 3:
            result.success = runDeploymentOperation(result, "同步590费率文件（站代码 " + info->baseInfo.stationID + "）",
                                                    [&](QString &e) { return tool->syncFeeRate(feePath, info->baseInfo.stationID, e); });
            break;
        case 4:
            result.success = runDeploymentOperation(result, "修改网络配置", [&](QString &e) { return tool->updateNetwork(*info, interfaceName, e); });
            break;
        }
        return result;
    }));
}

bool T_DeployTool::runDeploymentOperation(DeployToolDef::ST_DeployResult &result, const QString &name, const std::function<bool(QString &)> &operation)
{
    QString detail;
    if (!operation(detail)) {
        result.message = name + "失败：" + (detail.isEmpty() ? "接口未返回错误详情" : detail);
        return false;
    }
    result.logs.append(name + "成功");
    return true;
}

void T_DeployTool::finishDeploymentStep(int step, bool success, const QString &message)
{
    m_isDeploying = false;
    if (success) {
        m_deployStep = step + 1;
        m_deployStepper->setCurrentIndex(m_deployStep);
        if (m_deployStep == 5)
            m_deployStepper->setStepState(5, StepperWidget::StepState::Completed);
        const QString text = deploymentNames[step] + "成功";
        appendLog(text);
        if (!message.isEmpty())
            appendLog(message);

        ElaMessageBar::success(ElaMessageBarType::BottomRight, "执行成功", text, 2000, this);
    } else {
        m_deployStepper->setStepState(step, StepperWidget::StepState::Error);
        const QString text = message.isEmpty() ? deploymentNames[step] + "失败" : message;
        appendLog(text);
        ElaMessageBar::error(ElaMessageBarType::BottomRight, "执行失败", text, 2000, this);
    }
    refreshControlButtons();
}

void T_DeployTool::appendLog(const QString &message)
{
    if (!message.isEmpty())
        m_logEdit->appendPlainText(DataDealUtils::curDateTimeStr() + " | " + message + "\n");
}

void T_DeployTool::addField(QStandardItem *group, const QString &label, const QString &value)
{
    auto *nameItem = new QStandardItem(label);
    auto *valueItem = new QStandardItem(value);
    group->appendRow({nameItem, valueItem});
}

QString T_DeployTool::displayValue(const QString &val)
{
    return val.isEmpty() ? QStringLiteral("未配置") : val;
}

QString T_DeployTool::displayValue(int val)
{
    return QString::number(val);
}

QString T_DeployTool::displayValue(bool val)
{
    return val ? QStringLiteral("是") : QStringLiteral("否");
}

void T_DeployTool::showDeploymentInfo(const ST_DeployInfo &info)
{
    m_infoModel->removeRows(0, m_infoModel->rowCount()); // 清除旧有数据

    auto *group = new QStandardItem("基础信息");
    m_infoModel->appendRow({group, new QStandardItem()});
    addField(group, "站代码", displayValue(info.baseInfo.stationID));
    addField(group, "站名称", displayValue(info.baseInfo.stationName));
    addField(group, "站级 IP", displayValue(info.baseInfo.stationIP));
    addField(group, "车道号", displayValue(info.baseInfo.laneID));
    addField(group, "车道类型", QStringList({"未知", "混合入口", "混合出口", "ETC入口", "ETC出口"}).value(info.baseInfo.laneType, ""));
    addField(group, "车道 IP", displayValue(info.baseInfo.laneIP));
    addField(group, "子网掩码", displayValue(info.baseInfo.laneNetmask));
    addField(group, "网关", displayValue(info.baseInfo.laneGateway));
    addField(group, "路网编号", displayValue(info.baseInfo.roadNetNo));
    addField(group, "心跳上传 IP", displayValue(info.baseInfo.heartIP));
    addField(group, "承载门架编号", displayValue(info.baseInfo.flagHexNo));
    addField(group, "承载门架名称", displayValue(info.baseInfo.flagName));
    addField(group, "后通道", displayValue(info.baseInfo.isConvenientLane));
    addField(group, "自助缴费车道", displayValue(info.isAutoPay));
    m_infoTree->expand(group->index());

    if (info.db.isEnable) {
        auto *group = new QStandardItem("数据库");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "IP 地址", displayValue(info.db.ip));
        addField(group, "数据库名称", displayValue(info.db.dbName));
        addField(group, "用户名", displayValue(info.db.dbUser));
        addField(group, "密码", displayValue(info.db.dbPassword));
        addField(group, "端口", displayValue(info.db.dbPort));
        addField(group, "数据库类型", displayValue(info.db.dbType));
    }
    if (info.redis.isEnable) {
        auto *group = new QStandardItem("Redis");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "IP 地址", displayValue(info.redis.ip));
        addField(group, "数据库名称", displayValue(info.redis.dbName));
        addField(group, "用户名", displayValue(info.redis.dbUser));
        addField(group, "密码", displayValue(info.redis.password));
        addField(group, "端口", displayValue(info.redis.port));
    }
    if (info.reader.isEnable) {
        auto *group = new QStandardItem("桌面读卡器");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "动态库类型", displayValue(info.reader.apiType));
        addField(group, "动态库名称", displayValue(info.reader.apiFileName));
        addField(group, "卡槽位", displayValue(info.reader.slot));
        addField(group, "串口", displayValue(info.reader.comPort));
    }
    if (info.reader.isEnable) {
        auto *group = new QStandardItem("天线");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "IP 地址", displayValue(info.rsu.ip));
        addField(group, "功率", displayValue(info.rsu.power));
        addField(group, "端口", displayValue(info.rsu.port));
    }
    if (info.reader.isEnable) {
        auto *group = new QStandardItem("抓拍相机");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "IP 地址", displayValue(info.capture.ip));
        addField(group, "站点 HEX 编码", displayValue(info.capture.stationHex));
        addField(group, "站名称", displayValue(info.capture.stationName));
        addField(group, "车道号", displayValue(info.capture.laneID));
        addField(group, "端口", displayValue(info.capture.port));
        addField(group, "触发模式", displayValue(info.capture.triggerMode));
        addField(group, "类型", displayValue(info.capture.type));
    }
    if (info.reader.isEnable) {
        auto *group = new QStandardItem("发卡机");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "IP 地址", displayValue(info.cardRobot.ip));
        addField(group, "端口 / 串口", displayValue(info.cardRobot.port));
        addField(group, "上读卡器串口", displayValue(info.cardRobot.upReaderPort));
        addField(group, "下读卡器串口", displayValue(info.cardRobot.dnReaderPort));
        addField(group, "上下工位共享读卡器", displayValue(info.cardRobot.isShareReader));
        addField(group, "读卡器动态库", displayValue(info.cardRobot.readerApiFileName));
        addField(group, "厂家", displayValue(info.cardRobot.producer));
        addField(group, "读卡器槽位", displayValue(info.cardRobot.readerSlot));
        addField(group, "读卡器接口类型", displayValue(info.cardRobot.readerApiType));
        addField(group, "波特率", displayValue(info.cardRobot.baudRate));
        addField(group, "交叉验证", displayValue(info.cardRobot.option));
    }
    if (info.overlap.isEnable) {
        auto *group = new QStandardItem("字符叠加器");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "串口", displayValue(info.overlap.comPort));
        addField(group, "波特率", displayValue(info.overlap.baudRate));
        addField(group, "类型", displayValue(info.overlap.type));
    }
    if (info.overlap.isEnable) {
        auto *group = new QStandardItem("称重设备");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "串口", displayValue(info.weight.comPort));
        addField(group, "基地址", displayValue(info.weight.addr));
        addField(group, "波特率", displayValue(info.weight.baudRate));
        addField(group, "整车称", displayValue(info.weight.IWP));
    }
    if (info.feeScr.isEnable) {
        auto *group = new QStandardItem("费额显示器");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "端口 / 串口", displayValue(info.feeScr.port));
        addField(group, "波特率", displayValue(info.feeScr.baudRate));
        addField(group, "刷新时间（毫秒）", displayValue(info.feeScr.sleepMillisecond));
        addField(group, "类型", displayValue(info.feeScr.type));
    }
    if (info.smartLaneCtrl.isEnable) {
        auto *group = new QStandardItem("智能网关");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "IP 地址", displayValue(info.smartLaneCtrl.ip));
        addField(group, "端口", displayValue(info.smartLaneCtrl.port));
    }
    if (info.switchDev.isEnable) {
        auto *group = new QStandardItem("车控器");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "基地址", displayValue(info.switchDev.baseAddr));
        addField(group, "抓拍线圈", displayValue(info.switchDev.beginCoil));
        addField(group, "天棚灯", displayValue(info.switchDev.ceilingLamp));
        addField(group, "线圈 1", displayValue(info.switchDev.coil1));
        addField(group, "线圈 2", displayValue(info.switchDev.coil2));
        addField(group, "线圈 3", displayValue(info.switchDev.coil3));
        addField(group, "线圈 4", displayValue(info.switchDev.coil4));
        addField(group, "其他线圈数量", displayValue(info.switchDev.coilNum));
        addField(group, "落杆线圈", displayValue(info.switchDev.endCoil));
        addField(group, "通行灯", displayValue(info.switchDev.passingLamp));
        addField(group, "落杆", displayValue(info.switchDev.railingDown1));
        addField(group, "抬杆", displayValue(info.switchDev.railingUp1));
        addField(group, "报警", displayValue(info.switchDev.warning));
        addField(group, "类型", displayValue(info.switchDev.type));
        addField(group, "设备编号", displayValue(info.switchDev.which));
    }
    if (info.recognizer.isEnable) {
        auto *group = new QStandardItem("车型识别器");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "IP 地址", displayValue(info.recognizer.ip));
        addField(group, "端口", displayValue(info.recognizer.port));
    }
    if (info.payRobot.isEnable) {
        auto *group = new QStandardItem("缴费机");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "IP 地址", displayValue(info.payRobot.ip));
        addField(group, "端口", displayValue(info.payRobot.port));
        addField(group, "上读卡器串口", displayValue(info.payRobot.upReaderPort));
        addField(group, "下读卡器串口", displayValue(info.payRobot.dnReaderPort));
        addField(group, "上下工位共享读卡器", displayValue(info.payRobot.isShareReader));
        addField(group, "读卡器动态库", displayValue(info.payRobot.readerApiFileName));
        addField(group, "厂家", displayValue(info.payRobot.producer));
        addField(group, "读卡器槽位", displayValue(info.payRobot.readerSlot));
        addField(group, "读卡器接口类型", displayValue(info.payRobot.readerApiType));
    }
    if (info.infoboard.isEnable) {
        auto *group = new QStandardItem("折叠情报板");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "类型", displayValue(info.infoboard.type));
        addField(group, "IP 地址", displayValue(info.infoboard.ip));
        addField(group, "端口", displayValue(info.infoboard.port));
    }
}
