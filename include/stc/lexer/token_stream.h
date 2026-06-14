/**
 * @file token_stream.h
 * @author anishan
 * @date 2026/6/13
 * token stream 用于协调宏解析和 lexer 进行，将 lexer 与预处理阶段揉在一起，负责维护宏状态
 */

#ifndef STC_TOKEN_STREAM_H
#define STC_TOKEN_STREAM_H

#include "stc/preprocessor/directive.h"

#include <memory>
#include <stack>
#include "stc/source/manager.h"


namespace stc::lexer {

class TokenStream {
public:
    explicit TokenStream(const std::shared_ptr<source::Manager>& source_manager);

private:
    std::shared_ptr<source::Manager> _source_manager;
    std::stack<preprocessor::Directive> _pp_stack;
};


}

#endif //STC_TOKEN_STREAM_H
