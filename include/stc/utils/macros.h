/**
 * @file macros.h
 * @author anishan
 * @date 2026/6/13
 * 简单的宏工具类
 */

#pragma once

#include <source_location>
#include <string_view>
#include <utility>
#include <fmt/format.h>
#include <fmt/color.h>

namespace stc::utils::inner_ {

/**
 * panic报错，并进行 backtrace 打印
 * @param kind 错误原因，比如 ASSERTION FAILED
 * @param condition 出错条件，可以没有
 * @param message 出错信息，可以没有
 * @param loc 出错源码位置
 * @param skip 栈打印skip
 */
[[noreturn]]
void panic_(std::string_view kind,
            std::string_view condition,
            std::string_view message,
            const std::source_location& loc,
            uint32_t skip = 1);


// ============================================================
// ASSERT
// ============================================================

[[noreturn]]
inline void assert_failed(
    const std::string_view condition,
    const std::source_location& loc
    ) {
    panic_(
        "ASSERTION FAILED",
        condition,
        "",
        loc,
        2
        );
}

template <typename... Args>
[[noreturn]]
void assert_failed(
    const std::string_view condition,
    const std::source_location& loc,
    fmt::format_string<Args...> fmtstr,
    Args&&... args
    ) {
    inner_::panic_(
        "ASSERTION FAILED",
        condition,
        fmt::format(
            fmtstr,
            std::forward<Args>(args)...
            ),
        loc,
        2
        );
}

// ============================================================
// TO.DO
// ============================================================

[[noreturn]]
inline void todo(
    const std::source_location& loc
    ) {
    panic_(
        "TODO",
        "",
        "",
        loc,
        2
        );
}

template <typename... Args>
[[noreturn]]
void todo(
    const std::source_location& loc,
    fmt::format_string<Args...> fmtstr,
    Args&&... args
    ) {
    inner_::panic_(
        "TODO",
        "",
        fmt::format(
            fmtstr,
            std::forward<Args>(args)...
            ),
        loc,
        2
        );
}

// ============================================================
// UNREACHABLE
// ============================================================

[[noreturn]]
inline void unreachable(
    const std::source_location& loc
    ) {
    panic_(
        "UNREACHABLE",
        "",
        "",
        loc,
        2
        );
}

template <typename... Args>
[[noreturn]]
void unreachable(
    const std::source_location& loc,
    fmt::format_string<Args...> fmtstr,
    Args&&... args
    ) {
    inner_::panic_(
        "UNREACHABLE",
        "",
        fmt::format(
            fmtstr,
            std::forward<Args>(args)...
            ),
        loc,
        2
        );

}

// ============================================================
// PANIC
// ============================================================

[[noreturn]]
inline void panic(
    const std::source_location& loc
    ) {
    panic_(
        "PANIC",
        "",
        "",
        loc,
        2
        );
}

template <typename... Args>
[[noreturn]]
void panic(
    const std::source_location& loc,
    fmt::format_string<Args...> fmtstr,
    Args&&... args
    ) {
    inner_::panic_(
        "PANIC",
        "",
        fmt::format(
            fmtstr,
            std::forward<Args>(args)...
            ),
        loc,
        2
        );

}


} // namespace stc::utils::inner_


// ============================================================
// Macros
// ============================================================

#ifndef NDEBUG

#define ASSERT(expr, ...)                                            \
    do {                                                             \
        if (!(expr)) {                                               \
            stc::utils::inner_::assert_failed(                       \
                #expr,                                               \
                std::source_location::current()                      \
                __VA_OPT__(,) __VA_ARGS__                            \
            );                                                       \
        }                                                            \
    } while(false)

#else

#define ASSERT(expr, ...) ((void)0)

#endif

#define TODO(...)                                                    \
     stc::utils::inner_::todo(                                       \
        std::source_location::current()                              \
        __VA_OPT__(,) __VA_ARGS__                                    \
    )

#define UNREACHABLE(...)                                             \
     stc::utils::inner_::unreachable(                                \
        std::source_location::current()                              \
        __VA_OPT__(,) __VA_ARGS__                                    \
    )

#define PANIC(...)                                                   \
     stc::utils::inner_::panic(                                      \
        std::source_location::current()                              \
        __VA_OPT__(,) __VA_ARGS__                                    \
    )
