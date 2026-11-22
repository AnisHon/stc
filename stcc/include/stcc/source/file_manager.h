/**
 * @file file_manager.h
 * @author anishan
 * @date 2026/11/18
 */

#ifndef STCC_FILE_MANAGER_H
#define STCC_FILE_MANAGER_H

#include <unordered_map>

#include "stcc/sys/file_key.h"
#include "stcc/sys/memory_buffer.h"

namespace stcc::source {

struct FileEntry {
    sys::FileKey key;
    size_t file_sz;
    std::string filename;
    std::filesystem::path phy_path;
    sys::MemoryBuffer buffer;
};

/**
 * 管理 File 去重复
 */
class FileManager {
public:
    std::expected<FileEntry &, std::error_code> get_file(const std::filesystem::path &path);

    std::expected<sys::MemoryBuffer, std::error_code> get_buffer(sys::FileKey key);

private:
    std::unordered_map<sys::FileKey, FileEntry> entries_;
};

} // namespace stcc::source

#endif // STCC_FILE_MANAGER_H
