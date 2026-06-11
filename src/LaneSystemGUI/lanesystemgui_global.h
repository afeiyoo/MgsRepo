#pragma once

#include <QObject>
#include <QString>
#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN)
    #if defined(LANESYSTEMGUI_DYNAMIC)
        #define LANESYSTEMGUI_EXPORT Q_DECL_EXPORT
    #elif defined(LANESYSTEMGUI_STATIC)
        #define LANESYSTEMGUI_EXPORT
    #else
        #define LANESYSTEMGUI_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define LANESYSTEMGUI_EXPORT
#endif

// 界面称重信息结构体
struct ST_WeightInfoItem
{
    int index = 0;
    QString plate;
    uint axisType = 0;
    uint axisNum = 0;
    qreal tollWeight = 0.0;
    uint status = 0;
    bool isManual = false;
    qreal limitWeight = 0.0;
    bool allowPass = false;
    qreal exceedRate = 0.0;
};
