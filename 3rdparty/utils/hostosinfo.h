#pragma once

#include "osspecificaspects.h"

#include <QString>

#ifdef Q_OS_WIN
#define QTC_HOST_EXE_SUFFIX QTC_WIN_EXE_SUFFIX
#else
#define QTC_HOST_EXE_SUFFIX ""
#endif // Q_OS_WIN

namespace Utils {

class HostOsInfo
{
public:
    // 返回当前主机的操作系统类型
    static constexpr OsType hostOs()
    {
#if defined(Q_OS_WIN)
        return OsTypeWindows;
#elif defined(Q_OS_LINUX)
        return OsTypeLinux;
#elif defined(Q_OS_MAC)
        return OsTypeMac;
#elif defined(Q_OS_UNIX)
        return OsTypeOtherUnix;
#else
        return OsTypeOther;
#endif
    }

    enum HostArchitecture {
        HostArchitectureX86,
        HostArchitectureAMD64,
        HostArchitectureItanium,
        HostArchitectureArm,
        HostArchitectureUnknown
    };
    // 返回当前主机 CPU 架构
    static HostArchitecture hostArchitecture();

    // 判断当前系统是不是特定操作系统（Windows、Linux、macOS）
    static constexpr bool isWindowsHost() { return hostOs() == OsTypeWindows; }
    static constexpr bool isLinuxHost() { return hostOs() == OsTypeLinux; }
    static constexpr bool isMacHost() { return hostOs() == OsTypeMac; }
    static constexpr bool isAnyUnixHost()
    {
#ifdef Q_OS_UNIX
        return true;
#else
        return false;
#endif
    }

    // 根据当前系统，返回带有正确后缀的可执行文件名。Windows：添加 .exe；其他系统：不添加后缀
    static QString withExecutableSuffix(const QString &executable);

    // 允许临时覆盖当前平台的文件名大小写敏感
    static void setOverrideFileNameCaseSensitivity(Qt::CaseSensitivity sensitivity);
    // 取消设置的大小写覆盖规则，恢复默认行为（即自动根据操作系统判断）
    static void unsetOverrideFileNameCaseSensitivity();

    // 返回当前平台的文件名是否大小写敏感
    // Windows，Mac默认不敏感；Linux，Unix默认敏感
    // 如果调用setOverrideFileNameCaseSensitivity()，则返回设置的值
    static Qt::CaseSensitivity fileNameCaseSensitivity();

    // 返回当前系统的环境变量中路径分隔符
    static QChar pathListSeparator();

    // 返回当前平台上的“控制键”修饰符（常用于快捷键）
    static Qt::KeyboardModifier controlModifier();

private:
    static Qt::CaseSensitivity m_overrideFileNameCaseSensitivity;
    static bool m_useOverrideFileNameCaseSensitivity;
};

} // namespace Utils
