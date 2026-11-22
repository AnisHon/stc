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

/// 只读文件映射，sz 是实际文件字节数。
struct MMapResult {
    const char *begin{};
    std::size_t sz{};
#ifdef STCC_PLATFORM_WINDOWS
    HANDLE handle{};
#endif // STCC_PLATFORM_WINDOWS
};

/**
 * 映射完整的非空文件，文件句柄仍由调用者管理。
 *
 * 文件尾部落在页内时，可以检查 begin[sz] 是否为系统补出的 NUL。
 * 文件大小恰为页大小的整数倍时，begin[sz] 不在映射内，不可读取。
 * 成功结果用 unmmap_file() 释放。空文件返回 invalid_argument。
 */
std::expected<MMapResult, std::error_code> mmap_file(FileT file) noexcept;

/// 解除映射并清空 result。允许传入空结果。
void unmmap_file(MMapResult &result) noexcept;

} // namespace stcc::sys

#endif // STCC_MMAP_H
