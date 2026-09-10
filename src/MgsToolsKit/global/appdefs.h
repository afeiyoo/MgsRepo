#pragma once

#include "ideploytool.h"
#include <QObject>
#include <QtGlobal>

struct ST_CardRobotStatusInfo
{
    uchar boxStatus;
    uchar hasCardBag;
    ushort restCard;
    uchar hasAnnCard;
};
Q_DECLARE_METATYPE(ST_CardRobotStatusInfo);

// 部署工具
namespace DeployToolDef {
struct ST_DeployInfoLoadResult
{
    ST_DeployInfo info;
    QString error;
};

struct ST_DeployResult
{
    bool success = false;
    QString message;
    QStringList logs;
};

const QStringList deploymentNames = {"收费软件初始化", "Dtp初始化", "Start123初始化", "费率同步", "环境初始化"};
} // namespace DeployToolDef
Q_DECLARE_METATYPE(DeployToolDef::ST_DeployInfoLoadResult)
Q_DECLARE_METATYPE(DeployToolDef::ST_DeployResult)
