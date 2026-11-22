/**
 * @file memory_buffer.h
 * @author anishan
 * @date 2026/11/18
 */

#ifndef STCC_MEMORY_BUFFER_H
#define STCC_MEMORY_BUFFER_H

#include "stcc/sys/mmap.h"

#include <cstdint>
#include <expected>
#include <string_view>
#include <system_error>

namespace stcc::sys {

/**
 * 读取文件，自动决定 malloc 或 mmap
 * 独占语义，只能移动不能拷贝
 * get_data() 返回的视图只包含文件数据。
 */
class MemoryBuffer {
    enum class BufferKind : std::uint8_t {
        None,
        MMap,
        Malloc,
    };

public:
    static std::expected<MemoryBuffer, std::error_code> open_file(File file);

    MemoryBuffer(const MemoryBuffer &) = delete;
    MemoryBuffer &operator=(const MemoryBuffer &) = delete;

    MemoryBuffer(MemoryBuffer &&other) noexcept;
    MemoryBuffer &operator=(MemoryBuffer &&other) noexcept;

    ~MemoryBuffer();

    std::string_view get_data() const;

private:
    MemoryBuffer(MMapResult buffer, BufferKind kind) noexcept
        : buffer_{buffer},
          kind_{kind} {}

    /**
     * 释放当前对象，一旦调用对象将成为 None MemoryBuffer
     */
    void release() noexcept;

    MMapResult buffer_;
    BufferKind kind_;
};

} // namespace stcc::sys

#endif // STCC_MEMORY_BUFFER_H
