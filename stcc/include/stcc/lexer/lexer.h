/**
 * @file lexer.h
 * @author anishan
 * @date 2026/6/12
 */

#ifndef STCC_LEXER_H
#define STCC_LEXER_H

#include <expected>
#include <optional>
#include <spdlog/logger.h>
#include <stdint.h>
#include <string_view>

#include "stcc/lexer/lex_opt.h"
#include "stcc/lexer/pp_token.h"
#include "stcc/utils/char_utils.h"

namespace stcc {

/// 指向源码视图中字节的只读迭代器。
using SourceIter = std::string_view::const_iterator;

enum class LexerError : uint8_t {
    Invalid,
    CommentUnclosed,
    CharUnclosed,
    StringUnclosed,
    ExpectHex,
    UnknownEscape,
};

/**
 * The lexer assumes a UTF-8 encoded, null-terminated input stream.
 * And the end iterator must be valid and point to the trailing \0.
 */
class Lexer {
public:
    /**
     * 借用源码、驻留池和日志记录器。三者须在 Lexer 使用期间保持有效。
     */
    Lexer(StringInterner *interner, std::string_view source, spdlog::logger &log);

    /**
     * 获取下一个token，可能出现 EOF InvalidToken，
     * 注意返回的Invalid Token是没有具体位置信息的，需要使用 recover 跳过提供一个位置信息
     * @return 下一个token
     */
    PPToken next_token();

private:
    struct SkipSpaceResult {
        SourceIter iter;
        bool leading_space;
        bool start_of_line;
    };


    struct TokenSpec {
        SourceIter scan_end;

        /// PPToken 的种类
        PPTokenKind kind;

        ///
        bool leading_space;

        /// 是否是行开始
        bool start_of_line;
    };

    /// 注释的起始符类型。


    enum class CommentKind : uint8_t { Invalid = 0, Line, Block };

    enum class CharStrKind : uint8_t { Invalid = 0, ASCII, Wide, UTF16, UTF32 };

    enum class UCNKind : uint8_t { Invalid = 0, U16, U32 };

    template <typename T>
    struct PeekResult {
        T value;
        /// 结果对应的源字节数。
        uint32_t bytes;
    };

    template <typename T>
    struct LexResult {
        T value;
        /// 匹配成功时指向匹配部分之后，未匹配时指向原位置。
        SourceIter iter;
    };

    /**
     * trigraph 字符映射
     */
    static constexpr char map_trigraph(char chr) noexcept;

    /**
     * 获取到下一个换行之前的字符长度
     * @param iter escape 字符之后的迭代器
     * @return 返回0表示没有newline转译，返回非零表示续行
     */
    uint32_t peek_newline_size_after_escape(SourceIter iter) const;

    /**
     * 处理可能涉及反斜杠或三字符组的前瞻，返回字符和对应的源字节数。
     * TODO 将 CR、CRLF 和 LF 统一为 LF。
     */
    PeekResult<char> peek_chr_slow(SourceIter iter) const;

    PeekResult<char> peek_chr(const SourceIter iter) const;

    LexResult<char> next_chr_slow(SourceIter iter) const;

    /// 返回当前位置的字符和读取后的迭代器，不移动 Lexer 的游标。
    LexResult<char> next_chr(SourceIter iter) const;

    /// 在已知输入为有效 UTF-8 时，解码当前位置的码点并返回其源字节数。
    LexResult<char32_t> next_codepoint(SourceIter iter) const noexcept;

    /// 识别行注释或块注释的起始符。
    LexResult<CommentKind> check_comment(SourceIter iter) const;

    /// 识别字符字面量的可选编码前缀和起始单引号。
    LexResult<CharStrKind> check_char(SourceIter iter) const;

    /// 识别字符串字面量的可选编码前缀和起始双引号。
    LexResult<CharStrKind> check_string(SourceIter iter) const;

    /**
     * '[0-9]' or '.[0-9]'
     * pp-number grammar
     * - first => '[0-9]'  '.'
     * - first-1 => '.[0-9]'  '[0-9](e|E|[0-9])'  '0x'  '0X'
     */
    LexResult<bool> check_pp_number(SourceIter iter) const;
    /**
     * 不负责UCN
     */
    LexResult<bool> check_ident(SourceIter iter) const;

    /// 识别通用字符名的 `\u` 或 `\U` 前缀。
    static LexResult<UCNKind> check_ucn(SourceIter iter);

    /// 扫描标识符起始字符之后的部分，返回标识符种类和结束位置。
    LexResult<PPTokenKind> scan_ident(SourceIter iter) const;

    /// 扫描标点符号，返回其种类和结束位置。
    LexResult<PPTokenKind> scan_punctuator(SourceIter iter) const;

    /// 扫描预处理数字起始部分之后的内容。
    std::optional<LexResult<PPTokenKind>> scan_pp_number(SourceIter curr) const;

    /**
     * 从起始分隔符之后扫描头文件名。尖括号形式的内容不含换行或 `<`，
     * 双引号形式的内容不含换行或 `"`。
     */
    std::optional<LexResult<PPTokenKind>> scan_header_name(SourceIter iter) const;

    /// 检查恰好 Num 位十六进制数字，其中 Num 只能为 4 或 8。
    template <size_t Num>
        requires(Num == 4 || Num == 8)
    bool scan_hex_qual(const SourceIter iter) const {
        this->log_.trace("scan {} hex", Num);

        SourceIter curr{iter};
        for (size_t i = 0; i < Num; ++i, ++curr) {
            const auto character{this->next_chr(curr)};

            if (!utils::is_xdigit(character.value)) {
                return false;
            }
            curr = character.iter;
        }
        return true;
    }

    /// 从前缀之后扫描连续的通用字符名，返回扫描结束的位置。
    std::optional<LexResult<PPTokenKind>> scan_ucn(SourceIter iter, UCNKind kind) const;

    /// 从 `//` 之后扫描到换行符或文件末尾。
    LexResult<PPTokenKind> scan_line_comment(SourceIter iter) const;

    /// 从 `/*` 之后扫描块注释，遇到文件末尾仍未闭合时返回错误。
    std::optional<LexResult<PPTokenKind>> scan_block_comment(SourceIter iter) const;

    /// 根据已识别的注释类型，从起始符之后继续扫描。
    std::optional<LexResult<PPTokenKind>> scan_comment(
        SourceIter iter,
        const CommentKind kind
        ) const;

    /// 从起始单引号之后扫描字符字面量。
    std::optional<LexResult<PPTokenKind>> scan_char(SourceIter iter) const;

    /// 从起始双引号之后扫描字符串字面量。
    std::optional<LexResult<PPTokenKind>> scan_string(SourceIter iter) const;

    /// 识别当前位置的预处理 token。扫描失败时返回 std::nullopt。
    std::optional<LexResult<PPTokenKind>> scan_token_kind(SourceIter iter) const;

    /// 统计当前位置的空白，记录是否有前导空白以及后续 token 是否位于行首。
    SkipSpaceResult skip_space(SourceIter iter) const;

    PPToken make_token(TokenSpec token_spec) const;

    spdlog::logger &log_;

    const LexerOptions *options_{nullptr};

    /// 驻留池
    StringInterner *interner_;

    /// 当前位置迭代器 TODO 重命名 curr_iter_
    SourceIter curr_iter_;

    /// 起始位置迭代器
    const SourceIter source_beg_;

    /// 尾后迭代器
    const SourceIter source_end_;

    bool leading_space_;

    bool start_of_line_;

    bool enable_header_name_;
};

} // namespace stcc

#endif // STCC_LEXER_H