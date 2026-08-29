#pragma once

#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN)
    #if defined(VEHRECOGNIZER_DYNAMIC)
        #define VEHRECOGNIZER_EXPORT Q_DECL_EXPORT // 构建动态库
    #elif defined(VEHRECOGNIZER_STATIC)
        #define VEHRECOGNIZER_EXPORT
    #else
        #define VEHRECOGNIZER_EXPORT Q_DECL_IMPORT // 使用动态库
    #endif
#else
    #define VEHRECOGNIZER_EXPORT
#endif
