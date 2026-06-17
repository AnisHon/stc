/**
 * @file token.h
 * @author anishan
 * @date 2026/6/11
 * token 及其相关工具函数定义
 */

#ifndef STC_TOKEN_H
#define STC_TOKEN_H

#include "stc/source/location.h"
#include "token_kind.h"
#include "string_interner.h"

namespace stc::lexer {

/**
 * Token 数据结构，设计为不可变数据类型
 */
struct Token {
    /// Token 的种类
    const TokenKind kind;

    /// 词素
    const Lexeme lexeme;

    /// 位置
    const source::LocationRange range;


    [[nodiscard]] constexpr bool is_keyword() const {
        return lexer::is_keyword(this->kind);
    }

    [[nodiscard]] constexpr bool is_literal() const {
        return lexer::is_literal(this->kind);
    }

    [[nodiscard]] constexpr bool is_assignment_op() const {
        return lexer::is_assignment_op(this->kind);
    }
};

/**
 * 返回一个无效token
 * @return 无效Token
 */
constexpr Token invalid_token() {
    return {TokenKind::Invalid, null_lexeme(), source::zero_range()};
}


}

#endif //STC_TOKEN_H
