/**
 * @date 2026-06-22 15:26:54
 * Generated, DO NOT EDIT!!!
 */
#include "stc/utils/macros.h"
#include "stc/lexer/token_kinds.h"

namespace stc::lexer {

bool is_punctuators_start(const char32_t chr) {
    switch (chr) {
    case U'!':
    case U'%':
    case U'&':
    case U'(':
    case U')':
    case U'*':
    case U'+':
    case U',':
    case U'-':
    case U'.':
    case U'/':
    case U':':
    case U';':
    case U'<':
    case U'=':
    case U'>':
    case U'?':
    case U'[':
    case U']':
    case U'^':
    case U'{':
    case U'|':
    case U'}':
    case U'~':
    case U'#':
        return true;
    default:
        return false;
    }
}

/**
 * 是否是预处理
 */
bool is_preprocessor(const PPTokenKind k) {
    switch (k) {
#define X(kind, symbol, name, since) case PPTokenKind::name:
    STC_TOKEN_KIND_PREPROCESSOR_LIST
#undef X
        return true;
    default:
        return false;
    }
}

/**
 * 枚举转字符串，得到枚举名字符串
 */
std::string_view to_string(const PPTokenKind k) {
    switch (k) {
#define X(kind, symbol, name, since) \
    case PPTokenKind::name: return #name;
    STC_PP_TOKEN_KIND_ALL_LIST
#undef X
    default:
        UNREACHABLE("uncovered enumerate: {}", static_cast<size_t>(k));
    }
}

/**
 * 枚举转字符串，得到原始符号的内容
 */
std::string_view to_symbol(const PPTokenKind k) {
    switch (k) {
#define X(kind, symbol, name, since) \
    case PPTokenKind::name: return symbol;
    STC_PP_TOKEN_KIND_ALL_LIST
#undef X
    default:
        UNREACHABLE("uncovered enumerate: {}", static_cast<size_t>(k));
    }
}

} // namespace stc::lexer