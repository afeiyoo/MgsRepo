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
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrentRun>

#include "ElaLineEdit.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "ElaTreeView.h"
#include "Logger.h"
#include "ideploytool.h"
#include "utils/datadealutils.h"

using namespace Utils;

namespace {
// Ela 的文本绘制额外留有左边距，尺寸提示也需预留空间，避免列间文字挤压。
class DeployInfoDelegate : public QStyledItemDelegate
{
public:
    explicit DeployInfoDelegate(QObject *parent)
        : QStyledItemDelegate(parent)
    {}

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.rwidth() += 40;
        return size;
    }
};

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

struct LoadResult
{
    ST_DeployInfo info;
    QString error;
};

QString displayValue(const QString &value)
{
    return value.isEmpty() ? QStringLiteral("未配置") : value;
}
QString displayValue(int value)
{
    return QString::number(value);
}
QString displayValue(bool value)
{
    return value ? QStringLiteral("是") : QStringLiteral("否");
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
    connect(m_stationEdit, &ElaLineEdit::returnPressed, this, &T_DeployTool::onLoadDeploymentInfo);
    connect(m_laneEdit, &ElaLineEdit::returnPressed, this, &T_DeployTool::onLoadDeploymentInfo);
    connect(m_excelPathEdit, &ElaLineEdit::textChanged, this, &T_DeployTool::onInputChanged);
    connect(m_stationEdit, &ElaLineEdit::textChanged, this, &T_DeployTool::onInputChanged);
    connect(m_laneEdit, &ElaLineEdit::textChanged, this, &T_DeployTool::onInputChanged);
    connect(m_logClearButton, &ElaPushButton::clicked, m_logEdit, &ElaPlainTextEdit::clear);
    connect(m_expandButton, &ElaPushButton::clicked, this, &T_DeployTool::onToggleInfoExpanded);
    connect(m_infoTree, &ElaTreeView::expanded, this, &T_DeployTool::refreshExpandButton);
    connect(m_infoTree, &ElaTreeView::collapsed, this, &T_DeployTool::refreshExpandButton);
    connect(m_infoModel, &QStandardItemModel::rowsInserted, this, &T_DeployTool::refreshExpandButton);
    connect(m_infoModel, &QStandardItemModel::rowsRemoved, this, &T_DeployTool::refreshExpandButton);

    connect(cuteLogger, &Logger::sigLogWrite, this, [this](Logger::LogLevel, const QString &log, const QString &category) {
        if (category == "DeployTool")
            appendLog(log);
    });

    setLoadingState(false);
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
    m_loadButton = new ElaPushButton("加载部署信息", this);
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

    // 部署信息展示
    m_statusText = createSectionTitle("部署信息: 尚未加载", this);
    m_expandButton = new ElaPushButton("展开全部", this);
    m_expandButton->setEnabled(false);
    auto *infoTitleLayout = new QHBoxLayout();
    infoTitleLayout->setContentsMargins(0, 0, 0, 0);
    infoTitleLayout->setSpacing(8);
    infoTitleLayout->addWidget(m_statusText);
    infoTitleLayout->addWidget(m_expandButton);
    infoTitleLayout->addStretch();
    m_infoModel = new QStandardItemModel(0, 2, this);
    m_infoTree = new ElaTreeView(this);
    m_infoTree->setModel(m_infoModel);
    m_infoTree->setHeaderHidden(true);
    m_infoTree->setItemDelegate(new DeployInfoDelegate(m_infoTree));
    m_infoTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_infoTree->setRootIsDecorated(true);
    m_infoTree->setMinimumHeight(220);
    m_infoTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_infoTree->header()->setResizeContentsPrecision(-1);
    m_infoTree->header()->setStretchLastSection(true);
    m_infoTree->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

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
    m_logEdit->setMinimumHeight(150);
    m_logEdit->setPlaceholderText("采集表加载、车道查询结果和错误信息将显示在这里");

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle(windowTitle());
    auto *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setContentsMargins(0, 5, 5, 0);
    centralLayout->setSpacing(5);
    centralLayout->addWidget(inputGroup);
    centralLayout->addLayout(infoTitleLayout);
    centralLayout->addWidget(m_infoTree, 2);
    centralLayout->addLayout(logTitleLayout);
    centralLayout->addWidget(m_logEdit, 1);
    addCentralWidget(centralWidget, true, true, 0);
}

void T_DeployTool::onSelectExcelFile()
{
    const QString path = QFileDialog::getOpenFileName(this, "选择车道信息采集表", m_excelPathEdit->text(), "Excel 工作簿 (*.xlsx)");
    if (path.isEmpty())
        return;
    m_excelPathEdit->setText(QFileInfo(path).absoluteFilePath());

    onInputChanged();
    appendLog(QString("已选择采集表：%1").arg(path));
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
    if (isAllInfoExpanded())
        m_infoTree->collapseAll();
    else
        m_infoTree->expandAll();
    refreshExpandButton();
}

void T_DeployTool::onInputChanged()
{
    refreshControlButtons();
    m_infoModel->removeRows(0, m_infoModel->rowCount());
}

void T_DeployTool::onLoadDeploymentInfo()
{
    if (m_isLoading)
        return;
    onInputChanged();
    const QString path = m_excelPathEdit->text();
    const QString stationID = m_stationEdit->text().trimmed();
    bool laneValid = false;
    const int laneID = m_laneEdit->text().trimmed().toInt(&laneValid);
    QString error;
    const QFileInfo file(path);
    if (path.isEmpty())
        error = "请先选择车道信息采集表";
    else if (!file.isFile() || !file.isReadable())
        error = "采集表不存在或不可读取，请重新选择";
    else if (stationID.isEmpty())
        error = "请输入站代码";
    else if (!laneValid || laneID < 1 || laneID > 99)
        error = "请输入 1～99 范围内的车道号";
    if (!error.isEmpty()) {
        m_statusText->setText(error);
        appendLog(error);
        return;
    }

    setLoadingState(true);
    appendLog(QString("开始加载：站代码 %1，车道号 %2").arg(stationID).arg(laneID));

    auto *watcher = new QFutureWatcher<LoadResult>(this);
    connect(watcher, &QFutureWatcher<LoadResult>::finished, this, [this, watcher]() {
        const LoadResult result = watcher->result();
        watcher->deleteLater();
        setLoadingState(false);
        if (!result.error.isEmpty()) {
            m_statusText->setText(result.error);
            appendLog("加载失败：" + result.error);
            return;
        }
        showDeploymentInfo(result.info);
        const auto &base = result.info.baseInfo;
        const QString message = QString("%1（%2），车道 %3").arg(base.stationName, base.stationID).arg(base.laneID);
        m_statusText->setText(message);
        appendLog(message);
    });
    // 后台任务只捕获输入值，页面关闭后也不会访问已销毁的控件。
    watcher->setFuture(QtConcurrent::run([path, stationID, laneID]() {
        LoadResult result;
        std::unique_ptr<IDeployTool, decltype(&destroyDeployTool)> tool(createDeployTool(), &destroyDeployTool);
        if (!tool) {
            result.error = "无法创建部署信息读取器";
            return result;
        }
        if (!tool->loadDeployInfo(path, result.error)) {
            if (result.error.isEmpty())
                result.error = "采集表加载失败";
            return result;
        }
        result.info = tool->getCurDeployInfo(stationID, laneID);
        if (result.info.baseInfo.stationID != stationID || result.info.baseInfo.laneID != laneID)
            result.error = QString("采集表中未找到站代码 %1、车道号 %2 的部署信息").arg(stationID).arg(laneID);
        return result;
    }));
}

void T_DeployTool::setLoadingState(bool loading)
{
    m_isLoading = loading;
    m_loadButton->setText(loading ? "正在加载…" : "加载部署信息");
    refreshControlButtons();
}

void T_DeployTool::refreshControlButtons()
{
    m_browseButton->setEnabled(!m_isLoading);
    m_stationEdit->setEnabled(!m_isLoading);
    m_laneEdit->setEnabled(!m_isLoading);
    m_loadButton->setEnabled(!m_isLoading && !m_excelPathEdit->text().isEmpty());
}

void T_DeployTool::appendLog(const QString &message)
{
    if (!message.isEmpty())
        m_logEdit->appendPlainText(DataDealUtils::curDateTimeStr() + " | " + message + "\n");
}

void T_DeployTool::showDeploymentInfo(const ST_DeployInfo &info)
{
    m_infoModel->removeRows(0, m_infoModel->rowCount());
    const auto addField = [](QStandardItem *group, const QString &label, const QString &value) {
        auto *nameItem = new QStandardItem(label);
        auto *valueItem = new QStandardItem(value);
        valueItem->setToolTip(value);
        group->appendRow({nameItem, valueItem});
    };
    {
        auto *group = new QStandardItem("基础信息");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "站代码", displayValue(info.baseInfo.stationID));
        addField(group, "站名称", displayValue(info.baseInfo.stationName));
        addField(group, "站级 IP", displayValue(info.baseInfo.stationIP));
        addField(group, "车道 IP", displayValue(info.baseInfo.laneIP));
        addField(group, "子网掩码", displayValue(info.baseInfo.laneNetmask));
        addField(group, "网关", displayValue(info.baseInfo.laneGateway));
        addField(group, "路网编号", displayValue(info.baseInfo.roadNetNo));
        addField(group, "心跳上传 IP", displayValue(info.baseInfo.heartIP));
        addField(group, "承载门架编号", displayValue(info.baseInfo.flagHexNo));
        addField(group, "承载门架名称", displayValue(info.baseInfo.flagName));
        addField(group, "车道类型",
                 QStringList({"未知", "混合入口", "混合出口", "ETC入口", "ETC出口"})
                     .value(info.baseInfo.laneType, QString::number(info.baseInfo.laneType)));
        addField(group, "车道号", displayValue(info.baseInfo.laneID));
        addField(group, "后通道", displayValue(info.baseInfo.isConvenientLane));
        addField(group, "自助缴费车道", displayValue(info.isAutoPay));
        m_infoTree->expand(group->index());
    }
    {
        auto *group = new QStandardItem("数据库");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "启用", displayValue(info.db.isEnable));
        addField(group, "IP 地址", displayValue(info.db.ip));
        addField(group, "数据库名称", displayValue(info.db.dbName));
        addField(group, "用户名", displayValue(info.db.dbUser));
        addField(group, "密码", info.db.dbPassword.isEmpty() ? QStringLiteral("未配置") : QStringLiteral("••••••"));
        addField(group, "数据库端口", displayValue(info.db.dbPort));
        addField(group, "数据库类型", displayValue(info.db.dbType));
    }
    {
        auto *group = new QStandardItem("Redis");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "启用", displayValue(info.redis.isEnable));
        addField(group, "IP 地址", displayValue(info.redis.ip));
        addField(group, "数据库名称", displayValue(info.redis.dbName));
        addField(group, "用户名", displayValue(info.redis.dbUser));
        addField(group, "密码", info.redis.password.isEmpty() ? QStringLiteral("未配置") : QStringLiteral("••••••"));
        addField(group, "端口 / 串口", displayValue(info.redis.port));
    }
    {
        auto *group = new QStandardItem("桌面读卡器");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "启用", displayValue(info.reader.isEnable));
        addField(group, "槽位", displayValue(info.reader.slot));
        addField(group, "接口类型", displayValue(info.reader.apiType));
        addField(group, "串口", displayValue(info.reader.comPort));
        addField(group, "动态库", displayValue(info.reader.apiFileName));
    }
    {
        auto *group = new QStandardItem("天线");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "启用", displayValue(info.rsu.isEnable));
        addField(group, "IP 地址", displayValue(info.rsu.ip));
        addField(group, "功率", displayValue(info.rsu.power));
        addField(group, "端口 / 串口", displayValue(info.rsu.port));
    }
    {
        auto *group = new QStandardItem("抓拍相机");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "启用", displayValue(info.capture.isEnable));
        addField(group, "IP 地址", displayValue(info.capture.ip));
        addField(group, "站点 HEX 编码", displayValue(info.capture.stationHex));
        addField(group, "站名称", displayValue(info.capture.stationName));
        addField(group, "车道号", displayValue(info.capture.laneID));
        addField(group, "端口 / 串口", displayValue(info.capture.port));
        addField(group, "触发模式", displayValue(info.capture.triggerMode));
        addField(group, "类型", displayValue(info.capture.type));
    }
    {
        auto *group = new QStandardItem("发卡机");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "启用", displayValue(info.cardRobot.isEnable));
        addField(group, "IP 地址", displayValue(info.cardRobot.ip));
        addField(group, "端口 / 串口", displayValue(info.cardRobot.port));
        addField(group, "上读卡器串口", displayValue(info.cardRobot.upReaderPort));
        addField(group, "下读卡器串口", displayValue(info.cardRobot.dnReaderPort));
        addField(group, "读卡器动态库", displayValue(info.cardRobot.readerApiFileName));
        addField(group, "厂家代码", displayValue(info.cardRobot.producer));
        addField(group, "读卡器槽位", displayValue(info.cardRobot.readerSlot));
        addField(group, "读卡器接口类型", displayValue(info.cardRobot.readerApiType));
        addField(group, "波特率", displayValue(info.cardRobot.baudRate));
        addField(group, "选项", displayValue(info.cardRobot.option));
        addField(group, "上下工位共享读卡器", displayValue(info.cardRobot.isShareReader));
    }
    {
        auto *group = new QStandardItem("字符叠加器");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "启用", displayValue(info.overlap.isEnable));
        addField(group, "串口", displayValue(info.overlap.comPort));
        addField(group, "类型", displayValue(info.overlap.type));
        addField(group, "波特率", displayValue(info.overlap.baudRate));
    }
    {
        auto *group = new QStandardItem("称重设备");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "启用", displayValue(info.weight.isEnable));
        addField(group, "串口", displayValue(info.weight.comPort));
        addField(group, "地址", displayValue(info.weight.addr));
        addField(group, "波特率", displayValue(info.weight.baudRate));
        addField(group, "称重协议 IWP", displayValue(info.weight.IWP));
    }
    {
        auto *group = new QStandardItem("费额显示器");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "启用", displayValue(info.feeScr.isEnable));
        addField(group, "端口 / 串口", displayValue(info.feeScr.port));
        addField(group, "波特率", displayValue(info.feeScr.baudRate));
        addField(group, "等待时间（毫秒）", displayValue(info.feeScr.sleepMillisecond));
        addField(group, "类型", displayValue(info.feeScr.type));
    }
    {
        auto *group = new QStandardItem("智能网关");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "启用", displayValue(info.smartLaneCtrl.isEnable));
        addField(group, "IP 地址", displayValue(info.smartLaneCtrl.ip));
        addField(group, "端口 / 串口", displayValue(info.smartLaneCtrl.port));
    }
    {
        auto *group = new QStandardItem("车控器");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "启用", displayValue(info.switchDev.isEnable));
        addField(group, "基地址", displayValue(info.switchDev.baseAddr));
        addField(group, "入口线圈", displayValue(info.switchDev.beginCoil));
        addField(group, "顶棚灯", displayValue(info.switchDev.ceilingLamp));
        addField(group, "线圈 1", displayValue(info.switchDev.coil1));
        addField(group, "线圈 2", displayValue(info.switchDev.coil2));
        addField(group, "线圈 3", displayValue(info.switchDev.coil3));
        addField(group, "线圈 4", displayValue(info.switchDev.coil4));
        addField(group, "出口线圈", displayValue(info.switchDev.endCoil));
        addField(group, "通行灯", displayValue(info.switchDev.passingLamp));
        addField(group, "落杆", displayValue(info.switchDev.railingDown1));
        addField(group, "抬杆", displayValue(info.switchDev.railingUp1));
        addField(group, "报警", displayValue(info.switchDev.warning));
        addField(group, "类型", displayValue(info.switchDev.type));
        addField(group, "设备编号", displayValue(info.switchDev.which));
        addField(group, "线圈数量", displayValue(info.switchDev.coilNum));
    }
    {
        auto *group = new QStandardItem("车型识别器");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "启用", displayValue(info.recognizer.isEnable));
        addField(group, "IP 地址", displayValue(info.recognizer.ip));
        addField(group, "端口 / 串口", displayValue(info.recognizer.port));
    }
    {
        auto *group = new QStandardItem("缴费机");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "启用", displayValue(info.payRobot.isEnable));
        addField(group, "IP 地址", displayValue(info.payRobot.ip));
        addField(group, "端口 / 串口", displayValue(info.payRobot.port));
        addField(group, "上读卡器串口", displayValue(info.payRobot.upReaderPort));
        addField(group, "下读卡器串口", displayValue(info.payRobot.dnReaderPort));
        addField(group, "读卡器动态库", displayValue(info.payRobot.readerApiFileName));
        addField(group, "厂家代码", displayValue(info.payRobot.producer));
        addField(group, "读卡器槽位", displayValue(info.payRobot.readerSlot));
        addField(group, "读卡器接口类型", displayValue(info.payRobot.readerApiType));
        addField(group, "上下工位共享读卡器", displayValue(info.payRobot.isShareReader));
    }
    {
        auto *group = new QStandardItem("折叠情报板");
        m_infoModel->appendRow({group, new QStandardItem()});
        addField(group, "启用", displayValue(info.infoboard.isEnable));
        addField(group, "类型", displayValue(info.infoboard.type));
        addField(group, "IP 地址", displayValue(info.infoboard.ip));
        addField(group, "端口 / 串口", displayValue(info.infoboard.port));
    }
}
