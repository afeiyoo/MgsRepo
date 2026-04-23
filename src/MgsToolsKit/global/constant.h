#pragma once

namespace Constant {

//日志常量
namespace Log {
const char FORMAT[] = "[%{time}{yyyy-MM-dd HH:mm:ss.zzz}] [%{type}] [%{threadid}] [%{file}:%{line}] | %{message}\n\n";
const int MAX_SAVE_DAY = 90;
} // namespace Log

} // namespace Constant
