#pragma once

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
