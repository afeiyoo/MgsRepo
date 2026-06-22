#pragma once

#include "global/etcdefs.h"
#include <QList>
#include <QObject>

class Environment : public QObject
{
    Q_OBJECT
public:
    explicit Environment(QObject *parent = nullptr);
    ~Environment() override;

public:
    bool m_isInShifted = false; // 是否已登班

    QString m_curCapVehPlate;      // 当前抓拍车牌
    QString m_curCapImageFilePath; // 当前抓拍文件路径

    QList<ST_TradeInfo> m_tradeList; // 交易缓存队列
};
