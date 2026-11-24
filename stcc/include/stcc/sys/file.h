/**
 * @file file.h
 * @author anishan
 * @date 2026/11/22
 */

#ifndef STCC_FILE_H
#define STCC_FILE_H

#include "stcc/sys/file_key.h"
#include "stcc/sys/platform.h"
#include <expected>
#include <filesystem>
#include <system_error>

#if defined(STCC_PLATFORM_POSIX)
#  include <unistd.h>
#elif defined(STCC_PLATFORM_WINDOWS)
#  include <windows.h>
#else
#  error "Unsupported operating system."
#endif

namespace stcc::sys {

/**
 * 跨平台的统一文件描述符，file_t
 */
#if defined(STCC_PLATFORM_POSIX)
using FileT = int; // fd
#elif defined(STCC_PLATFORM_WINDOWS)
using FileT = HANDLE;
#endif

/**
 * 轻量级 File 抽象，包装平台相关文件标识符，提供平台无关接口，提供
 * - 获取大小
 * - 获取 FileKey 对象
 * - 获取物理路径
 * - 获取原生文件标识符
 */
class File {
public:
    static std::expected<File, std::error_code> open(const std::filesystem::path &path);

    File(const File &) = delete;
    File &operator=(const File &) = delete;
    File(File &&other) noexcept;
    File &operator=(File &&other) noexcept;
    ~File();

    /**
     * 原生文件标识符，fd 或 handle
     */
    FileT native_handle() const noexcept { return fd_; }

    /**
     * 文件唯一标识符
     */
    std::expected<FileKey, std::error_code> file_key() const;

    /**
     * 文件大小
     */
    std::expected<size_t, std::error_code> file_sz() const;

    /**
     * 从已打开的文件句柄查询绝对路径，解析打开时经过的符号链接。
     * 查询失败或平台无法从句柄取得路径时返回错误。
     */
    std::expected<std::filesystem::path, std::error_code> phy_path() const;

private:
    explicit File(FileT fd)
        : fd_{fd} {}
    void close() noexcept;

    FileT fd_;
};

} // namespace stcc::sys

#endif // STCC_FILE_H
