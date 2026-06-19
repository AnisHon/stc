/**
 * @file unicode_utils.h
 * @author anishan
 * @date 2026/6/17
 */

#ifndef STC_UNICODE_UTILS_H
#define STC_UNICODE_UTILS_H

#include "stc/utils/generated/unicode_props.h"

namespace stc::utils {

/**
 * 检查unicode是否有该属性
 * @param chr unicode 码点
 * @param mask 查询位置的掩码
 * @return 是否有属性
 */
inline bool has_unicode_class(const char32_t chr, const uint32_t mask) {
    int l = 0;
    int r = static_cast<int>(std::size(gen::UNICODE_RANGES)) - 1;

    // 二分查找第一个 beg <= chr 的区间
    int result = -1;
    while (l <= r) {
        const int mid = l + (r - l) / 2; // 防止溢出
        if (gen::UNICODE_RANGES[mid].beg <= chr) {
            result = mid; // 记录候选位置
            l = mid + 1; // 继续向右找更大的 beg
        } else {
            r = mid - 1;
        }
    }

    // 检查找到的候选区间是否包含 chr
    if (result != -1 && chr <= gen::UNICODE_RANGES[result].end) {
        return gen::UNICODE_RANGES[result].tag & mask;
    }
    return false;
}

/**
 * 是否是 XID_Start 属性的字符，支持快路径
 */
inline bool is_xid_start(const char32_t chr) {
    // 快路径
    if (chr <= 0x7f) {
        return (chr >= U'A' && chr <= U'Z') ||
               (chr >= U'a' && chr <= U'z');
    }

    return has_unicode_class(
        chr,
        static_cast<uint32_t>(gen::UnicodeClass::XIDStart)
        );
}

/**
 * 是否是 XID_Continue 属性的字符，支持快路径
 */
inline bool is_xid_continue(const char32_t chr) {
    // 快路径
    if (chr <= 0x7f) {
        return (chr >= U'A' && chr <= U'Z') ||
               (chr >= U'a' && chr <= U'z') ||
               (chr >= U'0' && chr <= U'9') ||
               chr == U'_';
    }

    return has_unicode_class(
        chr,
        static_cast<uint32_t>(gen::UnicodeClass::XIDContinue)
        );
}

inline bool is_ascii_whitespace(const char32_t chr) {
    return chr == U' ' || chr == U'\t' || chr == U'\n' ||
           chr == U'\r' || chr == U'\f' || chr == U'\v';
}

}

#endif //STC_UNICODE_UTILS_H
