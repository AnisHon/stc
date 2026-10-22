#include "lexer_test_helper.h"
#include "stc/lexer/lexer.h"
#include "stc/lexer/pp_token.h"
#include "stc/lexer/string_interner.h"
#include "stc/lexer/token_kinds.h"
#include <vector>

namespace {

using stc::Lexer, stc::PPTokenKind, stc::PPToken;

std::vector<stc::PPTokenKind> code2pptoken(std::string code) {
    std::vector<PPTokenKind> res;
    stc::StringInterner internal;
    code.push_back('\0');
    Lexer lexer(&internal, code);

    
    PPToken tok = lexer.next_token(); 
    while (tok.kind != PPTokenKind::Invalid || tok.kind != PPTokenKind::Eof) {
        res.push_back(tok.kind);
        tok = lexer.next_token();
    }

    return res;
}

} // namespace
