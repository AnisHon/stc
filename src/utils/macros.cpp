/**
 * @file macros.cpp
 * @author Anis Han
 * @date 2026/6/18
 */

#include <source_location>
#include <string_view>
#include <thread>
#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/std.h>
#include <cpptrace/cpptrace.hpp>

#include "stc/utils/macros.h"


[[noreturn]]
void stc::utils::inner_::panic_(
    std::string_view kind,
    std::string_view condition,
    std::string_view message,
    const std::source_location& loc,
    const uint32_t skip
    ) {
    // 大标题
    fmt::print(
        stderr,
        fg(fmt::color::red) | fmt::emphasis::bold,
        "\n========================================\n"
        "{}\n"
        "========================================\n",
        kind
        );

    if (!condition.empty()) {
        fmt::print(
            stderr,
            "Condition : {}\n",
            condition
            );
    }

    const auto tid = std::this_thread::get_id();
    fmt::print(
        stderr,
        "thread '{}'({}) panicked at {}:{}:\n",
        loc.function_name(),
        tid,
        loc.file_name(),
        loc.line()
        );

    if (!message.empty()) {
        fmt::println(stderr, "{}", message);
    }

    // 打印栈
    fmt::println(stderr, "stack backtrace:");
    size_t index = 0;

    for (auto trace : cpptrace::generate_trace(skip)) {
        fmt::println(stderr, "    {}: {}", index, trace.symbol);
        const std::uint32_t line = trace.line.value_or(0);
        const std::uint32_t column = trace.column.value_or(0);
        fmt::println(stderr, "            at {}:{}:{}", trace.filename, line, column);
        index++;
    }
    std::abort();
}
