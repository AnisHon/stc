/**
 * @file lexer.h
 * @author anishan
 * @date 2026/6/12
 * lexer 只负责提交文本返回 token，不过可以提供跳过 token 的基础功能（用于错误恢复）
 */

#ifndef STC_LEXER_H
#define STC_LEXER_H
#include "token.h"
#include "utf8cpp/utf8.h"

namespace stc::lexer {

/**
 * 经典的Lexer，简单直接
 * Lexer必须要求错误utf8本身无错误，必须先检查一遍
 */
class Lexer {
    /**
     * Lexer是一个状态机，这是状态
     */
    enum class State: std::uint8_t {
        /// 可能是字符串或标识符
        MaybeIdent,
        /// 可能是宏或运算符
        MaybePunctuator,
        /// 可能是数字常量
        MaybeNumberConstant,
        /// 可能是字符串
        MaybeString,
        /// 可能是字符
        MaybeChar,
        /// 错误状态，指针不会推进，只会返回 Invalid Token
        Invalid,
        /// 结束状态
        Eof,
    };

    /**
     * 注释类型
     */
    enum class CommentType: std::uint8_t {
        NotComment,
        SingleLineComment,
        MultiLineComment,
    };

    /**
     * 换行类型
     */
    enum class NewLineType : std::uint8_t {
        LF, // \n
        CR, // \r
        CRLF, // \r\n
        NotNewLine
    };

    /**
     * 字符类型
     */
    enum class CharType : std::uint8_t {
        NotChar,
        Char,
        /// 平台相关 char 32 或 16
        LongChar,
        Char16,
        Char32,
    };

    /**
     * 字符串类型
     */
    enum class StringType : std::uint8_t {
        NotString,
        String,
        /// 平台相关 string
        LongString,
        U8String,
        U16String,
        U32String,
    };

    /**
     * 判断换行类型
     */
    [[nodiscard]]
    NewLineType is_new_line() const;

    [[nodiscard]]
    CommentType get_comment_type_() const;

    [[nodiscard]]
    bool is_comment_end_(CommentType kind) const;

    [[nodiscard]]
    CharType is_char_constant_start_() const;

    [[nodiscard]]
    StringType is_string_start_() const;

    [[nodiscard]]
    std::u8string_view get_current_view_() const;

    /**
     * 初始状态转移
     * @return 取决于读到的第一个字符
     */
    [[nodiscard]]
    State peek_state() const;

    /**
     * 如果 is_ident_start 成立就是则可以调用这个函数，否则出错
     */
    Token lex_keyword_or_ident_();

    /**
     * 识别 punctuator kind，消耗字符
     */
    TokenKind lex_punctuator_kind_();

    /**
     * 符号处理函数
     */
    Token lex_punctuator_();

    /**
     * 是否到达文件尾
     */
    [[nodiscard]]
    bool is_eof() const;

    /**
     * 移动迭代器，计算一个utf8码点，维护一个索引位置，不允许对已经结束的迭代器操作
     * @return 返回码点
     */
    void consume_();

    template <size_t N>
    void consume_n_() {
        for (size_t i = 0; i < N; ++i) {
            this->consume_();
        }
    }

    /**
     * 如果匹配则消耗
     * @return 是否匹配
     */
    bool match_(char32_t c);

    /**
     * peek 当前字符，当迭代器结束返回 zero 字符
     */
    [[nodiscard]]
    char32_t peek_() const;

    /**
     * peek n 个字符，如果提前结束，所有其他值为 0 字符
     * @tparam N peek 的字符数量
     * @return 返回 peek 序列
     */
    template <std::size_t N>
    [[nodiscard]]
    std::array<char32_t, N> peekn_() const {
        std::array<char32_t, N> res{};
        auto it = this->current_iter_;
        for (std::size_t i = 0; i < N && it != this->source_end_; ++i) {
            res[i] = utf8::next(it, source_end_);
        }
        return res;
    }

    /**
     * 跳过空白字符
     */
    void skip_white_space();

    /**
     * 跳过注释处理函数
     * @return 是否未出错
     */
    bool skip_comment_();

    /**
     * @return 当前位置的loc
     */
    [[nodiscard]]
    source::Location make_current_location_() const;

    /**
     * 计算当前的位置
     */
    [[nodiscard]]
    source::LocationRange make_range_() const;

    /**
      * 利用当前信息构建一个 Token，会移动 prev 指针
      */
    Token make_token_(TokenKind kind);

public:
    explicit Lexer(std::u8string_view source);

    /**
     * 获取下一个token，可能出现 EOF InvalidToken，
     * 注意返回的Invalid Token是没有具体位置信息的，需要使用 recover 跳过提供一个位置信息
     * @return 下一个token
     */
    Token next_token();

private:
    /// 是否是行开始状态
    bool is_start_of_line;

    /// 每次 Token 结束的下一个位置
    std::u8string_view::const_iterator prev_iter_;

    /// 起始位置迭代器
    const std::u8string_view::const_iterator source_beg_;

    /// 当前位置迭代器
    std::u8string_view::const_iterator current_iter_;

    /// 尾后迭代器
    const std::u8string_view::const_iterator source_end_;
};

}

#endif //STC_LEXER_H
