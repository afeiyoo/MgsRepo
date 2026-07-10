#pragma once

#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN)
    #if defined(SERVICEHUB_DYNAMIC)
        #define SERVICEHUB_EXPORT Q_DECL_EXPORT
    #elif defined(SERVICEHUB_STATIC)
        #define SERVICEHUB_EXPORT
    #else
        #define SERVICEHUB_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define SERVICEHUB_EXPORT
#endif
