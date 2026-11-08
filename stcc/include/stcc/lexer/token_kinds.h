/**
 * @date 2026-06-22 15:26:54
 * Generated, DO NOT EDIT!!!
 */
#pragma once

#include "stcc/lexer/token_kinds.inc"

#include <cstdint>
#include <string_view>

#define STCC_TOKEN_KIND_PUNCTUATOR_LIST \
    STCC_TOKEN_KIND_DELIMITER_LIST      \
    STCC_TOKEN_KIND_OPERATOR_LIST       \
    STCC_TOKEN_KIND_PREPROCESSOR_LIST   \
    STCC_TOKEN_KIND_DIGRAPH_LIST

#define STCC_PP_TOKEN_KIND_ALL_LIST \
    STCC_TOKEN_KIND_SPECIAL_LIST    \
    STCC_TOKEN_KIND_IDENTIFIER_LIST \
    STCC_TOKEN_KIND_PP_LIST         \
    STCC_TOKEN_KIND_PUNCTUATOR_LIST

namespace stcc {

/**
 * 没有 Token
 */
enum class PPTokenKind : std::uint8_t {
#define X(kind, symbol, name, since) name,
    STCC_PP_TOKEN_KIND_ALL_LIST
#undef X
};

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

} // namespace stcc