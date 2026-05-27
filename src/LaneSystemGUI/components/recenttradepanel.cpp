#include "recenttradepanel.h"

#include "global/uiconst.h"
#include <QHeaderView>
#include <QPalette>
#include <QTableView>
#include <QVBoxLayout>

//==============================================================================
// 模型实现
//==============================================================================
RecentTradeModel::RecentTradeModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

RecentTradeModel::~RecentTradeModel() {}

int RecentTradeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_tradeList.count();
}

int RecentTradeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_header.count();
}

QVariant RecentTradeModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        return m_tradeList[index.row()][index.column()];
    } else if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }
    return QVariant();
}

QVariant RecentTradeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return m_header[section];
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

void RecentTradeModel::setHeader(QStringList header)
{
    m_header = header;
    if (!m_header.isEmpty()) {
        emit headerDataChanged(Qt::Horizontal, 0, m_header.count() - 1);
    }
}

void RecentTradeModel::appendTrade(QStringList trade)
{
    if (trade.size() != m_header.size())
        return;

    beginInsertRows(QModelIndex(), m_tradeList.count(), m_tradeList.count());
    m_tradeList.append(trade);
    endInsertRows();
}

void RecentTradeModel::clearTrades()
{
    beginResetModel();
    m_tradeList.clear();
    endResetModel();
}

//==============================================================================
// 视图实现
//==============================================================================
RecentTradePanel::RecentTradePanel(QStringList header, PageArea *parent)
    : PageArea{parent}
{
    setBorderRadius(8);
    setBackgroundColor(Color::CUSTOM_AREA_BG);

    initUi();
    m_recentTradeModel = new RecentTradeModel(this);
    m_recentTradeModel->setHeader(header);
    m_recentTradeView->setModel(m_recentTradeModel);
}

RecentTradePanel::~RecentTradePanel() {}

void RecentTradePanel::appendTrade(QStringList trade)
{
    m_recentTradeModel->appendTrade(trade);
    // 自动滚动到最后一行
    int lastRow = m_recentTradeModel->rowCount() - 1;
    if (lastRow >= 0) {
        m_recentTradeView->scrollToBottom();
    }
}

void RecentTradePanel::clearTrades()
{
    m_recentTradeModel->clearTrades();
}

void RecentTradePanel::initUi()
{
    m_recentTradeView = new QTableView(this);

    // 表头字体
    QFont headerFont = m_recentTradeView->horizontalHeader()->font();
    headerFont.setWeight(QFont::DemiBold);
    headerFont.setPixelSize(14);
    m_recentTradeView->horizontalHeader()->setFont(headerFont);

    // 表头行为
    m_recentTradeView->horizontalHeader()->setStretchLastSection(true);
    m_recentTradeView->horizontalHeader()->setSectionsMovable(false);
    m_recentTradeView->horizontalHeader()->setSectionsClickable(false);
    m_recentTradeView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // 索引列行为
    m_recentTradeView->verticalHeader()->setVisible(false);

    // 视图字体
    QFont tableFont = m_recentTradeView->font();
    tableFont.setPixelSize(14);
    m_recentTradeView->setFont(tableFont);

    // 视图行为
    m_recentTradeView->setSelectionMode(QAbstractItemView::NoSelection);
    m_recentTradeView->setFocusPolicy(Qt::NoFocus);
    m_recentTradeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_recentTradeView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_recentTradeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_recentTradeView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_recentTradeView->setShowGrid(true);           // 启用表格线
    m_recentTradeView->setGridStyle(Qt::SolidLine); // 使用实线

    m_recentTradeView->viewport()->setAutoFillBackground(false);
    m_recentTradeView->horizontalHeader()->setStyleSheet(
        QString("QHeaderView { background-color: #D1D1D1; border: none; border-top-left-radius: 8px; border-top-right-radius: 8px; }"
                "QHeaderView::section { background-color: transparent; border: none; padding: 0px; }"));
    m_recentTradeView->setStyleSheet(QString("QTableView { gridline-color: #D1D1D1; background-color: transparent; border: none; }"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(1, 1, 0, 0);
    layout->addWidget(m_recentTradeView);
}

QTableView *RecentTradePanel::getRecentTradeView() const
{
    return m_recentTradeView;
}
