/**
 * @date 2026-06-17 23:42:49
 * Generated, DO NOT EDIT!!!
 */
#pragma once

#include <cstdint>

namespace stc::utils::gen {

enum class UnicodeClass: std::uint32_t { 
    Alphabetic = 0x1,
    CaseIgnorable = 0x2,
    Cased = 0x4,
    ChangesWhenCasefolded = 0x8,
    ChangesWhenCasemapped = 0x10,
    ChangesWhenLowercased = 0x20,
    ChangesWhenTitlecased = 0x40,
    ChangesWhenUppercased = 0x80,
    Consonant = 0x100,
    DefaultIgnorableCodePoint = 0x200,
    Extend = 0x400,
    GraphemeBase = 0x800,
    GraphemeExtend = 0x1000,
    GraphemeLink = 0x2000,
    IDContinue = 0x4000,
    IDStart = 0x8000,
    InCB = 0x10000,
    Linker = 0x20000,
    Lowercase = 0x40000,
    Math = 0x80000,
    Uppercase = 0x100000,
    XIDContinue = 0x200000,
    XIDStart = 0x400000,
};

struct UnicodeRange {
    const char32_t beg;
    const char32_t end;
    const char32_t tag;
};

/**
 * @brief ICU字符范围数据表
 */
extern const UnicodeRange UNICODE_RANGES[3712];

} // namespace stc::utils::gen