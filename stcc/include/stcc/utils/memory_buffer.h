/**
 * @file memory_buffer.h
 * @author anishan
 * @date 2026/11/18
 */

#ifndef STCC_MEMORY_BUFFER_H
#define STCC_MEMORY_BUFFER_H
#include <expected>
#include <filesystem>
#include <system_error>
#include <stdint.h>
#include <gsl/pointers>
#include <utility>

namespace stcc::utils {

/**
 * 读取文件，自动决定 malloc 或 mmap
 * 独占语义，只能移动不能拷贝
 * 数据范围 [begin_, end_)，end_ 是尾后指针，不能用来解引用。
 */
class MemoryBuffer {
    enum class BufferKind : uint8_t {
        None,
        MMap,
        Malloc,
    };

public:
    static std::expected<MemoryBuffer, std::error_code> open_file(
        const std::filesystem::path &path
        );

    MemoryBuffer(const MemoryBuffer &) = delete;
    MemoryBuffer &operator=(const MemoryBuffer &) = delete;

    MemoryBuffer(MemoryBuffer &&other) noexcept
        : begin_{std::exchange(other.begin_, nullptr)},
          end_{std::exchange(other.end_, nullptr)},
          kind_{std::exchange(other.kind_, BufferKind::None)} {
    };
    MemoryBuffer &operator=(MemoryBuffer &&other) noexcept;

    ~MemoryBuffer();

    std::string_view get_data() const;

private:
    MemoryBuffer(gsl::owner<const char *> begin, const char *end, const BufferKind kind)
        : begin_(begin),
          end_(end),
          kind_(kind) {
    };

    /**
     * 释放当前对象，一旦调用对象将成为 None MemoryBuffer
     */
    void release();

    gsl::owner<const char *> begin_;
    const char *end_;
    BufferKind kind_;
};

} // namespace stcc::utils

#endif // STCC_MEMORY_BUFFER_H