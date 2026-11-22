/**
 * @file memory_buffer.cpp
 * @author anishan
 * @date 2026/11/18
 */

#include "stcc/sys/memory_buffer.h"
#include "stcc/sys/mmap.h"
#include "stcc/sys/page.h"
#include <algorithm>
#include <cstddef>
#include <expected>
#include <gsl/pointers>
#include <limits>
#include <sys/file.h>
#include <utility>

namespace stcc::sys {

/* static */
std::expected<MemoryBuffer, std::error_code> MemoryBuffer::open_file(File file) {
    const auto res = file.file_sz();
    if (!res) {
        return std::unexpected(res.error());
    }

    const auto file_sz = *res;

    // size = 0 mmap 无法映射
    if (file_sz == 0) {
        const gsl::owner<char *> data = new char[1]{'\0'};
        const auto mmap_res = MMapResult{.begin = data, .sz = 0};
        return MemoryBuffer{mmap_res, BufferKind::Malloc};
    }

    // 尝试映射
    const auto mapping = mmap_file(file.native_handle());
    if (!mapping) {
        return std::unexpected(mapping.error());
    }
    MMapResult mapping_result = *mapping;

    // 末尾 0 填充 mapping 可用
    if (file_sz % page_size() != 0) {
        return MemoryBuffer{mapping_result, BufferKind::MMap};
    }

    // 为文件末尾的 NUL 留出一个字节，视图长度仍为文件大小。
    if (file_sz >= (std::numeric_limits<std::size_t>::max)()
        || file_sz > static_cast<std::uintmax_t>((std::numeric_limits<std::streamsize>::max)())) {
        return std::unexpected(std::make_error_code(std::errc::file_too_large));
    }

    gsl::owner<char *> data = new char[file_sz + 1];
    std::copy(mapping_result.begin, mapping_result.begin + mapping_result.sz, data);
    data[file_sz] = '\0';

    // 删除文件映射
    unmmap_file(mapping_result);
    const MMapResult mmap_res{
        .begin = data,
        .sz = file_sz + 1,
    };
    return MemoryBuffer{mmap_res, BufferKind::Malloc};
}

MemoryBuffer::MemoryBuffer(MemoryBuffer &&other) noexcept
    : buffer_{std::exchange(other.buffer_, {})},
      kind_{std::exchange(other.kind_, BufferKind::None)} {
}

MemoryBuffer &MemoryBuffer::operator=(MemoryBuffer &&other) noexcept {
    if (this == &other) {
        return *this;
    }

    release();
    buffer_ = std::exchange(other.buffer_, {});
    kind_ = std::exchange(other.kind_, BufferKind::None);

    return *this;
}

MemoryBuffer::~MemoryBuffer() {
    release();
}

std::string_view MemoryBuffer::get_data() const {
    if (!buffer_.begin) {
        return {};
    }
    return {buffer_.begin, buffer_.sz};
}

void MemoryBuffer::release() noexcept {
    switch (kind_) {
    case BufferKind::MMap:
        unmmap_file(buffer_);
        break;
    case BufferKind::Malloc:
        delete[] buffer_.begin;
        break;
    case BufferKind::None:
        break;
    }
    buffer_ = {};
    kind_ = BufferKind::None;
}

} // namespace stcc::sys
