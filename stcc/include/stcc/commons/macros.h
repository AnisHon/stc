/**
 * @file macros.h
 * @author anishan
 * @date 2026/6/13
 * 简单的宏工具类
 */

#ifndef STCC_MACROS_H
#define STCC_MACROS_H

#include <expected>
#include <source_location>
#include <stdint.h>
#include <string_view>
#include <utility>
#include <fmt/format.h>
#include <fmt/color.h>

namespace stcc::utils::detail {

/**
 * 输出诊断和调用栈，然后终止进程。
 * @param kind 错误原因，比如 ASSERTION FAILED
 * @param condition 出错条件，可以没有
 * @param message 出错信息，可以没有
 * @param loc 出错源码位置
 * @param skip 栈打印skip
 */
[[noreturn]]
void panic(
    std::string_view kind,
    std::string_view condition,
    std::string_view message,
    const std::source_location &loc,
    uint32_t skip = 1
);

// ============================================================
// ASSERT
// ============================================================

[[noreturn]]
inline void assert_failed(const std::string_view condition, const std::source_location &loc) {
    ::stcc::utils::detail::panic("ASSERTION FAILED", condition, "", loc, 2);
}

template <typename... Args>
[[noreturn]]
void assert_failed(
    const std::string_view condition,
    const std::source_location &loc,
    fmt::format_string<Args...> fmtstr,
    Args &&...args
) {
    ::stcc::utils::detail::panic(
        "ASSERTION FAILED",
        condition,
        fmt::format(fmtstr, std::forward<Args>(args)...),
        loc,
        2
    );
}

// ============================================================
// TO.DO
// ============================================================

[[noreturn]]
inline void todo(const std::source_location &loc) {
    ::stcc::utils::detail::panic("TODO", "", "", loc, 2);
}

template <typename... Args>
[[noreturn]]
void todo(const std::source_location &loc, fmt::format_string<Args...> fmtstr, Args &&...args) {
    ::stcc::utils::detail::panic(
        "TODO",
        "",
        fmt::format(fmtstr, std::forward<Args>(args)...),
        loc,
        2
    );
}

// ============================================================
// UNREACHABLE
// ============================================================

[[noreturn]]
inline void unreachable(const std::source_location &loc) {
    ::stcc::utils::detail::panic("UNREACHABLE", "", "", loc, 2);
}

template <typename... Args>
[[noreturn]]
void unreachable(
    const std::source_location &loc,
    fmt::format_string<Args...> fmtstr,
    Args &&...args
) {
    ::stcc::utils::detail::panic(
        "UNREACHABLE",
        "",
        fmt::format(fmtstr, std::forward<Args>(args)...),
        loc,
        2
    );
}

// ============================================================
// PANIC
// ============================================================

[[noreturn]]
inline void panic_at(const std::source_location &loc) {
    ::stcc::utils::detail::panic("PANIC", "", "", loc, 2);
}

template <typename... Args>
[[noreturn]]
void panic_at(const std::source_location &loc, fmt::format_string<Args...> fmtstr, Args &&...args) {
    ::stcc::utils::detail::panic(
        "PANIC",
        "",
        fmt::format(fmtstr, std::forward<Args>(args)...),
        loc,
        2
    );
}

} // namespace stcc::utils::detail

// ============================================================
// Macros
// ============================================================

#ifndef NDEBUG

#  define ASSERT(expr, ...)                                                    \
      do {                                                                     \
          if (!(expr)) {                                                       \
              ::stcc::utils::detail::assert_failed(                            \
                  #expr,                                                       \
                  ::std::source_location::current() __VA_OPT__(, ) __VA_ARGS__ \
              );                                                               \
          }                                                                    \
      } while (false)

#else

#  define ASSERT(expr, ...) ((void)0)

#endif

#define TODO(...) \
    ::stcc::utils::detail::todo(::std::source_location::current() __VA_OPT__(, ) __VA_ARGS__)

#define UNREACHABLE(...) \
    ::stcc::utils::detail::unreachable(::std::source_location::current() __VA_OPT__(, ) __VA_ARGS__)

#define PANIC(...) \
    ::stcc::utils::detail::panic_at(::std::source_location::current() __VA_OPT__(, ) __VA_ARGS__)

/**
 * 对 std::expected<T, E> 表达式求值一次。成功时继续执行。
 * 失败时返回 std::unexpected<E>，当前函数可以返回 std::expected<U, E>。
 */
#define TRY_ERR(...)                                                                 \
    do {                                                                             \
        if (auto &&stcc_try_result_ = (__VA_ARGS__); !stcc_try_result_) {            \
            return ::std::unexpected{                                                \
                ::std::forward<decltype(stcc_try_result_)>(stcc_try_result_).error() \
            };                                                                       \
        }                                                                            \
    } while (false)

#define TRY(...) TRY_ERR(__VA_ARGS__)

#endif // STCC_MACROS_H
