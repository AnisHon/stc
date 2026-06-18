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
        /// 初始状态
        Init,
        /// 可能是字符串或标识符
        MaybeKeywordOrIdent,
        /// 错误状态，指针不会推进，只会返回 Invalid Token
        Error,
        /// 结束状态
        Eof,
    };

    /**
     * 初始状态转移
     * @return 取决于读到的第一个字符
     */
    State handle_init_state_();

    /**
     * 移动迭代器，计算一个utf8码点，维护一个索引位置，不允许对已经结束的迭代器操作
     * @return 返回码点
     */
    char32_t next_();

    char32_t peek_() const;

    /**
     * @return 当前位置的loc
     */
    source::Location make_current_location_();

    /**
     * 计算当前的位置，并自动更新 prev_location_
     * @return 返回一个当前到目前为止的一个 LocationRange
     */
    source::LocationRange make_range_();

public:
    Lexer(source::Location start_offset, std::u8string_view source);

    /**
     * 获取下一个token，可能出现 EOF InvalidToken，
     * 注意返回的Invalid Token是没有具体位置信息的，需要使用 recover 跳过提供一个位置信息
     * @return 下一个token
     */
    Token next_token();

private
:
    /// 状态机当前状态
    State current_state_;

    /// 索引开始的偏移量
    const source::Location start_offset_;

    /// 每次 Token 结束的下一个位置
    source::Location prev_location_;

    /// 起始位置迭代器
    const std::u8string_view::const_iterator source_beg_;

    /// 当前位置迭代器
    std::u8string_view::const_iterator source_iter_;

    /// 尾后迭代器
    const std::u8string_view::const_iterator source_end_;
};

}

#endif //STC_LEXER_H
