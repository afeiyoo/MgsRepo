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

struct ST_ConfigSnap
{
    uint dbType;        // 数据库类型
    QString dbHost;     // 数据库IP
    QString dbName;     // 数据库名称
    QString dbUser;     // 数据库用户名
    QString dbPassword; // 数据库密码
    uint dbPort;        // 数据库端口

    QString logFormat;   // 日志输出格式
    int logLimits = 180; // 日志最大保存天数

    QStringList sqlFiles; // sql文件存储路径

    QString fullBlackPath; // 全量文件所在路径
    int fullBatchNo = 0;   // 当前全量批次

    QString stationServiceURL; // 站级服务URL
};
