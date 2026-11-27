/**
 * @file utf8.h
 * @author anishan
 * @date 2026/11/25
 */

#ifndef STCC_UTF8_H
#define STCC_UTF8_H

#include <cstddef>
#include <span>
#include <stdint.h>

#include "stcc/commons/constant.h"

namespace stcc::utf8 {

// 输入使用同一缓冲区内的半开区间 [first, last)。两个空指针也表示空区间。

enum class DecodeStatus : int8_t { Ok, Eof, Invalid };

struct DecodeResult {
    char32_t codepoint;
    std::size_t bytes;
    DecodeStatus status;
};

/// 判断码点是否为可用 UTF-8 编码的 Unicode 标量值。
[[nodiscard]] constexpr bool is_valid_codepoint(char32_t codepoint) noexcept {
    return codepoint <= 0x10ffff && (codepoint < 0xd800 || codepoint > 0xdfff);
}

/**
 * 判断 position 是否位于序列末尾或可能的字符起始字节上。
 * position 越过 last 时返回 false。
 * 起始字节之后的编码是否完整、合法，由 decode 判断。
 */
[[nodiscard]] constexpr bool is_boundary(const char *position, const char *last) noexcept {
    if (position >= last) {
        return position == last;
    }
    const auto byte = static_cast<unsigned char>(*position);
    return byte < 0x80 || (byte >= 0xc2 && byte <= 0xf4);
}

/**
 * 解码输入开头的一个码点。成功时返回该码点和占用的字节数。
 * 空输入返回 Eof。非法编码（含截断）返回 Invalid 和 U+FFFD。
 * 两种情况下 bytes 均为 0，由调用者决定如何继续。
 */
[[nodiscard]] constexpr DecodeResult decode(const char *first, const char *last) noexcept {
    if (first == last) {
        return {.codepoint = U'\0', .bytes = 0, .status = DecodeStatus::Eof};
    }

    const auto lead = static_cast<unsigned char>(*first);
    if (lead < 0x80) {
        return {.codepoint = lead, .bytes = 1, .status = DecodeStatus::Ok};
    }

    constexpr DecodeResult invalid{
        .codepoint = chr32::replacement,
        .bytes = 0,
        .status = DecodeStatus::Invalid,
    };
    if (!is_boundary(first, last)) {
        return invalid;
    }

    std::size_t width{2};
    char32_t minimum{0x80};
    if (lead >= 0xf0) {
        width = 4;
        minimum = 0x10000;
    } else if (lead >= 0xe0) {
        width = 3;
        minimum = 0x800;
    }
    const auto lead_mask = 0x7fu >> width;
    char32_t codepoint = lead & lead_mask;
    for (std::size_t i = 1; i < width; ++i) {
        ++first;
        if (first == last) {
            return invalid;
        }
        const auto byte = static_cast<unsigned char>(*first);
        if ((byte & 0xc0u) != 0x80u) {
            return invalid;
        }
        codepoint = (codepoint << 6) | (byte & 0x3fu);
    }
    if (codepoint < minimum || !is_valid_codepoint(codepoint)) {
        return invalid;
    }
    return {.codepoint = codepoint, .bytes = width, .status = DecodeStatus::Ok};
}

/// 判断区间开头是否包含完整的 UTF-8 BOM（U+FEFF）。
[[nodiscard]] constexpr bool has_bom(const char *first, const char *last) noexcept {
    const auto result = decode(first, last);
    return result.status == DecodeStatus::Ok && result.codepoint == U'\ufeff';
}

/**
 * 将一个码点编码到 output 的开头，返回写入的字节数。
 * 码点非法或空间不足时返回 0，输出缓冲区不变。
 */
[[nodiscard]] constexpr std::size_t encode(char32_t codepoint, std::span<char> output) noexcept {
    if (!is_valid_codepoint(codepoint)) {
        return 0;
    }

    std::size_t width{1};
    unsigned char lead_prefix{0};
    if (codepoint >= 0x10000) {
        width = 4;
        lead_prefix = 0xf0;
    } else if (codepoint >= 0x800) {
        width = 3;
        lead_prefix = 0xe0;
    } else if (codepoint >= 0x80) {
        width = 2;
        lead_prefix = 0xc0;
    }
    if (output.size() < width) {
        return 0;
    }

    for (std::size_t i = width - 1; i > 0; --i) {
        output[i] = static_cast<char>(0x80u | (codepoint & 0x3fu));
        codepoint >>= 6;
    }
    output.front() = static_cast<char>(lead_prefix | codepoint);
    return width;
}

/// 检查整个字节序列是否都是合法的 UTF-8。空序列也合法。
[[nodiscard]] bool is_valid(const char *first, const char *last) noexcept;

/**
 * 从当前字节之后寻找下一个可能的字符起始字节，返回相对输入开头的偏移。
 * 未找到时返回整个区间的字节数。非空输入至少跳过一个字节。
 */
[[nodiscard]] std::size_t resync(const char *first, const char *last) noexcept;

} // namespace stcc::utf8

#endif // STCC_UTF8_H
