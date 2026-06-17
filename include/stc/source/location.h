/**
 * @file location.h
 * @author anishan
 * @date 2026/6/11
 * 一般对于 include 将会直接当作文件，处理时通过 FileEntry 而不是 MacroEntry
 * 对于使用 define 定义的宏，将会实例化成一个 MacroEntry，类似模版引擎生成一段"渲染的字符串"
 * 所有的索引都是字节索引，不是 u8 字符索引
 *
 * source 的管理打算应用的结构是
 * - 真的将文件展开，记录其展开后的偏移量
 * - 为了更好的位置提示，资源统一管理，实现将逻辑偏移量映射到物理偏移量，目标复杂度 O(log n)
 * - location 能直接定位物理位置（一个坐标），逻辑位置，文件，宏派生
 */

#ifndef STC_SOURCE_LOCATION_H
#define STC_SOURCE_LOCATION_H
#include <filesystem>
#include <string>
#include <variant>
#include <vector>

namespace stc::source {

/// FileID 和 MacroID 的 0 视作无效 ID
using FileID = std::uint32_t;
using MacroID = std::uint32_t;

constexpr FileID InvalidFileID = 0;
constexpr MacroID InvalidMacroID = 0;

/**
 * 文件信息，
 */
struct FileInfo {
    /// 文件ID，冗余存储
    const FileID id;

    /// 文件路径
    const std::filesystem::path path;

    /// 文件内容
    const std::u8string content;

    /// 行号 映射 行开始偏移量 的快速索引
    const std::vector<std::uint32_t> line_starts;
};

/**
 * 宏定义的代码信息，这里是 define
 */
struct MacroInfo {
    /// 当前的ID，冗余存储
    const MacroID macro_id;

    /// 定义宏的文件
    const FileID file_id;

    /// 宏定义的文件起始位置
    const std::uint32_t file_begin;

    /// 宏定义的文件结束位置
    const std::uint32_t file_end;
};

/**
 * 文件的额外信息
 */
struct FileEntry {
    /// 来自的文件
    const FileID id;

};


/**
 * 宏展开的额外信息，不是 include
 */
struct MacroEntry {
    /// 使用的宏
    const MacroID id;

    /// 具体宏展开后的内容，由宏生成
    const std::u8string content;
};

/**
 * 代码开始结束，代码内容
 */
struct SourceEntry {

    /// 逻辑起始位置
    const uint32_t begin;

    /// 逻辑结束位置
    const uint32_t end;

    /// 代码内容
    const std::u8string_view buffer;

    /// 在文件中的起始位置
    const std::uint32_t file_begin;

    /// 在文件中的结束位置
    const std::uint32_t file_end;

    /// 具体变体
    const std::variant<FileEntry, MacroEntry> variant;
};

/**
 * 位置
 */
struct Location {
    uint32_t offset;
};

/**
 * 实现偏移量和location的加法，都是32位数不使用指针和引用，拷贝更快
 * @return new_location: location.offset + rhs
 */
constexpr Location operator+(Location lhs, const uint32_t rhs) {
    lhs.offset += rhs;
    return lhs;
}

/**
 * 位置区间，左闭右开 [L, R)
 */
struct LocationRange {
    const Location begin;
    const Location end;
};


constexpr Location zero_loc() {
    return {};
}

constexpr LocationRange zero_range() {
    return {zero_loc(), zero_loc()};
}

}

#endif //STC_SOURCE_LOCATION_H
