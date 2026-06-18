/**
 * @file strutils.h
 * @author Anis Han
 * @date 2026/6/15
 * 字符串工具
 */

#ifndef STC_STRUTILS_H
#define STC_STRUTILS_H
#include "macros.h"

#include <vector>

namespace stc::utils {

/**
 * 计算行开始索引映射表
 * @param content 进行分段字符串的片段
 * @return 行开始索引表
 */
inline std::vector<std::uint32_t> get_line_start_indices(const std::u8string_view content) {
    enum class State {
        LineStart,
        CR, // \r
        Normal
    };
    std::vector<std::uint32_t> line_starts;
    auto state = State::LineStart;
    size_t i = 0;

    while (i < content.size()) {
        switch (state) {
        case State::LineStart:
            line_starts.push_back(i);
            state = State::Normal;
            break;
        case State::CR:
            // CRLF \r\n
            if (content[i] == u8'\n') {
                i++;
            }
            state = State::LineStart;
            break;
        case State::Normal:
            if (content[i] == u8'\r') {
                state = State::CR;
            } else if (content[i] == u8'\n') {
                state = State::LineStart;
            } else {
                state = State::Normal;
            }
            i++;
            break;
        default:
            UNREACHABLE();
        }
    }
    return line_starts;
}

inline bool is_start_char(const char32_t chr) {
    TODO("未实现");
}
}

#endif //STC_STRUTILS_H
