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
    this->_files.emplace_back(InvalidFileID, "", u8"");
    this->_macros.emplace_back(InvalidMacroID, InvalidFileID, 0, 0);
}


std::expected<FileID, utils::IOError> Manager::create_file(const std::filesystem::path& path) {
    auto [it,insert] = this->_file_id.try_emplace(path, InvalidFileID);

    // 不存在则创建
    if (insert) {
        const FileID id = _files.size();

        // 读文件
        const auto content_res = utils::read_u8string(path);
        if (!content_res) {
            return std::unexpected(content_res.error());
        }
        const auto content{std::move(*content_res)};

        // 算索引
        std::vector<std::uint32_t> line_starts = utils::get_line_start_indices(content);

        FileInfo info{id, path, content, std::move(line_starts)};
        this->_files.emplace_back(std::move(info));
        it->second = id;
    }

    return std::ref(it->second);
}

MacroID Manager::create_macro(CreateMacroInfo info) {
    MacroID id = this->_macros.size();
    _macros.emplace_back(id, info.file_id, info.file_begin, info.file_end);
    return id;
}

void Manager::push_entry(SourceEntry entry) {
    this->_entries.emplace_back(std::move(entry));
}

}
