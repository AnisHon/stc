/**
 * @date 2026-06-22 15:26:54
 * Generated, DO NOT EDIT!!!
 */
#pragma once

#include <cstdint>
#include <string_view>
#include "generated/token_kinds.inc"

#define STC_TOKEN_KIND_PUNCTUATOR_LIST \
    STC_TOKEN_KIND_DELIMITER_LIST \
    STC_TOKEN_KIND_OPERATOR_LIST \
    STC_TOKEN_KIND_PREPROCESSOR_LIST \
    STC_TOKEN_KIND_DIGRAPH_LIST

#define STC_PP_TOKEN_KIND_ALL_LIST \
    STC_TOKEN_KIND_SPECIAL_LIST \
    STC_TOKEN_KIND_IDENTIFIER_LIST \
    STC_TOKEN_KIND_PP_LIST \
    STC_TOKEN_KIND_PUNCTUATOR_LIST

namespace stc::lexer {

/**
 * 没有 Token
 */
enum class PPTokenKind : uint8_t {
#define X(kind, symbol, name, since) name,
    STC_PP_TOKEN_KIND_ALL_LIST
#undef X
};

/**
 * 是否是特殊字符（ + - * / ; () [] {} += == 之类 ）的开始字符
 */
bool is_punctuators_start(char32_t chr);

/**
 * 是否是预处理
 */
bool is_preprocessor(PPTokenKind k);

/**
 * 枚举转字符串，得到枚举名字符串
 */
std::string_view to_string(PPTokenKind k);

/**
 * 枚举转字符串，得到原始符号的内容
 */
std::string_view to_symbol(PPTokenKind k);

} // namespace stc::lexer