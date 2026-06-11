#pragma once

#include <QAbstractListModel>
#include <QWidget>

#include "lanesystemgui_global.h"

class WeightInfoModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit WeightInfoModel(QObject *parent = nullptr);
    ~WeightInfoModel() override;

    enum WeightInfoRoles { PlateRole = Qt::UserRole + 1, AxisTypeRole, AxisNumRole, WeightRole, StatusRole };

    void setItems(const QList<ST_WeightInfoItem> &items);
    void appendItem(const ST_WeightInfoItem &item);
    void removeItem(const QModelIndex &index);
    void clearItem();
    void updateItem(const QModelIndex &index, const ST_WeightInfoItem &item);
    void updateItem(const QModelIndex &index, uint status);

    int getRowByIndex(int index);

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

private:
    void showUpdateCurWeightInfo(ST_WeightInfoItem &item);

private:
    QList<ST_WeightInfoItem> m_dataList;
};

class ElaListView;
class WeightInfoPanel : public QWidget
{
    Q_OBJECT
public:
    explicit WeightInfoPanel(QWidget *parent = nullptr);

    ~WeightInfoPanel() override;

    void appendItem(const ST_WeightInfoItem &item);
    void removeItem(int row);
    void updateItem(int row, const ST_WeightInfoItem &item);
    void updateItem(uint status);
    void clearItem();

private:
    void initUi();

private:
    ElaListView *m_weightInfoView = nullptr;
    WeightInfoModel *m_weightInfoModel = nullptr;
};
