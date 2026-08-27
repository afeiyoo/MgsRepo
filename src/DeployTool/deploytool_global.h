#pragma once

#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN)
    #if defined(DEPLOYTOOL_DYNAMIC)
        #define DEPLOYTOOL_EXPORT Q_DECL_EXPORT // 构建动态库
    #elif defined(DEPLOYTOOL_STATIC)
        #define DEPLOYTOOL_EXPORT
    #else
        #define DEPLOYTOOL_EXPORT Q_DECL_IMPORT // 使用动态库
    #endif
#else
    #define DEPLOYTOOL_EXPORT
#endif
