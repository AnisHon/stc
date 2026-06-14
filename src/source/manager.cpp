//
// Created by anishan on 2026/6/12.
//
#include "stc/source/manager.h"
#include "stc/utils/todo.h"

namespace stc::source {

Manager::Manager() {
    // 初始化，占用无效 0 号索引
    this->_files.emplace_back(InvalidFileID, "", u8"");
    this->_macros.emplace_back(InvalidMacroID, InvalidFileID, 0, 0);
}

const FileInfo& Manager::create_file(const std::filesystem::path& path) {
    auto [it,insert] = this->_file_id.try_emplace(path, InvalidFileID);

    if (!insert) {
        return this->_files[it->second];
    }

    FileID id = _files.size();

    FileInfo{}

}

const MacroInfo& Manager::create_macro(CreateMacroInfo info) {
}

void Manager::push_entry(SourceEntry entry) {
}

}
