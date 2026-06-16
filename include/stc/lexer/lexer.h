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

class Lexer {

    /**
     * Lexer是一个状态机，这是状态
     */
    enum class State {
        /// 初始状态
        Init,
        /// 错误状态，指针不会推进，只会返回 Invalid Token
        Error,
    };

private:


public:
    Lexer(uint32_t start_offset, std::u8string_view::const_iterator _source_iter);

    Token next_token();

private:
    State current_state_;
    const uint32_t start_offset_;
    std::u8string_view::const_iterator source_iter_;
};

}

#endif //STC_LEXER_H
