#pragma once

#include <algorithm>
#include <QString>

#define QTC_WIN_EXE_SUFFIX ".exe"

namespace Utils {

enum OsType { OsTypeWindows, OsTypeLinux, OsTypeMac, OsTypeOtherUnix, OsTypeOther };

namespace OsSpecificAspects {

// 根据传入的操作系统类型 osType，给可执行文件名添加后缀（Windows 加 .exe，其他系统不加）
inline QString withExecutableSuffix(OsType osType, const QString &executable)
{
    QString finalName = executable;
    if (osType == OsTypeWindows)
        finalName += QLatin1String(QTC_WIN_EXE_SUFFIX);
    return finalName;
}

// 返回对应系统的文件名大小写敏感规则：
// Windows 和 macOS 默认大小写不敏感（Qt::CaseInsensitive）
// Linux 和其他 Unix 默认大小写敏感（Qt::CaseSensitive）
inline Qt::CaseSensitivity fileNameCaseSensitivity(OsType osType)
{
    return osType == OsTypeWindows || osType == OsTypeMac ? Qt::CaseInsensitive : Qt::CaseSensitive;
}

// 获取路径列表分隔符
// Windows 用分号 ';' 分隔路径列表（如环境变量 PATH），其他系统用冒号 ':'
inline QChar pathListSeparator(OsType osType)
{
    return QLatin1Char(osType == OsTypeWindows ? ';' : ':');
}

//  获取“控制键”修饰符，Windows 和 Linux 用Ctrl键，
inline Qt::KeyboardModifier controlModifier(OsType osType)
{
    return osType == OsTypeMac ? Qt::MetaModifier : Qt::ControlModifier;
}

// 路径字符串格式转换（native分隔符）
// 如果是 Windows，将路径中的斜杠替换为反斜杠。其他平台不做改动，保持原样
inline QString pathWithNativeSeparators(OsType osType, const QString &pathName)
{
    if (osType == OsTypeWindows) {
        const int pos = pathName.indexOf('/');
        if (pos >= 0) {
            QString n = pathName;
            std::replace(std::begin(n) + pos, std::end(n), '/', '\\');
            return n;
        }
    }
    return pathName;
}

} // namespace OsSpecificAspects
} // namespace Utils
