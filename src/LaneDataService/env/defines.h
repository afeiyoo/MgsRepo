#pragma once

#include <QObject>
#include <QUrl>

// 全量状态只描述最近一次检查或更新的最终结果。
// 全量当前是否可查询，以ST_EnvSnap::isFullBlackValid为准。
enum EM_FullBlackStatus {
    FullBlackReady = 0,            // 最近一次检查或更新成功
    FullBlackCheckFailed = -1,     // 远程权威清单不可用，无法确认当前全量批次
    FullBlackDownloadFailed = -4,  // 远程全量下载失败
    FullBlackVerifyFailed = -5,    // 远程全量完整性或数据库校验失败
    FullBlackPublishFailed = -6,   // 远程全量发布失败
    FullBlackCleanupFailed = -7    // 候选全量指定的增量表清理失败
};

// 增量状态只描述当前处理阶段或最近一次失败原因。
// 增量是否已由站级确认追平最新版本，以ST_EnvSnap::isDeltaBlackValid为准。
enum EM_DeltaBlackStatus {
    DeltaBlackReady = 0,                // 已追平站级当前增量版本
    DeltaBlackWaitingForCheck = 1,      // 数据库已就绪，等待首次检查
    DeltaBlackApplying = 2,             // 正在保存并连续追赶增量版本
    DeltaBlackDBUnavailable = -1,       // 增量SQLite不可访问或结构无效
    DeltaBlackBaselineUnavailable = -2, // 无法确定增量请求基线
    DeltaBlackRequestFailed = -3,       // 向站级请求增量失败
    DeltaBlackResponseInvalid = -4,     // 响应内容无效或目标表不匹配
    DeltaBlackApplyFailed = -5          // 清表、保存或事务提交失败
};

struct ST_EnvSnap
{
    bool isDeltaBlackValid = false;                 // 最近一次向站级检查时是否已确认追平最新增量版本
    int deltaBlackStatus = DeltaBlackDBUnavailable; // 当前处理阶段或最近一次失败原因
    QString deltaBlackVersion;                      // 最近一次成功提交的增量版本

    bool isFullBlackValid = false;              // 当前全量是否可用
    int fullBlackStatus = FullBlackCheckFailed; // 全量状态
    QString fullBlackVersion;                   // 全量版本
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

    QString fullBlackPath;  // 全量文件所在路径
    QString deltaBlackPath; // 增量文件所在路径

    QString stationServiceURL; // 站级服务URL
};

struct ST_FullSliceInfo
{
    int index = 0;
    QString fileName;
    QString md5;
    QUrl url;
};

struct ST_FullManifest
{
    int batchNo = 0;
    QString totalMd5;
    QList<ST_FullSliceInfo> slices;
    QByteArray rawXml;
};
