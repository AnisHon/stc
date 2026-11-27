/**
 * @file utf8.cpp
 */

#include "stcc/utils/utf8.h"

namespace stcc::utf8 {

bool is_valid(const char *first, const char *last) noexcept {
    while (first != last) {
        const auto result = decode(first, last);
        if (result.status != DecodeStatus::Ok) {
            return false;
        }
        first += result.bytes;
    }
    return true;
}

std::size_t resync(const char *first, const char *last) noexcept {
    std::size_t skipped{0};
    while (first != last) {
        ++first;
        ++skipped;
        if (is_boundary(first, last)) {
            break;
        }
    }
    return skipped;
}

} // namespace stcc::utf8
