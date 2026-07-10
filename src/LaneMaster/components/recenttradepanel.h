#pragma once

#include "components/pagearea.h"
#include <QAbstractTableModel>
#include <QTableView>

class RecentTradeModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit RecentTradeModel(QObject *parent = nullptr);
    ~RecentTradeModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setHeader(QStringList header);
    void appendTrade(QStringList trade);
    void clearTrades();

private:
    QStringList m_header;
    QList<QStringList> m_tradeList;
};

class RecentTradePanel : public PageArea
{
    Q_OBJECT
public:
    explicit RecentTradePanel(QStringList header, PageArea *parent = nullptr);

    ~RecentTradePanel() override;

    void appendTrade(QStringList trade);

    void clearTrades();

    QTableView *getRecentTradeView() const;

private:
    void initUi();

private:
    QTableView *m_recentTradeView = nullptr;
    RecentTradeModel *m_recentTradeModel = nullptr;
};
