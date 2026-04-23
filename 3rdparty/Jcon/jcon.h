#pragma once

#include <QtGlobal>

// Use the generic helper definitions above to define JCON_API, which is used
// for the public API symbols. It either DLL imports or DLL exports (or does
// nothing for static build).

#if defined(Q_OS_WIN)
    #if defined(JCON_DYNAMIC) // defined if JCON is compiled as a DLL
        #define JCON_API Q_DECL_EXPORT
    #elif defined(JCON_STATIC) // JCON_DLL is not defined: this means JCON is a static lib.
        #define JCON_API
    #else
        #define JCON_API Q_DECL_IMPORT
    #endif
#else
    #define JCON_API
#endif
