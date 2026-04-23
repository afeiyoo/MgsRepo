#pragma once

#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN)
    #if defined(SMARTLANECONTROLLER_DYNAMIC)
        #define SMARTLANECONTROLLER_EXPORT Q_DECL_EXPORT // 构建动态库
    #elif defined(SMARTLANECONTROLLER_STATIC)
        #define SMARTLANECONTROLLER_EXPORT
    #else
        #define SMARTLANECONTROLLER_EXPORT Q_DECL_IMPORT // 使用动态库
    #endif
#else
    #define SMARTLANECONTROLLER_EXPORT
#endif
