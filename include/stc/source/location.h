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
#include <string>
#include <cstdint>

namespace stc::source {

/**
 * FileID 的 0 视作无效 ID
 */
class FileID {

public:
    constexpr FileID() = default;

    constexpr explicit FileID(const std::uint32_t value) : value_{value} {
    }

    static constexpr FileID invalid() {
        return FileID{};
    }

    bool operator==(const FileID&) const = default;

    [[nodiscard]]
    constexpr bool is_valid() const {
        return *this != invalid();
    }

private:
    std::uint32_t value_{0};

};


/**
 * MacroID 的 0 视作无效 ID
 */
class MacroID {
public:
    constexpr MacroID() = default;

    constexpr explicit MacroID(const std::uint32_t value) : value_{value} {
    }

    static constexpr MacroID invalid() {
        return MacroID{};
    }

    bool operator==(const MacroID&) const = default;

    [[nodiscard]]
    constexpr bool is_valid() const {
        return *this != invalid();
    }

private:
    std::uint32_t value_{0};

};


/**
 * 0 表示无效位置
 */
class SourceLocation {
public:
    constexpr SourceLocation() = default;

    constexpr explicit SourceLocation(const std::uint32_t value) : value{value} {
    }

    static constexpr SourceLocation invalid() {
        return SourceLocation{};
    }

    bool operator==(const SourceLocation&) const = default;

    [[nodiscard]]
    constexpr bool is_valid() const {
        return *this != invalid();
    }

private:
    std::uint32_t value{};

};


/**
 * 位置区间，左闭右开 [L, R)
 */
class SourceRange {
public:
    constexpr SourceRange() = default;

    constexpr SourceRange(const SourceLocation begin, const SourceLocation end) : begin{begin},
        end{end} {
    }

    constexpr static SourceRange invalid() {
        return {SourceLocation::invalid(), SourceLocation::invalid()};
    }

    /**
     * 空 range, [at, at)
     */
    constexpr static SourceRange empty(const SourceLocation at) {
        return {at, at};
    }

    /**
     * 合并两个 range
     * a.join(b)  ->  [a.begin, b.end)
     */
    [[nodiscard]]
    constexpr SourceRange join(const SourceRange other) const {
        return {this->begin, other.end};
    }

    bool operator==(const SourceRange&) const = default;

    [[nodiscard]]
    constexpr bool is_valid() const {
        return *this != invalid();
    }

private:
    const SourceLocation begin{};
    const SourceLocation end{};
};


/**
 *
 */
struct FileLocation {
    const FileID file_id;
    const std::uint32_t byte_offset;
};

/**
 * 用户可以使用使用 '#line' 设置文件和行号
 */
struct PresumedLocation {
    const std::string filename;
    const std::uint32_t line;
    const std::uint32_t column;
};


}

#endif //STC_SOURCE_LOCATION_H