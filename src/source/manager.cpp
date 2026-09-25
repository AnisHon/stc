/**
 * @file manager.cpp
 * @author anishan
 * @data 2026/6/12
 * manager的实现文件
 */

#include <vector>

#include "stc/source/manager.h"
#include "stc/utils/io_utils.h"
#include "stc/utils/strutils.h"

namespace stc::source {

Manager::Manager() {
    // 初始化，占用无效 0 号索引
    this->files_.emplace_back(FileID::invalid(), "", "");
    this->macros_.emplace_back(MacroID::invalid(), FileID::invalid(), 0, 0);
}

std::expected<FileID, utils::IOError> Manager::create_file(const std::filesystem::path& path) {
    auto [it, insert] = this->file_id_.try_emplace(path, FileID::invalid());

    // 不存在则创建
    if (insert) {
        const FileID id{static_cast<uint32_t>(files_.size())};

        // 读文件
        const auto content_res = utils::read_text(path);
        if (!content_res) {
            return std::unexpected(content_res.error());
        }
        const auto& content{*content_res};

        // 算索引
        std::vector<std::uint32_t> line_starts = utils::get_line_start_indices(content);

        FileInfo info{id, path, content, std::move(line_starts)};
        this->files_.emplace_back(std::move(info));
        it->second = id;
    }

    return std::ref(it->second);
}

MacroID Manager::create_macro(CreateMacroInfo info) {
    MacroID id{static_cast<uint32_t>(this->macros_.size())};
    macros_.emplace_back(id, info.file_id, info.file_begin, info.file_end);
    return id;
}

void Manager::push_entry(SourceEntry entry) {
    this->entries_.emplace_back(std::move(entry));
}

} // namespace stc::source
