#pragma once

#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN)
    #if defined(MOBILEPLUSTERMINAL_DYNAMIC)
        #define MOBILEPLUSTERMINAL_EXPORT Q_DECL_EXPORT
    #elif defined(MOBILEPLUSTERMINAL_STATIC)
        #define MOBILEPLUSTERMINAL_EXPORT
    #else
        #define MOBILEPLUSTERMINAL_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define MOBILEPLUSTERMINAL_EXPORT
#endif
