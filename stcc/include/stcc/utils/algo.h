/**
 * @file algo.h
 * @author anishan
 * @date 2026/11/26
 */

#ifndef STCC_UTILS_ALGO_H
#define STCC_UTILS_ALGO_H

namespace stcc::algo {

template <typename T, typename... Ts>
constexpr bool is_one_of(const T &x, const Ts &...xs) {
    return ((x == xs) || ...);
}

} // namespace stcc::algo

#endif // STCC_UTILS_ALGO_H
