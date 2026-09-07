#pragma once

#include "t_basepage.h"

class ElaLineEdit;
class ElaPlainTextEdit;
class ElaPushButton;
class ElaText;
class ElaTreeView;
class QStandardItemModel;

struct ST_DeployInfo;

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
    // 查询条件改变
    void onInputChanged();
    // 展开或收起全部部署信息分组
    void onToggleInfoExpanded();

private:
    // 页面初始化
    void initContent();
    void setLoadingState(bool loading);
    void refreshControlButtons();
    void refreshExpandButton();
    bool isAllInfoExpanded() const;
    // 分组展示部署信息
    void showDeploymentInfo(const ST_DeployInfo &info);
    void appendLog(const QString &message);

private:
    // 部署信息输入
    ElaLineEdit *m_excelPathEdit = nullptr;
    ElaLineEdit *m_stationEdit = nullptr;
    ElaLineEdit *m_laneEdit = nullptr;
    ElaPushButton *m_browseButton = nullptr;
    ElaPushButton *m_loadButton = nullptr;

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
