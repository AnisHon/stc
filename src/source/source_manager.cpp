/**
 * @file source_manager.cpp
 * @author anishan
 * @data 2026/6/12
 * manager的实现文件
 */

#include <fstream>
#include <string_view>
#include <vector>

#include "stc/source/source_manager.h"
#include "stc/source/source_location.h"
#include "stc/utils/io_utils.h"
#include "stc/utils/strutils.h"
#include "stc/utils/macros.h"

namespace stc {

namespace source {

FileBuffer::~FileBuffer() {
    switch (this->kind_) {
    case StorageKind::Empty:
        break;
    case StorageKind::Heap:
        delete[] this->data_;
        break;
    case StorageKind::MMap:
        // todo
        TODO("MMAP NOT IMPLEMENTED");
        break;
    }
    this->data_ = nullptr;
    this->kind_ = StorageKind::Empty;
    this->logical_size_ = 0;
}

std::expected<FileBuffer, utils::IOError> FileBuffer::open(const std::filesystem::path &path) {
    // todo 先不做mmap，全部采用简单读取拷贝
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return std::unexpected(utils::IOError::OpenFailed);
    }

    // 获取大小
    file.seekg(0, std::ios::end);
    const auto size = static_cast<std::streamsize>(file.tellg());
    file.seekg(0, std::ios::beg);

    if (size < 0) {
        return std::unexpected(utils::IOError::ReadFailed);
    }


    // 读
    char *data = new char[size + 1];  // 多塞一个 \0
    data[size] = '\0';
    file.read(data, size);

    return FileBuffer(data, size, StorageKind::Heap);
}

} // namespace source

SourceManager::SourceManager() {
    // 初始化，占用无效 0 号索引
    this->files_.emplace_back();
    this->macros_.emplace_back();
}

std::expected<source::FileID, utils::IOError> SourceManager::create_file(
    const std::filesystem::path &path
) {
    // 自增 ID
    const source::FileID id{static_cast<source::FileID>(files_.size())};

    auto [it, inserted] = this->path2id_.try_emplace(path, id);

    if (!inserted) {
        return it->second;
    }

    // 不存在则创建
    return source::FileBuffer::open(path).transform(
        [id, this, &path](source::FileBuffer content) -> source::FileID {
            // 算line start
            const std::string_view sv(content.begin(), content.end());
            std::vector<std::uint32_t> line_starts = utils::get_line_start_indices(sv);

            // 加入文件，建立双射
            this->files_.emplace_back(id, path, std::move(content), std::move(line_starts));

            // 算索引
            return id;
        }
    );
}

source::MacroID SourceManager::create_macro(CreateMacroInfo info) {
    source::MacroID id{static_cast<uint32_t>(this->macros_.size())};
    macros_.emplace_back(id, info.file_id, info.file_begin, info.file_end);
    return id;
}

// void SourceManager::push_entry(SourceEntry entry) {
//     this->entries_.emplace_back(std::move(entry));
// }

} // namespace stc
