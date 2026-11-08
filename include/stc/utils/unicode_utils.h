/**
 * @file unicode_utils.h
 * @author anishan
 * @date 2026/6/17
 */

#ifndef STC_UNICODE_UTILS_H
#define STC_UNICODE_UTILS_H

#include <cstdint>

namespace stc::utils {

/**
 * 检查unicode是否有该属性
 * @param chr unicode 码点
 * @param mask 查询位置的掩码
 * @return 是否有属性
 */
bool has_unicode_class(char32_t chr, std::uint32_t mask);

/**
 * 是否是 XID_Start 属性的字符，支持快路径
 */
bool is_xid_start(char32_t chr);

/**
 * 是否是 XID_Continue 属性的字符，支持快路径
 */
bool is_xid_continue(char32_t chr);

/**
 * 是否是 ASCII 换行字符
 */
bool is_newline(char chr);

/**
 * 是否是 ASCII octal digit（0-7）
 */
bool is_oct(char chr);


} // namespace stc::utils

#endif // STC_UNICODE_UTILS_H
