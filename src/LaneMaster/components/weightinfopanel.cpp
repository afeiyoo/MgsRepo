#include "weightinfopanel.h"

#include "ElaWidgetTools/ElaListView.h"
#include "delegate/weightinfodelegate.h"
#include "global/globalmanager.h"
#include "global/signalmanager.h"
#include "global/uiconst.h"

#include <QVBoxLayout>

//==============================================================================
// 模型实现
//==============================================================================
WeightInfoModel::WeightInfoModel(QObject *parent)
    : QAbstractListModel(parent)
{}

WeightInfoModel::~WeightInfoModel() {}

void WeightInfoModel::setItems(const QList<ST_WeightInfoItem> &items)
{
    beginResetModel();
    m_dataList = items;
    endResetModel();

    if (!m_dataList.isEmpty()) {
        ST_WeightInfoItem firstWeight = m_dataList.first();
        showUpdateCurWeightInfo(firstWeight);
    }
    emit GM_SIG->sigShowUpdateWeightCount(m_dataList.size());
}

void WeightInfoModel::appendItem(const ST_WeightInfoItem &item)
{
    bool wasEmpty = m_dataList.isEmpty();

    beginInsertRows(QModelIndex(), m_dataList.size(), m_dataList.size());
    m_dataList.append(item);
    endInsertRows();

    if (wasEmpty) {
        ST_WeightInfoItem firstWeight = m_dataList.first();
        showUpdateCurWeightInfo(firstWeight);
    }
    emit GM_SIG->sigShowUpdateWeightCount(m_dataList.size());
}

void WeightInfoModel::removeItem(const QModelIndex &index)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_dataList.size())
        return;

    bool removeFirst = (index.row() == 0);

    beginRemoveRows(QModelIndex(), index.row(), index.row());
    m_dataList.removeAt(index.row());
    endRemoveRows();

    if (removeFirst) {
        if (!m_dataList.isEmpty()) {
            ST_WeightInfoItem firstWeight = m_dataList.first();
            showUpdateCurWeightInfo(firstWeight);
        } else {
            // 队列为空
            emit GM_SIG->sigShowUpdateCurWeightInfo("");
        }
    }
    emit GM_SIG->sigShowUpdateWeightCount(m_dataList.size());
}

void WeightInfoModel::updateItem(const QModelIndex &index, const ST_WeightInfoItem &item)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_dataList.size())
        return;

    m_dataList[index.row()].index = item.index;
    m_dataList[index.row()].plate = item.plate;
    m_dataList[index.row()].axisType = item.axisType;
    m_dataList[index.row()].axisNum = item.axisNum;
    m_dataList[index.row()].tollWeight = item.tollWeight;
    m_dataList[index.row()].status = item.status;
    m_dataList[index.row()].isManual = item.isManual;
    m_dataList[index.row()].limitWeight = item.limitWeight;
    m_dataList[index.row()].allowPass = item.allowPass;
    m_dataList[index.row()].exceedRate = item.exceedRate;

    emit dataChanged(index, index); // 刷新视图（plate，axisType，axisNum，tollWeight，status）

    if (index.row() == 0) {
        ST_WeightInfoItem firstWeight = m_dataList.first();
        showUpdateCurWeightInfo(firstWeight);
    }
}

void WeightInfoModel::updateItem(const QModelIndex &index, uint status)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_dataList.size())
        return;

    m_dataList[index.row()].status = status;
    emit dataChanged(index, index, {StatusRole}); // 刷新视图（status）
}

int WeightInfoModel::getRowByIndex(int index)
{
    int row = -1;
    for (int i = 0; i < m_dataList.size(); ++i) {
        if (m_dataList[i].index == index) {
            row = i;
            break;
        }
    }

    return row;
}

void WeightInfoModel::clearItem()
{
    beginResetModel();
    m_dataList.clear();
    endResetModel();
}

int WeightInfoModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_dataList.size();
}

QVariant WeightInfoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_dataList.size())
        return QVariant();

    const ST_WeightInfoItem &item = m_dataList[index.row()];
    switch (role) {
    case PlateRole:
        return item.plate;
    case AxisTypeRole:
        return item.axisType;
    case AxisNumRole:
        return item.axisNum;
    case WeightRole:
        return item.tollWeight;
    case StatusRole:
        return item.status;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> WeightInfoModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PlateRole] = "plate";
    roles[AxisTypeRole] = "axisType";
    roles[AxisNumRole] = "axisNum";
    roles[WeightRole] = "tollWeight";
    roles[StatusRole] = "status";

    return roles;
}

void WeightInfoModel::showUpdateCurWeightInfo(ST_WeightInfoItem &item)
{
    if (item.isManual) {
        // 是否手动插入称重数据
        QString info = QString("%1轴型 %2轴 %3吨(手动插入)").arg(item.axisType).arg(item.axisNum).arg(item.tollWeight);
        emit GM_SIG->sigShowUpdateCurWeightInfo(info);
    } else {
        QString info = QString("%1轴型 %2轴 %3吨 轴限%4吨").arg(item.axisType).arg(item.axisNum).arg(item.tollWeight).arg(item.limitWeight);
        if (item.allowPass) {
            info += QString(" 超限0.00%");
            emit GM_SIG->sigShowUpdateCurWeightInfo(info);
        } else {
            if (item.exceedRate > 0) {
                QString excessRatio = QString::number(item.exceedRate * 100, 'f', 2);
                info += QString(" 超限%1%").arg(excessRatio);
            }

            emit GM_SIG->sigShowUpdateCurWeightInfo(info, Color::WARN_TC);
        }
    }
}

//==============================================================================
// 视图实现
//==============================================================================
WeightInfoPanel::WeightInfoPanel(QWidget *parent)
    : QWidget(parent)
{
    initUi();
}

WeightInfoPanel::~WeightInfoPanel() {}

void WeightInfoPanel::appendItem(const ST_WeightInfoItem &item)
{
    if (!m_weightInfoModel)
        return;

    m_weightInfoModel->appendItem(item);
}

void WeightInfoPanel::removeItem(int row)
{
    if (!m_weightInfoModel)
        return;

    QModelIndex index = m_weightInfoModel->index(row, 0);
    m_weightInfoModel->removeItem(index);
}

void WeightInfoPanel::updateItem(int index, const ST_WeightInfoItem &item)
{
    if (!m_weightInfoModel)
        return;

    int row = m_weightInfoModel->getRowByIndex(index);

    QModelIndex modelIndex = m_weightInfoModel->index(row, 0);
    m_weightInfoModel->updateItem(modelIndex, item);
}

void WeightInfoPanel::updateItem(uint status)
{
    if (!m_weightInfoModel)
        return;

    QModelIndex modelIndex = m_weightInfoModel->index(0, 0);
    m_weightInfoModel->updateItem(modelIndex, status);
}

void WeightInfoPanel::clearItem()
{
    if (!m_weightInfoModel)
        return;

    m_weightInfoModel->clearItem();
}

void WeightInfoPanel::initUi()
{
    m_weightInfoView = new ElaListView(this);
    m_weightInfoModel = new WeightInfoModel(this);

    m_weightInfoView->setModel(m_weightInfoModel);
    m_weightInfoView->setItemDelegate(new WeightInfoDelegate(this));

    m_weightInfoView->setSpacing(0);
    m_weightInfoView->setViewMode(QListView::IconMode);
    m_weightInfoView->setFlow(QListView::LeftToRight);
    m_weightInfoView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_weightInfoView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_weightInfoView->setFocusPolicy(Qt::NoFocus);
    m_weightInfoView->setResizeMode(QListView::Adjust);
    m_weightInfoView->setIsTransparent(true);
    m_weightInfoView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_weightInfoView->setDragEnabled(false);
    m_weightInfoView->setWrapping(false); // 禁止自动换行

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_weightInfoView);
}
