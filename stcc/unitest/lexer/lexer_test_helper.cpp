#include "lexer_test_helper.h"
#include "stcc/lexer/lexer.h"
#include "stcc/lexer/pp_token.h"
#include "stcc/lexer/string_interner.h"
#include "stcc/lexer/token_kinds.h"

#include <spdlog/logger.h>
#include <vector>

namespace {

using stcc::Lexer, stcc::PPTokenKind, stcc::PPToken;

std::vector<stcc::PPTokenKind> code2pptoken(std::string code) {
    std::vector<PPTokenKind> res;
    stcc::StringInterner internal;
    spdlog::logger log{"lexer-test"};
    code.push_back('\0');
    Lexer lexer(&internal, code, log);

    PPToken tok = lexer.next_token();
    while (tok.kind != PPTokenKind::Invalid || tok.kind != PPTokenKind::Eof) {
        res.push_back(tok.kind);
        tok = lexer.next_token();
    }

    return res;
}

} // namespace
