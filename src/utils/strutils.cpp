/**
 * @file strutils.cpp
 * @author anishan
 * @date 2026/7/28
 */

#include "stc/utils/strutils.h"

#include <utf8/unchecked.h>

namespace stc::utils {

std::tuple<char32_t, size_t> UTF8Decoder::decode(std::string_view::const_iterator iter) const {
    const auto original_iter = iter;
    const char32_t cp = utf8::unchecked::next(iter);
    const size_t bytes = std::distance(original_iter, iter);
    return {cp, bytes};
}

std::tuple<char32_t, size_t> UTF16Decoder::decode(std::string_view::const_iterator iter) const {
    const char32_t cp = utf8::unchecked::next16(iter);
    return {cp, 2};
}

/**
 * 计算行开始索引映射表
 * @param content 进行分段字符串的片段
 * @return 行开始索引表
 */
std::vector<std::uint32_t> get_line_start_indices(const std::string_view content) {
    std::vector<std::uint32_t> line_starts;
    // 文本的第一个字符永远是第一行的开始（即使内容为空，通常也认为存在起始偏移0）
    line_starts.push_back(0);

    size_t pos = 0;
    while ((pos = content.find_first_of("\r\n", pos)) != std::string_view::npos) {
        if (content[pos] == '\r' && pos + 1 < content.size() && content[pos + 1] == '\n') {
            pos += 2;
        } else {
            pos += 1;
        }
        line_starts.push_back(static_cast<std::uint32_t>(pos));
    }

    return line_starts;
}

} // namespace stc::utils
