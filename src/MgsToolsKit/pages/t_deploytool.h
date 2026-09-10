#pragma once

#include "t_basepage.h"
#include <array>
#include <functional>
#include <memory>

class ElaComboBox;
class IDeployTool;
class ElaLineEdit;
class ElaPlainTextEdit;
class ElaPushButton;
class ElaText;
class ElaTreeView;
class QStandardItem;
class QStandardItemModel;
class StepperWidget;

struct ST_DeployInfo;
namespace DeployToolDef {
struct ST_DeployResult;
}

class T_DeployTool : public T_BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit T_DeployTool(QWidget *parent = nullptr);
    ~T_DeployTool() override;

private slots:
    // 选择车道信息采集表
    void onSelectExcelFile();
    // 加载当前车道部署信息
    void onLoadDeploymentInfo();
    // 展开或收起全部部署信息分组
    void onToggleInfoExpanded();

private:
    // 页面初始化
    void initContent();
    // 重置前端显示
    void resetFront();

    void refreshControlButtons();
    void refreshExpandButton();
    bool isAllInfoExpanded() const;
    // 分组展示部署信息
    void showDeploymentInfo(const ST_DeployInfo &info);
    // 界面添加日志
    void appendLog(const QString &message);
    void executeDeploymentStep(int step);
    void finishDeploymentStep(int step, bool success, const QString &message);
    // 执行单项部署操作，并记录成功日志或失败原因。
    static bool runDeploymentOperation(DeployToolDef::ST_DeployResult &result, const QString &name, const std::function<bool(QString &)> &operation);

    // 向部署信息分组添加配置项
    static void addField(QStandardItem *group, const QString &label, const QString &value);
    // 显示配置信息
    static QString displayValue(const QString &val);
    static QString displayValue(int val);
    static QString displayValue(bool val);

private:
    // 部署信息输入
    ElaLineEdit *m_excelPathEdit = nullptr;
    ElaLineEdit *m_stationEdit = nullptr;
    ElaLineEdit *m_laneEdit = nullptr;
    ElaPushButton *m_browseButton = nullptr;
    ElaPushButton *m_loadButton = nullptr;

    // 部署步骤指引
    StepperWidget *m_deployStepper = nullptr;

    // 部署执行参数和状态
    std::array<ElaPushButton *, 5> m_deployButtons{};
    ElaLineEdit *m_dtpVersionEdit = nullptr;
    ElaLineEdit *m_softVersionEdit = nullptr;
    ElaComboBox *m_interfaceCombo = nullptr;
    std::shared_ptr<const ST_DeployInfo> m_deployInfo;
    // 当前进行到的部署步骤 0: 收费软件初始化 1.Dtp初始化 2.start123初始化 3.费率同步 4.运行环境初始化
    int m_deployStep = 0;
    bool m_isDeploying = false;

    // 部署信息展示
    ElaText *m_infoTitleText = nullptr;
    ElaText *m_statusText = nullptr;
    ElaPushButton *m_expandButton = nullptr;
    ElaTreeView *m_infoTree = nullptr;
    QStandardItemModel *m_infoModel = nullptr;

    // 日志区
    ElaPlainTextEdit *m_logEdit = nullptr;
    ElaPushButton *m_logClearButton = nullptr;

    bool m_isLoading = false;
};
