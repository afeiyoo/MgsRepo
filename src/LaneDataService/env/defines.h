#pragma once

#include <QObject>

struct ST_EnvSnap
{
    bool isGrowthBlackVersion;  // 当前增量是否可用
    QString growthBlackVersion; // 增量版本

    bool isFullBlackValid = false; // 当前全量是否可用
    int fullBlackStatus = -1;      // 全量状态
    QString fullBlackVersion;      // 全量版本
};
