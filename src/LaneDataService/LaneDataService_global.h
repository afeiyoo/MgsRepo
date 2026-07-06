#pragma once

#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN)
    #if defined(LANEDATASERVICE_DYNAMIC)
        #define LANEDATASERVICE_EXPORT Q_DECL_EXPORT
    #elif defined(LANEDATASERVICE_STATIC)
        #define LANEDATASERVICE_EXPORT
    #else
        #define LANEDATASERVICE_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define LANEDATASERVICE_EXPORT
#endif
