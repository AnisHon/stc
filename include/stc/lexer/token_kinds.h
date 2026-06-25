/**
 * @date 2026-06-22 15:26:54
 * Generated, DO NOT EDIT!!!
 */
#pragma once

#include <cstdint>
#include <string_view>
#include "generated/token_kinds.def"
#include "stc/utils/macros.h"

namespace stc::lexer {

enum class TokenKind : uint8_t {
#define X(kind, symbol, name, since) name,
    STC_TOKEN_KIND_SPECIAL_LIST
    STC_TOKEN_KIND_IDENT_LIST
    STC_TOKEN_KIND_LITERAL_LIST
    STC_TOKEN_KIND_KEYWORD_LIST
    STC_TOKEN_KIND_DELIMITER_LIST
    STC_TOKEN_KIND_OPERATOR_LIST
    STC_TOKEN_KIND_PREPROCESSOR_LIST
    STC_TOKEN_KIND_DIGRAPH_LIST
#undef X
};

/**
 * 获取 punctuator 的长度
 */
size_t get_punctuator_len(TokenKind kind);

/**
 * 是否是特殊字符（ + - * / ; () [] {} += == 之类 ）的开始字符
 */
bool is_punctuators_start(char32_t chr);

/**
 * 查询 keyword
 */
bool lookup_keyword(std::u8string_view view);

/**
 * 是否是 keyword
 */
bool is_keyword(TokenKind k);

/**
 * 是否是常量
 */
bool is_literal(TokenKind k);

/**
 * 是否是赋值运算符
 */
bool is_assignment_op(TokenKind k);

/**
 * 是否是预处理
 */
bool is_preprocessor(TokenKind k);

/**
 * 枚举转字符串，得到枚举名字符串
 */
std::string_view to_string(TokenKind k);

/**
 * 枚举转字符串，得到原始符号的内容
 */
std::string_view to_symbol(TokenKind k);

} // namespace stc::lexer