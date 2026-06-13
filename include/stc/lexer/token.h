/**
 * @file token.h
 * @author anishan
 * @date 2026/6/11
 * token 及其相关工具函数定义
 */

#ifndef STC_TOKEN_H
#define STC_TOKEN_H

#include <string_view>

#include "stc/source/location.h"
#include "token_kind.h"

namespace stc::lexer {

struct Token {
    TokenKind kind;
    std::string_view lexeme;
    source::Location begin;
    source::Location end;

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

}

#endif //STC_TOKEN_H
