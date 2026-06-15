#pragma once

#include <QObject>

#include "etc_global.h"

class ETC_EXPORT ETC : public QObject
{
    Q_OBJECT
public:
    explicit ETC(QObject *parent = nullptr);
    ~ETC() override;

    int init(int argc, char *argv[]);

public slots:
    void onKeyPress(int key);
};
