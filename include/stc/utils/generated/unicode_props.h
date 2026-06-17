/**
 * @date 2026-06-17 21:47:21
 * Generated, DO NOT EDIT!!!
 */
#pragma once

namespace stc::utils {

struct UnicodeRange {
    const char32_t beg;
    const char32_t end;
    const char32_t tag;
};

/**
 * @brief ICU字符范围数据表
 */
extern const UnicodeRange UNICODE_RANGES[3712];

} // namespace stc::utils