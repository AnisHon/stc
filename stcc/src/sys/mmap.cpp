/**
 * @file mmap.cpp
 * @author anishan
 * @date 2026/11/19
 */

#include "stcc/sys/mmap.h"
#include "stcc/sys/platform.h"
#include "stcc/commons/macros.h"

#include <stdint.h>
#include <expected>
#include <limits>
#include <system_error>

#if defined(STCC_PLATFORM_WINDOWS)
#  include <windows.h>
#elif defined(STCC_PLATFORM_POSIX)
#  include <cerrno>
#  include <fcntl.h>
#  include <sys/mman.h>
#  include <sys/stat.h>
#  include <unistd.h>
#endif

namespace stcc::sys {

std::expected<MMapResult, std::error_code> mmap_file(FileT file) noexcept {
    MMapResult result{};

#if defined(STCC_PLATFORM_WINDOWS)
    LARGE_INTEGER file_size{};

    if (!::GetFileSizeEx(file, &file_size)) {
        const auto error
            = std::error_code(static_cast<int>(::GetLastError()), std::system_category());

        return std::unexpected(error);
    }

    if (file_size.QuadPart < 0
        || static_cast<uint64_t>(file_size.QuadPart)
            > (std::numeric_limits<std::size_t>::max)()) {
        return std::unexpected(std::make_error_code(std::errc::file_too_large));
    }

    if (file_size.QuadPart == 0) {
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    }

    HANDLE mapping = ::CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr);

    if (!mapping) {
        const auto error
            = std::error_code(static_cast<int>(::GetLastError()), std::system_category());

        return std::unexpected(error);
    }

    const auto view_size = static_cast<std::size_t>(file_size.QuadPart);
    void *data = ::MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, view_size);

    if (!data) {
        const auto error
            = std::error_code(static_cast<int>(::GetLastError()), std::system_category());

        ::CloseHandle(mapping);
        return std::unexpected(error);
    }

    result.begin = static_cast<const char *>(data);
    result.sz = view_size;
    result.handle = mapping;

#elif defined(STCC_PLATFORM_POSIX)
    struct stat st{};

    if (::fstat(file, &st) == -1) {
        const auto error = std::error_code(errno, std::generic_category());
        return std::unexpected(error);
    }

    if (st.st_size < 0
        || static_cast<uintmax_t>(st.st_size) > std::numeric_limits<std::size_t>::max()) {
        return std::unexpected(std::make_error_code(std::errc::file_too_large));
    }

    if (st.st_size == 0) {
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    }

    const void *const data
        = ::mmap(nullptr, static_cast<std::size_t>(st.st_size), PROT_READ, MAP_PRIVATE, file, 0);

    if (data == MAP_FAILED) {
        const auto error = std::error_code(errno, std::generic_category());
        return std::unexpected(error);
    }

    result.begin = static_cast<const char *>(data);
    result.sz = static_cast<std::size_t>(st.st_size);
#endif

    return result;
}

void unmmap_file(MMapResult &result) noexcept {
    if (!result.begin) {
        result = {};
        return;
    }

#if defined(STCC_PLATFORM_WINDOWS)
    if (!::UnmapViewOfFile(result.begin)) {
        PANIC("UnmapViewOfFile failed, error code {}", ::GetLastError());
    }

    if (!::CloseHandle(result.handle)) {
        PANIC("CloseHandle failed, error code {}", ::GetLastError());
    }
#elif defined(STCC_PLATFORM_POSIX)
    if (::munmap(const_cast<char *>(result.begin), result.sz) == -1) {
        PANIC("unmmap failed, errno: {}", errno);
    }
#endif

    result = {};
}

} // namespace stcc::sys
