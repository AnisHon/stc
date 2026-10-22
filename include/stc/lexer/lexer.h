/**
 * @file lexer.h
 * @author anishan
 * @date 2026/6/12
 */

#ifndef STC_LEXER_H
#define STC_LEXER_H

#include "pp_token.h"
#include <cstdint>

namespace stc {

/**
 * 'std::string_view::const_iterator' is too long
 */
using SVIter = std::string_view::const_iterator;

enum class LexerError : std::uint8_t {
    Invalid,
    CommentUnclosed,
    CharUnclosed,
    ExpectHex,
    UnknownEscape,
};

/**
 * The lexer assumes a UTF-8 encoded, null-terminated input stream.
 * And the end iterator must be valid and point to the trailing \0.
 */
class Lexer {
public:
    Lexer(StringInterner *interner, std::string_view source);

    /**
     * 获取下一个token，可能出现 EOF InvalidToken，
     * 注意返回的Invalid Token是没有具体位置信息的，需要使用 recover 跳过提供一个位置信息
     * @return 下一个token
     */
    PPToken next_token();

private:
    /// 驻留池
    StringInterner *interner_;

    /// 当前位置迭代器 TODO 重命名 curr_iter_
    SVIter curr_iter_;

    /// 起始位置迭代器
    const SVIter source_beg_;

    /// 尾后迭代器
    const SVIter source_end_;

    bool leading_space_;

    bool start_of_line_;
};

} // namespace stc

#endif // STC_LEXER_H
