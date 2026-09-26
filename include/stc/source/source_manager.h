/**
 * @file source_manager.h
 * @author anishan
 * @date 2026/6/12
 * 源码管理，所有的宏都会在这里真实展开，同时复杂维护段定位真实文件位置
 */

#ifndef STC_SOURCE_MANAGER_H
#define STC_SOURCE_MANAGER_H

#include <filesystem>
#include <unordered_map>
#include <variant>
#include <vector>

#include "source_location.h"
#include "stc/utils/io_utils.h"

namespace stc {

namespace source {
/**
 *
 */
struct LineDirectiveInfo {
  const uint32_t offset;
  const uint32_t phy_line;
};

/**
 * 源码文件
 */
struct FileInfo {
  /// 文件ID，冗余存储
  const source::FileID id;

  /// 文件路径
  const std::filesystem::path path;

  /// 文件内容
  const std::string content;

  /// 行号 映射 行开始偏移量 的快速索引
  const std::vector<std::uint32_t> line_starts;

  /// 被 include 引用的位置
  const std::vector<LineDirectiveInfo> line_directives;
};

/**
 * 文件的额外信息
 */
struct FileEntry {
  /// 来自的文件
  const source::FileID id;

  const uint32_t byte_offset;
};

/**
 * 宏展开的额外信息，不是 include
 */
struct MacroEntry {
  /// 使用的宏
  const source::MacroID id;

  /// 具体宏展开后的内容，由宏生成
  const std::string content;
};

/**
 * 宏定义的代码信息，这里是 define
 */
struct MacroInfo {
  /// 当前的ID，冗余存储
  const source::MacroID macro_id;

  /// 定义宏的文件
  const source::FileID file_id;

  /// 宏定义的文件起始位置
  const std::uint32_t file_begin;

  /// 宏定义的文件结束位置
  const std::uint32_t file_end;
};

} // namespace source

/**
 * 代码开始结束，代码内容
 */
struct SourceEntry {

  /// 逻辑起始位置
  const uint32_t begin;

  /// 逻辑结束位置
  const uint32_t end;

  /// 代码内容
  const std::string_view buffer;

  /// 在文件中的起始位置
  const std::uint32_t file_begin;

  /// 在文件中的结束位置
  const std::uint32_t file_end;

  /// 具体变体
  const std::variant<source::FileEntry, source::MacroEntry> variant;
};

class SourceManager {

public:
  SourceManager();

  /**
   * 创建 MacroEntry 辅助结构
   */
  struct CreateMacroInfo {
    /// 来自的文件 ID
    source::FileID file_id;

    /// 宏定义的文件起始位置
    std::uint32_t file_begin{};

    /// 宏定义的文件结束位置
    std::uint32_t file_end{};
  };

  /**
   * 创建文件信息，会自动读取文件
   * @param path 文件路径
   * @return 创建 FileInfo
   */
  auto create_file(const std::filesystem::path &path)
      -> std::expected<source::FileID, utils::IOError>;

  /**
   * 创建宏信息
   * @param info 宏信息
   * @return 创建的 MacroInfo
   */
  source::MacroID create_macro(CreateMacroInfo info);

  /**
   * 推入代码片段
   * @param entry 代码片段
   */
  void push_entry(SourceEntry entry);

private:
  /// path 到 FileID 的映射，防重
  std::unordered_map<std::string, source::FileID> file_id_;

  /// FileID 到 FileInfo 的映射，0无效
  std::vector<source::FileInfo> files_;

  /// MacroID 到 MacroInfo 的映射，0无效
  std::vector<source::MacroInfo> macros_;

  /// 源码 entries 数组，所有展开都会在这里展平
  std::vector<SourceEntry> entries_;
};

} // namespace stc

#endif // STC_SOURCE_MANAGER_H
