/**
 * @file file_manager.h
 * @author anishan
 * @date 2026/11/18
 */

#ifndef STCC_FILE_MANAGER_H
#define STCC_FILE_MANAGER_H

#include <cstddef>
#include <expected>
#include <filesystem>
#include <optional>
#include <spdlog/logger.h>
#include <system_error>
#include <type_traits>
#include <unordered_map>

#include "stcc/sys/file.h"
#include "stcc/sys/file_key.h"
#include "stcc/sys/memory_buffer.h"

namespace stcc::source {

/// 已加载文件的标识、路径和源码缓冲区。
struct FileEntry {
    sys::FileKey key;
    size_t file_sz;
    /// 首次打开此文件时传入的路径。
    std::filesystem::path filename;
    /// 通过已打开的文件句柄查询到的绝对路径。
    std::filesystem::path phy_path;
    sys::MemoryBuffer buffer;
};

static_assert(std::is_move_constructible_v<sys::MemoryBuffer>);
static_assert(std::is_move_constructible_v<sys::File>);
static_assert(std::is_move_constructible_v<FileEntry>);

/**
 * 按文件标识缓存已加载的源码。同一个文件经不同路径打开时复用已有条目。
 */
class FileManager {
public:
    /// 借用日志记录器，记录器须在 FileManager 使用期间保持有效。
    explicit FileManager(spdlog::logger &log)
        : log_{log} {}

    /**
     * 返回已有条目，或读取文件并把新条目加入缓存。失败时返回对应的文件错误。
     * 返回的指针借用 FileManager 持有的条目。
     */
    std::expected<const FileEntry *, std::error_code> get_or_open(
        const std::filesystem::path &path
    );

    /// 查找已缓存的缓冲区。文件标识未登记时返回 std::nullopt。
    std::optional<const sys::MemoryBuffer *> get_buffer(sys::FileKey key);

private:
    spdlog::logger &log_;
    std::unordered_map<sys::FileKey, FileEntry> entries_;
};

} // namespace stcc::source

#endif // STCC_FILE_MANAGER_H
