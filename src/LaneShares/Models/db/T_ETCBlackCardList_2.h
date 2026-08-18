#pragma once

#include <QDateTime>
#include <QObject>

class T_ETCBlackCardList_2 : public QObject
{
    Q_OBJECT
public:
    explicit T_ETCBlackCardList_2(QObject *parent = nullptr)
        : QObject{parent}
    {
        tbl_pk = "InsertTime,CreationTime,BlackType,CardID,Status";
    }

public:
    QString tbl_pk;
    Q_PROPERTY(QString tbl_pk MEMBER tbl_pk)
    QString auto_inc;
    Q_PROPERTY(QString auto_inc MEMBER auto_inc)
    QDateTime InsertTime;
    Q_PROPERTY(QDateTime InsertTime MEMBER InsertTime)
    QString IssuerID;
    Q_PROPERTY(QString IssuerID MEMBER IssuerID)
    QDateTime CreationTime;
    Q_PROPERTY(QDateTime CreationTime MEMBER CreationTime)
    int BlackType;
    Q_PROPERTY(int BlackType MEMBER BlackType)
    QString CardID;
    Q_PROPERTY(QString CardID MEMBER CardID)
    int Status;
    Q_PROPERTY(int Status MEMBER Status)
    QDateTime UpdateTime;
    Q_PROPERTY(QDateTime UpdateTime MEMBER UpdateTime)
};
