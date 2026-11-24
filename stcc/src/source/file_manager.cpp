/**
 * @file file_manager.cpp
 * @author anishan
 * @date 2026/11/23
 */

#include "stcc/source/file_manager.h"

#include <utility>

#include "stcc/commons/macros.h"

namespace stcc::source {

std::expected<const FileEntry *, std::error_code> FileManager::get_or_open(
    const std::filesystem::path &path
) {
    auto file = sys::File::open(path);
    this->log_.debug("adding file {}, exists: {}", path.string(), !!file);
    TRY(file);

    const auto key = file->file_key();
    TRY(key);

    if (const auto it = this->entries_.find(*key); it != this->entries_.end()) {
        return &it->second;
    }

    const auto file_sz = file->file_sz();
    auto phy_path = file->phy_path();
    auto buffer = sys::MemoryBuffer::open_file(*file);

    TRY(file_sz);
    TRY(phy_path);
    TRY(buffer);

    FileEntry entry{
        .key = *key,
        .file_sz = *file_sz,
        .filename = path,
        .phy_path = std::move(*phy_path),
        .buffer = std::move(*buffer),
    };

    auto [it, _] = this->entries_.try_emplace(*key, std::move(entry));

    return &it->second;
}

std::optional<const sys::MemoryBuffer *> FileManager::get_buffer(sys::FileKey key) {
    const auto it = this->entries_.find(key);
    if (it == this->entries_.end()) {
        return std::nullopt;
    }
    return &it->second.buffer;
}

} // namespace stcc::source
