#include "stcc/lexer/token_kinds.h"
#include "stcc/commons/macros.h"

namespace stcc {

bool is_preprocessor(const PPTokenKind k) {
    switch (k) {
#define STCC_PP_PUNCTUATOR(name, spelling) case PPTokenKind::name:
#include "stcc/lexer/token_kinds.def"
        return true;
    default:
        return false;
    }
}

std::string_view to_string(const PPTokenKind k) {
    switch (k) {
#define STCC_TOKEN(name, spelling) \
    case PPTokenKind::name:        \
        return #name;
#include "stcc/lexer/token_kinds.def"
    default:
        UNREACHABLE("uncovered enumerate: {}", static_cast<size_t>(k));
    }
}

std::string_view to_symbol(const PPTokenKind k) {
    switch (k) {
#define STCC_TOKEN(name, spelling) \
    case PPTokenKind::name:        \
        return spelling;
#include "stcc/lexer/token_kinds.def"
    default:
        UNREACHABLE("uncovered enumerate: {}", static_cast<size_t>(k));
    }
}

} // namespace stcc
