/**
 * @file memory_buffer.h
 * @author anishan
 * @date 2026/11/18
 */

#ifndef STCC_MEMORY_BUFFER_H
#define STCC_MEMORY_BUFFER_H

#include "stcc/sys/file.h"

#include <stdint.h>
#include <cstddef>
#include <expected>
#include <string_view>
#include <system_error>

namespace stcc::sys {

/**
 * 独占文件缓冲区，只能移动。
 * get_data() 的长度等于文件字节数，末尾的 NUL 放在视图之外。
 */
class MemoryBuffer {
    enum class BufferKind : uint8_t {
        None,
        Heap,
        MMap,
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
    MemoryBuffer(const char *data, std::size_t size, BufferKind kind) noexcept
        : data_{data},
          size_{size},
          kind_{kind} {}

    /**
     * 释放当前对象，一旦调用对象将成为 None MemoryBuffer
     */
    void release() noexcept;

    const char *data_;
    std::size_t size_;
    BufferKind kind_;
#if defined(STCC_PLATFORM_WINDOWS)
    HANDLE mapping_handle_{};
#endif
};

} // namespace stcc::sys

#endif // STCC_MEMORY_BUFFER_H
