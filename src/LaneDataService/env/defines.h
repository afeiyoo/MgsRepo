#pragma once

#include <QObject>
#include <QUrl>

// 全量状态只描述当前处理阶段或最近一次失败原因。
// 全量当前是否可查询，以ST_EnvSnap::isFullBlackValid为准。
enum EM_FullBlackStatus {
    FullBlackReady = 0,            // 当前全量已加载，可正常使用
    FullBlackChecking = 1,         // 正在检查本地、远程全量清单
    FullBlackDownloading = 2,      // 正在下载远程全量
    FullBlackVerifying = 3,        // 正在校验远程全量
    FullBlackPublishing = 4,       // 正在发布已校验的远程全量
    FullBlackCheckFailed = -1,     // 本地、远程清单均无效
    FullBlackLocalBatchAhead = -2, // 本地批次高于远程权威批次，禁止加载本地全量
    FullBlackLocalLoadFailed = -3, // 本地全量加载失败且无法远程恢复，可用性按原值处理
    FullBlackDownloadFailed = -4,  // 远程全量下载失败
    FullBlackVerifyFailed = -5,    // 远程全量完整性或数据库校验失败
    FullBlackPublishFailed = -6    // 远程全量发布失败
};

struct ST_EnvSnap
{
    bool isDeltaBlackValid;    // 当前增量是否可用
    int deltaBlackStatus = -1; // 增量状态
    QString deltaBlackVersion; // 增量版本

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
