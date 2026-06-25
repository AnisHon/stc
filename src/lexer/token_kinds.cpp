/**
 * @date 2026-06-22 15:26:54
 * Generated, DO NOT EDIT!!!
 */
#include "stc/utils/macros.h"
#include "stc/lexer/token_kinds.h"

namespace stc::lexer {

size_t get_punctuator_len(const TokenKind kind) {
    switch (kind) {
#define X(kind, symbol, name, since) \
    case TokenKind::name: \
         return std::u8string_view(u8##symbol).size();

    STC_TOKEN_KIND_DELIMITER_LIST
    STC_TOKEN_KIND_OPERATOR_LIST
    STC_TOKEN_KIND_PREPROCESSOR_LIST
    STC_TOKEN_KIND_DIGRAPH_LIST
#undef X
    default:
        UNREACHABLE("not a punctuator token kind: {}", to_string(kind));
    }
}

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
        return true;
    default:
        return false;
    }
}

/**
 * 是否是 keyword
 */
bool is_keyword(const TokenKind k) {
    switch (k) {
#define X(kind, symbol, name, since) case TokenKind::name:
    STC_TOKEN_KIND_KEYWORD_LIST
#undef X
        return true;
    default:
        return false;
    }
}

/**
 * 是否是常量
 */
bool is_literal(const TokenKind k) {
    switch (k) {
#define X(kind, symbol, name, since) case TokenKind::name:
    STC_TOKEN_KIND_LITERAL_LIST
#undef X
        return true;
    default:
        return false;
    }
}

bool is_assignment_op(const TokenKind k) {
    switch (k) {
    case TokenKind::Equal:
    case TokenKind::PlusEqual:
    case TokenKind::MinusEqual:
    case TokenKind::StarEqual:
    case TokenKind::SlashEqual:
    case TokenKind::PercentEqual:
    case TokenKind::AmpEqual:
    case TokenKind::PipeEqual:
    case TokenKind::CaretEqual:
    case TokenKind::ShiftLeftEqual:
    case TokenKind::ShiftRightEqual:
        return true;
    default:
        return false;
    }
}

/**
 * 是否是预处理
 */
bool is_preprocessor(const TokenKind k) {
    switch (k) {
#define X(kind, symbol, name, since) case TokenKind::name:
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
std::string_view to_string(const TokenKind k) {
    switch (k) {
#define X(kind, symbol, name, since) \
    case TokenKind::name: \
        return #name;

    STC_TOKEN_KIND_SPECIAL_LIST
    STC_TOKEN_KIND_IDENT_LIST
    STC_TOKEN_KIND_LITERAL_LIST
    STC_TOKEN_KIND_KEYWORD_LIST
    STC_TOKEN_KIND_DELIMITER_LIST
    STC_TOKEN_KIND_OPERATOR_LIST
    STC_TOKEN_KIND_PREPROCESSOR_LIST
    STC_TOKEN_KIND_DIGRAPH_LIST
#undef X
    default:
        UNREACHABLE("uncovered enumerate: {}", "???");
    }
}

/**
 * 枚举转字符串，得到原始符号的内容
 */
std::string_view to_symbol(const TokenKind k) {
    switch (k) {
#define X(kind, symbol, name, since) \
    case TokenKind::name: \
        return symbol;

    STC_TOKEN_KIND_SPECIAL_LIST
    STC_TOKEN_KIND_IDENT_LIST
    STC_TOKEN_KIND_LITERAL_LIST
    STC_TOKEN_KIND_KEYWORD_LIST
    STC_TOKEN_KIND_DELIMITER_LIST
    STC_TOKEN_KIND_OPERATOR_LIST
    STC_TOKEN_KIND_PREPROCESSOR_LIST
    STC_TOKEN_KIND_DIGRAPH_LIST
#undef X
    default:
        UNREACHABLE("uncovered enumerate: {}", "???");
    }
}

} // namespace stc::lexer