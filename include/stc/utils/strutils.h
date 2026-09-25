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
 * 字符串解码器
 */
class StrDecoder {
public:
    /**
     * 解析一个字符，返回码点和字符数
     * @param iter 字符串迭代器
     * @return (unicode码点, 字符字节数)
     */
    virtual std::tuple<char32_t, size_t> decode(std::string_view::const_iterator iter) const;

    virtual ~StrDecoder() = default;
};

/**
 * utf8 解码器
 */
class UTF8Decoder : public StrDecoder {
public:
    std::tuple<char32_t, size_t> decode(std::string_view::const_iterator iter) const override;
};

/**
 * utf16 解码器
 */
class UTF16Decoder : public StrDecoder {
public:
    std::tuple<char32_t, size_t> decode(std::string_view::const_iterator iter) const override;
};

/**
 * 计算行开始索引映射表
 * @param content 进行分段字符串的片段
 * @return 行开始索引表
 */
std::vector<std::uint32_t> get_line_start_indices(std::string_view content);

} // namespace stc::utils

#endif // STC_STRUTILS_H