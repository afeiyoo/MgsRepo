#pragma once

namespace API {
namespace EXCHANGE_SHIFT {
static const int REQUEST = 1;         // 交接班请求
static const int END_SHIFT_QUERY = 2; // 下班询问
} // namespace EXCHANGE_SHIFT

namespace SYSTEM_QUIT {
static const int REQUEST = 3; // 请求退出
static const int QUERY = 4;   // 退出询问
} // namespace SYSTEM_QUIT

namespace TEST_CAP {
static const int REQUEST = 5;
}

namespace INIT {
static const int REQUEST = 6; // 请求初始化
}

} // namespace API
