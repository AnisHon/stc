/**
 * @file token_stream.h
 * @author anishan
 * @date 2026/6/13
 * token stream 用于协调宏解析和 lexer 进行，将 lexer 与预处理阶段揉在一起，负责维护宏状态
 */

#ifndef STCC_TOKEN_STREAM_H
#define STCC_TOKEN_STREAM_H

#include <stack>

#include "directive.h"

namespace stcc {

class TokenStream {
public:
private:
    std::stack<pp::Directive> pp_stack_;
};

} // namespace stcc

#endif // STCC_TOKEN_STREAM_H
