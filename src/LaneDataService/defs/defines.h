#pragma once

#include <QObject>

// 全量状态
struct ST_FullBlackStatus
{
    bool isValid = false;
    int lastCheckStatus = -1; // 上一次检查全量状态
    QString activeVersion;    // 当前使用全量版本
};
Q_DECLARE_METATYPE(ST_FullBlackStatus);

// 配置快照
struct ST_ConfigSnapshot
{
    int fullBatchNo = 0;
};
