/**
 * @file source_manager.cpp
 * @author anishan
 * @data 2026/6/12
 * manager的实现文件
 */

#include <string_view>
#include <vector>

#include "stc/source/source_manager.h"
#include "stc/source/source_location.h"
#include "stc/utils/io_utils.h"
#include "stc/utils/strutils.h"

namespace stc {

namespace source {} // namespace source

SourceManager::SourceManager() {
    // 初始化，占用无效 0 号索引
    this->files_.emplace_back();
    this->macros_.emplace_back();
}

std::expected<source::FileID, utils::IOError> SourceManager::create_file(
    const std::filesystem::path &path
) {
    auto it = this->file_id_.find(path);
    auto existed = it != file_id_.end();

    // 不存在则创建
    if (!existed) {
        return source::FileBuffer::open(path).transform(
            [this, &path](source::FileBuffer content) -> source::FileID {
                // 自增ID
                const source::FileID id{static_cast<uint32_t>(files_.size())};

                // 算line start
                const std::string_view sv(content.get(), content.size());
                std::vector<std::uint32_t> line_starts = utils::get_line_start_indices(sv);

                // 加入文件，建立双射
                this->files_.emplace_back(id, path, std::move(content), std::move(line_starts));
                this->file_id_.emplace(path, id);

                // 算索引
                return id;
            }
        );
    }

    return it->second;
}

source::MacroID SourceManager::create_macro(CreateMacroInfo info) {
    source::MacroID id{static_cast<uint32_t>(this->macros_.size())};
    macros_.emplace_back(id, info.file_id, info.file_begin, info.file_end);
    return id;
}

void SourceManager::push_entry(SourceEntry entry) {
    this->entries_.emplace_back(std::move(entry));
}

} // namespace stc
