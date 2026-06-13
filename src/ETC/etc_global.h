#pragma once

#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN)
    #if defined(ETC_DYNAMIC)
        #define ETC_EXPORT Q_DECL_EXPORT
    #elif defined(ETC_STATIC)
        #define ETC_EXPORT
    #else
        #define ETC_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define ETC_EXPORT
#endif
