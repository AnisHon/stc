/**
 * @file memory_buffer.cpp
 * @author anishan
 * @date 2026/11/18
 */

#include "stcc/sys/memory_buffer.h"
#include "stcc/sys/mmap.h"
#include "stcc/sys/page.h"
#include "stcc/sys/platform.h"

#include <algorithm>
#include <gsl/pointers>
#include <gsl/util>
#include <limits>
#include <utility>

namespace stcc::sys {

/* static */
std::expected<MemoryBuffer, std::error_code> MemoryBuffer::open_file(File file) {
    const auto file_size = file.file_sz();
    if (!file_size) {
        return std::unexpected(file_size.error());
    }
    if (*file_size == 0) {
        const gsl::owner<char *> data = new char[1]{'\0'};
        return MemoryBuffer{data, 0, BufferKind::Heap};
    }

    auto mapping = mmap_file(file.native_handle());
    if (!mapping) {
        return std::unexpected(mapping.error());
    }
    const auto unmap_on_exit = gsl::finally([&mapping]() noexcept { unmmap_file(*mapping); });
    const auto size = mapping->sz;
    const auto page = page_size();

    // 只有文件尾部还在最后一页内，才读取系统补出的字节。
    if (page != 0 && size % page != 0 && mapping->begin[size] == '\0') {
        MemoryBuffer buffer{mapping->begin, size, BufferKind::MMap};
#if defined(STCC_PLATFORM_WINDOWS)
        buffer.mapping_handle_ = mapping->handle;
#elif defined(STCC_PLATFORM_POSIX)
        // noting to do
#endif
        *mapping = {};
        return buffer;
    }

    if (size == (std::numeric_limits<std::size_t>::max)()) {
        return std::unexpected(std::make_error_code(std::errc::file_too_large));
    }
    const gsl::owner<char *> data = new char[size + 1];
    std::copy_n(mapping->begin, size, data);
    data[size] = '\0';
    return MemoryBuffer{data, size, BufferKind::Heap};
}

MemoryBuffer::MemoryBuffer(MemoryBuffer &&other) noexcept
    : data_{std::exchange(other.data_, nullptr)},
      size_{std::exchange(other.size_, 0)},
      kind_{std::exchange(other.kind_, BufferKind::None)} {
#if defined(STCC_PLATFORM_WINDOWS)
    mapping_handle_ = std::exchange(other.mapping_handle_, nullptr);
#endif
}

MemoryBuffer &MemoryBuffer::operator=(MemoryBuffer &&other) noexcept {
    if (this == &other) {
        return *this;
    }

    release();
    data_ = std::exchange(other.data_, nullptr);
    size_ = std::exchange(other.size_, 0);
    kind_ = std::exchange(other.kind_, BufferKind::None);
#if defined(STCC_PLATFORM_WINDOWS)
    mapping_handle_ = std::exchange(other.mapping_handle_, nullptr);
#endif

    return *this;
}

MemoryBuffer::~MemoryBuffer() {
    release();
}

std::string_view MemoryBuffer::get_data() const {
    if (!data_) {
        return {};
    }
    return {data_, size_};
}

void MemoryBuffer::release() noexcept {
    switch (kind_) {
    case BufferKind::MMap: {
        MMapResult mapping{
            .begin = data_,
            .sz = size_,
#if defined(STCC_PLATFORM_WINDOWS)
            .handle = std::exchange(mapping_handle_, nullptr),
#endif
        };
        unmmap_file(mapping);
        break;
    }
    case BufferKind::Heap:
        delete[] data_;
        break;
    case BufferKind::None:
        break;
    }
    data_ = nullptr;
    size_ = 0;
    kind_ = BufferKind::None;
}

} // namespace stcc::sys
