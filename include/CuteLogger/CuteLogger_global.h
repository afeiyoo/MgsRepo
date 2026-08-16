#pragma once

#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN)
    #if defined(CUTELOGGER_DYNAMIC)
        #define CUTELOGGER_EXPORT Q_DECL_EXPORT
    #elif defined(CUTELOGGER_STATIC)
        #define CUTELOGGER_EXPORT
    #else
        #define CUTELOGGER_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define CUTELOGGER_EXPORT
#endif
