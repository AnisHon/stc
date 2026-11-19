/**
 * @file memory_buffer.cpp
 * @author anishan
 * @date 2026/11/18
 */

#include "sys_page.h"
#include "sys_mmap.h"
#include "stcc/utils/macros.h"
#include "stcc/utils/memory_buffer.h"

#include <cstddef>
#include <fstream>
#include <ios>
#include <gsl/pointers>
#include <string_view>

namespace stcc::utils {

namespace {

struct PtrPair {
    const char *begin;
    const char *end;
};

/**
 * 会自动在后面多申请一个字节0填充
 */
PtrPair do_malloc(const std::filesystem::path &path, const size_t file_size) {
    ASSERT(std::filesystem::exists(path));
    ASSERT(file_size < std::numeric_limits<std::streamsize>::max());

    // clang tidy, shutdown the fuck up
    const auto file_size_casted{static_cast<std::streamsize>(file_size)};
    const std::streamsize malloc_size{file_size_casted + 1};

    std::ifstream ifs(path, std::ios::binary);

    char *const begin = new char[malloc_size]; // 没必要用 gsl

    ifs.read(begin, file_size_casted);

    return {.begin = begin, .end = begin + malloc_size};
}

PtrPair do_mmap(const std::filesystem::path &path) {
    ASSERT(std::filesystem::exists(path));
    const auto [begin, sz] = mmap_file(path);
    return {.begin = begin, .end = begin + sz};
}

} // namespace

/* static */
std::expected<MemoryBuffer, std::error_code> MemoryBuffer::open_file(
    const std::filesystem::path &path
    ) {
    std::error_code ec;
    auto file_sz = std::filesystem::file_size(path, ec);

    if (ec) {
        return std::unexpected(ec);
    }

    const auto page_sz{page_size()};
    PtrPair pair;
    auto kind = BufferKind::None;

    if (file_sz % page_sz == 0) {
        pair = do_malloc(path, file_sz);
        kind = BufferKind::Malloc;
    } else {
        pair = do_mmap(path);
        kind = BufferKind::MMap;
    }
    return MemoryBuffer{pair.begin, pair.end, kind};
}

MemoryBuffer &MemoryBuffer::operator=(MemoryBuffer &&other) noexcept {
    if (this == &other) {
        return *this;
    }

    // 释放自己
    this->release();

    std::swap(other.begin_, this->begin_);
    std::swap(other.end_, this->end_);
    std::swap(other.kind_, this->kind_);

    return *this;
}

MemoryBuffer::~MemoryBuffer() {
    this->release();
}

std::string_view MemoryBuffer::get_data() const {
    return std::string_view{this->begin_, this->end_};
}

void MemoryBuffer::release() {
    MMapResult result{
        .begin = this->begin_,
        .sz = static_cast<size_t>(this->begin_ - this->end_)
    };
    switch (this->kind_) {
    case BufferKind::MMap:
        unmmap_file(result);
        break;
    case BufferKind::Malloc:
        delete[] this->begin_;
        break;
    case BufferKind::None: // noting to do
        break;
    }
    this->begin_ = nullptr;
    this->end_ = nullptr;
    this->kind_ = BufferKind::None;
}

} // namespace stcc::utils