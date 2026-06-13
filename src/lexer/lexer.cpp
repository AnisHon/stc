/**
 * @file lexer.c
 * @author anishan
 * @date 2026/6/13
 */

#include <stc/lexer/lexer.h>

#include "stc/utils/todo.h"

namespace stc::lexer {
Lexer::Lexer(const uint32_t start_offset, const std::u8string_view::const_iterator _source_iter) :
    _start_offset{start_offset}, _source_iter{_source_iter} {

}

Token Lexer::next_token() {
    TODO();
}

}