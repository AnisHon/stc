/**
 * @file lexer.c
 * @author anishan
 * @date 2026/6/13
 */

#include "stc/lexer/lexer.h"
#include "stc/utils/macros.h"

namespace stc::lexer {
Lexer::Lexer(const uint32_t start_offset,
             const std::u8string_view::const_iterator _source_iter) : start_offset_{start_offset},
    source_iter_{_source_iter} {

}

Token Lexer::next_token() {
    TODO();
}

}