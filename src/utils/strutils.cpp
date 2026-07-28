/**
 * @file strutils.cpp
 * @author anishan
 * @date 2026/7/28
 */

#include "stc/utils/strutils.h"


namespace stc::utils {
/**
 * 计算行开始索引映射表
 * @param content 进行分段字符串的片段
 * @return 行开始索引表
 */
std::vector<std::uint32_t> get_line_start_indices(const std::string_view content) {
    enum class State: std::uint8_t {
        LineStart,
        CR, // \r
        Normal
    };
    // todo 重构一下，无需用状态机
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
            if (content[i] == '\n') {
                i++;
            }
            state = State::LineStart;
            break;
        case State::Normal:
            if (content[i] == '\r') {
                state = State::CR;
            } else if (content[i] == '\n') {
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

} // namespace stc::utils