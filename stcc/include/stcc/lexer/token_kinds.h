#pragma once

#include <string_view>
#include <stdint.h>

namespace stcc {

enum class PPTokenKind : uint8_t {
#define STCC_TOKEN(name, spelling) name,
#include "stcc/lexer/token_kinds.def"
};

/**
 * 是否是 # 或 ##
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
