/**
 * @file token_stream.c
 * @author anishan
 * @date 2026/6/13
 */

#include "stc/lexer/token_stream.h"

namespace stc::lexer {
TokenStream::TokenStream(const std::shared_ptr<source::Manager>& source_manager) : source_manager{
    source_manager} {
}
}

