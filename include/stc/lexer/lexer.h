/**
 * @file lexer.h
 * @author anishan
 * @date 2026/6/12
 * lexer 只负责提交文本返回 token，不过可以提供跳过 token 的基础功能（用于错误恢复）
 */

#ifndef STC_LEXER_H
#define STC_LEXER_H
#include "token.h"

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
        MaybeKeywordOrIdent,
        /// 可能是宏
        MaybeMacro,
        /// 错误状态，指针不会推进，只会返回 Invalid Token
        Invalid,
        /// 结束状态
        Eof,
    };

    enum class CommentKind: std::uint8_t {
        NotComment,
        SingleLineComment,
        MultiLineComment,
    };


    /**
     * 判断是否是新行
     */
    [[nodiscard]]
    bool is_new_line() const;

    [[nodiscard]]
    CommentKind get_comment_type_() const;

    [[nodiscard]]
    bool is_comment_end_(CommentKind kind) const;

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
    Token handle_keyword_or_ident_();

    /**
     * 宏处理函数
     */
    Token handle_macro_();

    /**
     * 移动迭代器，计算一个utf8码点，维护一个索引位置，不允许对已经结束的迭代器操作
     * @return 返回码点
     */
    char32_t next_();

    /**
     * peek 当前字符，当迭代器结束返回 zero 字符
     */
    [[nodiscard]]
    char32_t peek_() const;

    /**
     * peek 两个字符，当迭代器结束返回 zero 字符
     */
    [[nodiscard]]
    std::array<char32_t, 2> peek2_() const;

    /**
     * 跳过空白字符
     */
    void skip_white_space();

    /**
     * 跳过注释处理函数
     */
    void skip_comment_();


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
