/**
 * @file pp_token.h
 * @author anishan
 * @date 2026/6/11
 * token 及其相关工具函数定义
 */

#ifndef STC_TOKEN_H
#define STC_TOKEN_H

#include "stc/source/location.h"
#include "token_kinds.h"
#include "string_interner.h"

namespace stc::lexer {

/**
 * PPToken 数据结构，设计为不可变数据类型
 */
struct PPToken {
    /// PPToken 的种类
    const PPTokenKind kind;

    /// 词素
    const Lexeme lexeme;

    /// 位置
    const source::LocationRange range;
};

/**
 * 返回一个无效token
 * @return 无效Token
 */
constexpr PPToken invalid_token() {
    return {PPTokenKind::Invalid, null_lexeme(), source::zero_range()};
}

}

#endif //STC_TOKEN_H
