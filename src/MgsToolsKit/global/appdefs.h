#pragma once

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
