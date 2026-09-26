/**
 * @file pp_token.h
 * @author anishan
 * @date 2026/6/11
 * token 及其相关工具函数定义
 */

#ifndef STC_TOKEN_H
#define STC_TOKEN_H

#include "stc/source/source_location.h"
#include "token_kinds.h"
#include "string_interner.h"

namespace stc {

/**
 * PPToken 数据结构，设计为不可变数据类型
 */
struct PPToken {

    /**
     * 返回一个无效token
     * @return 无效Token
     */
    constexpr static PPToken invalid() {
        return {.kind = PPTokenKind::Invalid, .lexeme = null_lexeme(),
                .range = SourceRange::invalid()};
    }

    /// PPToken 的种类
    const PPTokenKind kind;

    /// 词素
    const Lexeme lexeme;

    /// 位置
    const SourceRange range;

    ///
    const bool leading_space;

    /// 是否是行开始
    const bool start_of_line;

};


}

#endif //STC_TOKEN_H
