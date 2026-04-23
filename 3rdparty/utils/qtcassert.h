#pragma once

namespace Utils {
void writeAssertLocation(const char *msg);
}

// 宏参数字符串化：传进来的参数 x 变成字符串文字 "x"
#define QTC_ASSERT_STRINGIFY_HELPER(x) #x
#define QTC_ASSERT_STRINGIFY(x) QTC_ASSERT_STRINGIFY_HELPER(x)

// 构造一条断言信息字符串
#define QTC_ASSERT_STRING(cond) \
    ::Utils::writeAssertLocation("\"" cond "\" in file " __FILE__ \
                                 ", line " QTC_ASSERT_STRINGIFY(__LINE__))

// 可自定义行为的断言宏，例如
// QTC_ASSERT(ptr != nullptr, return);       // 断言失败就 return
// QTC_ASSERT(vec.size() == 3, exit(1));     // 断言失败就退出程序
#define QTC_ASSERT(cond, action) \
    if (Q_LIKELY(cond)) { \
    } else { \
        QTC_ASSERT_STRING(#cond); \
        action; \
    } \
    do { \
    } while (0)

// 与 QTC_ASSERT 一样，但没有 action，只是打印断言失败信息，不终止程序也不退出。
#define QTC_CHECK(cond) \
    if (Q_LIKELY(cond)) { \
    } else { \
        QTC_ASSERT_STRING(#cond); \
    } \
    do { \
    } while (0)

// 这是一个布尔值表达式形式的断言宏，适合写在条件语句中。
#define QTC_GUARD(cond) ((Q_LIKELY(cond)) ? true : (QTC_ASSERT_STRING(#cond), false))
