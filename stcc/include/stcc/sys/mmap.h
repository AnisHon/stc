/**
 * @file mmap.h
 * @author anishan
 * @date 2026/11/19
 */

#ifndef STCC_MMAP_H
#define STCC_MMAP_H

#include <cstddef>
#include <expected>
#include <system_error>

#include "stcc/sys/file.h"

namespace stcc::sys {

struct MMapResult {
    const char *begin;
    std::size_t sz;
#ifdef STCC_PLATFORM_WINDOWS
    HANDLE handle;
#endif // STCC_PLATFORM_WINDOWS
};

std::expected<MMapResult, std::error_code> mmap_file(FileT file) noexcept;

void unmmap_file(MMapResult &result) noexcept;

} // namespace stcc::sys

#endif // STCC_MMAP_H
