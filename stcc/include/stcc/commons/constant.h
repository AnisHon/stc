/**
 * @file constant.h
 * @author anishan
 * @date 2026/11/24
 */

#ifndef STCC_COMMONS_CONSTANT_H
#define STCC_COMMONS_CONSTANT_H

namespace stcc {

namespace chr {
/**
 * NUL 字符
 */
inline constexpr char nul = '\0';

} // namespace chr

namespace chr32 {

/**
 * NUL 字符
 */
inline constexpr char32_t nul = '\0';

/**
 * unicode replacement character 替换字符
 */
inline constexpr char32_t replacement = L'\ufffd';

} // namespace chr32

} // namespace stcc

#endif // STCC_COMMONS_CONSTANT_H
