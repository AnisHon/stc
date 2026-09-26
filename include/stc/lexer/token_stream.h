/**
 * @file token_stream.h
 * @author anishan
 * @date 2026/6/13
 * token stream 用于协调宏解析和 lexer 进行，将 lexer 与预处理阶段揉在一起，负责维护宏状态
 */

#ifndef STC_TOKEN_STREAM_H
#define STC_TOKEN_STREAM_H

#include <memory>
#include <stack>

#include "stc/source/source_manager.h"
#include "directive.h"


namespace stc {

class TokenStream {
public:
    explicit TokenStream(const std::shared_ptr<SourceManager>& source_manager);

private:
    std::shared_ptr<SourceManager> source_manager_;
    std::stack<pp::Directive> pp_stack_;
};


}

#endif //STC_TOKEN_STREAM_H
