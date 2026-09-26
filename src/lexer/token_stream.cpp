/**
 * @file token_stream.cpp
 * @author anishan
 * @date 2026/6/13
 */

#include "stc/lexer/token_stream.h"

namespace stc {
TokenStream::TokenStream(const std::shared_ptr<SourceManager>& source_manager) : source_manager_{
    source_manager} {
}
}
