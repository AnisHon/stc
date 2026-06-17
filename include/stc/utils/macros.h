/**
 * @file macros.h
 * @author anishan
 * @date 2026/6/13
 * 简单的宏工具类
 */

#ifndef STC_TODO_H
#define STC_TODO_H
#include <stdexcept>
#include <fmt/core.h>

/**
 * 类似 rust 的一个 to_do! 宏，执行后会抛出一个异常。
 * @param msg 可选，报错信息
 */
#define TODO(msg, ...)                                          \
    throw std::runtime_error(                                   \
        fmt::format("not yet implemented: " msg ", at {}:{}",   \
        ##__VA_ARGS__, __FILE__, __LINE__))


/**
 * 类似 rust 的一个 unreachable! 宏，执行后会抛出一个异常。
 * @param msg 可选，报错信息
 */
#define UNREACHABLE(msg, ...)                                       \
    throw std::logic_error(                                         \
        fmt::format("entered unreachable code:" msg ", at {}:{}",   \
        ##__VA_ARGS__, __FILE__, __LINE__))

/**
 * 实现一个类似 rust 的 try! 宏的兼容性较差，且不直观。
 * 观望，保留。
 */
#define TRY throw std::logic_error("Macro TRY is not implemented yet")

#endif //STC_TODO_H
