/**
 * @file source_manager.cpp
 * @author anishan
 * @data 2026/6/12
 * manager的实现文件
 */

#include <vector>

#include "stc/source/source_manager.h"
#include "stc/utils/io_utils.h"
#include "stc/utils/strutils.h"

namespace stc {

SourceManager::SourceManager() {
  // 初始化，占用无效 0 号索引
  this->files_.emplace_back(source::FileID::invalid(), "", "");
  this->macros_.emplace_back(source::MacroID::invalid(),
                             source::FileID::invalid(), 0, 0);
}

std::expected<source::FileID, utils::IOError>
SourceManager::create_file(const std::filesystem::path &path) {
  auto [it, insert] =
      this->file_id_.try_emplace(path, source::FileID::invalid());

  // 不存在则创建
  if (insert) {
    const source::FileID id{static_cast<uint32_t>(files_.size())};

    // 读文件
    const auto content_res = utils::read_text(path);
    if (!content_res) {
      return std::unexpected(content_res.error());
    }
    const auto &content{*content_res};

    // 算索引
    std::vector<std::uint32_t> line_starts =
        utils::get_line_start_indices(content);

    source::FileInfo info{id, path, content, std::move(line_starts)};
    this->files_.emplace_back(std::move(info));
    it->second = id;
  }

  return std::ref(it->second);
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
