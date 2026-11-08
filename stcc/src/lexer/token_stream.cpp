/**
 * @file token_stream.cpp
 * @author anishan
 * @date 2026/6/13
 */

#include "stcc/lexer/token_stream.h"

namespace stcc {
TokenStream::TokenStream(const std::shared_ptr<SourceManager> &source_manager)
    : source_manager_{source_manager} {
}
} // namespace stcc
