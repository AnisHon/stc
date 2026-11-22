/**
 * @file lexer.h
 * @author anishan
 * @date 2026/6/12
 */

#ifndef STCC_LEXER_H
#define STCC_LEXER_H

#include <expected>
#include <stdint.h>
#include <tuple>

#include "pp_token.h"

namespace stcc {

/**
 * 'std::string_view::const_iterator' is too long
 */
using SVIter = std::string_view::const_iterator;

enum class LexerError : uint8_t {
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
    struct TokenKindResult {
        PPTokenKind kind;
        uint32_t bytes;
    };

    struct SkipSpaceResult {
        uint32_t bytes;
        bool leading_space;
        bool start_of_line;
    };

    enum class CommentKind : uint8_t { Invalid = 0, Line, Block };

    enum class CharStrKind : uint8_t { Invalid = 0, ASCII, Wide, UTF16, UTF32 };

    enum class UCNKind : uint8_t { Invalid = 0, U16, U32 };

    /**
     * trigraph 字符映射
     */
    static constexpr char map_trigraph(char chr) noexcept;

    /**
     * lex Trigraph, unify Line-Ending(LF, CR, CRLF -> LF)
     */
    std::tuple<char, uint32_t> peek_chr_slow(SVIter iter) const;
    std::tuple<char, uint32_t> peek_chr(SVIter iter) const;

    /**
     * @return (comment kind, comment start bytes)
     */
    std::tuple<CommentKind, uint32_t> check_comment(SVIter iter) const;
    /**
     * @return (char kind, char start bytes)
     */
    std::tuple<CharStrKind, uint32_t> check_char(SVIter iter) const;
    /**
     * @return (string kind, string start bytes)
     */
    std::tuple<CharStrKind, uint32_t> check_string(SVIter iter) const;
    /**
     * '[0-9]' or '.[0-9]'
     * pp-number grammar
     * - first => '[0-9]'  '.'
     * - first-1 => '.[0-9]'  '[0-9](e|E|[0-9])'  '0x'  '0X'
     */
    uint32_t check_pp_number(SVIter iter) const;
    /**
     * 不负责UCN
     */
    static uint32_t check_ident(SVIter iter);
    static UCNKind check_ucn(SVIter iter);
    uint32_t scan_ident(SVIter iter) const;
    TokenKindResult scan_punctuator(SVIter iter) const;
    std::expected<TokenKindResult, LexerError> scan_pp_number(SVIter iter) const;
    std::expected<uint32_t, LexerError> scan_ucn(SVIter iter, UCNKind kind) const;
    uint32_t scan_line_comment(SVIter iter, SVIter end) const;
    std::expected<uint32_t, LexerError> scan_block_comment(SVIter iter, SVIter end) const;
    std::expected<uint32_t, LexerError> scan_comment(
        SVIter iter,
        SVIter end,
        CommentKind kind
    ) const;
    std::expected<uint32_t, LexerError> scan_string(SVIter iter, SVIter end) const;
    std::expected<TokenKindResult, LexerError> sacn_token_kind(SVIter iter, SVIter end) const;
    SkipSpaceResult skip_space(SVIter iter) const;

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

} // namespace stcc

#endif // STCC_LEXER_H
