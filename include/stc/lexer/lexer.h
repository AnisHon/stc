/**
 * @file lexer.h
 * @author anishan
 * @date 2026/6/12
 * lexer 只负责提交文本返回 token，不过可以提供跳过 token 的基础功能（用于错误恢复）
 */

#ifndef STC_LEXER_H
#define STC_LEXER_H
#include "token.h"

#include <memory>

namespace stc::lexer {

class Lexer {
public:
    Lexer(uint32_t start_offset, std::u8string_view::const_iterator _source_iter);
    Token next_token();

private:
    const uint32_t _start_offset;
    std::u8string_view::const_iterator _source_iter;
};

}

#endif //STC_LEXER_H
