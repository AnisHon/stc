/**
 * @file sys_mmap.h
 * @author anishan
 * @date 2026/11/19
 */

#ifndef STCC_SYS_MMAP_H
#define STCC_SYS_MMAP_H

#include <gsl/pointers>
#include <filesystem>

namespace stcc::utils {

struct MMapResult {
    const char *begin;
    size_t sz;
};

MMapResult mmap_file(const std::filesystem::path &path) noexcept;

void unmmap_file(MMapResult &result) noexcept;

} // namespace stcc::utils

#endif // STCC_SYS_MMAP_H