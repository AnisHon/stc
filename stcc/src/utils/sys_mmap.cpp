/**
 * @file sys_mmap.cpp
 * @author anishan
 * @date 2026/11/19
 */

#include "sys_mmap.h"
#include "stcc/utils/macros.h"

#include <cstdint>
#include <limits>

#ifdef _WIN32
#include <Windows.h>
#else
#include <cerrno>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace stcc::utils {
MMapResult mmap_file(
    const std::filesystem::path &path
    ) noexcept {
    MMapResult result;

#ifdef _WIN32
    HANDLE file = ::CreateFileW(
        path.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
        );

    if (file == INVALID_HANDLE_VALUE) {
        PANIC(
            "failed to open file: {}, error: {}",
            path.string(),
            ::GetLastError()
            );
    }

    LARGE_INTEGER file_size{};

    if (!::GetFileSizeEx(file, &file_size)) {
        const auto error = ::GetLastError();
        ::CloseHandle(file);

        PANIC(
            "failed to get file size: {}, error: {}",
            path.string(),
            error
            );
    }

    if (file_size.QuadPart < 0 ||
        static_cast<std::uint64_t>(file_size.QuadPart) >
        std::numeric_limits<std::size_t>::max()) {
        ::CloseHandle(file);

        PANIC("file_too_large: {}", file_size.QuadPart);
    }

    if (file_size.QuadPart == 0) {
        ::CloseHandle(file);
        TODO("don't know");
    }

    HANDLE mapping = ::CreateFileMappingW(
        file,
        nullptr,
        PAGE_READONLY,
        0,
        0,
        nullptr
        );

    if (!mapping) {
        const auto error = ::GetLastError();
        ::CloseHandle(file);

        PANIC(
            "CreateFileMapping failed, error code {}",
            error
            );
    }

    // 映射对象创建成功后，可以关闭原始文件句柄
    ::CloseHandle(file);

    void *data = ::MapViewOfFile(
        mapping,
        FILE_MAP_READ,
        0,
        0,
        0
        );

    if (!data) {
        const auto error = ::GetLastError();
        ::CloseHandle(mapping);

        PANIC(
            "MapViewOfFile failed, error code {}",
            error
            );
    }

    result.begin = static_cast<const char *>(data);
    result.sz = static_cast<std::size_t>(file_size.QuadPart);
    result.mapping_handle = mapping;

#else
    const int fd = ::open(path.c_str(), O_RDONLY);

    if (fd == -1) {
        PANIC(
            "failed to open file: {}, errno: {}",
            path.string(),
            errno
            );
    }

    struct stat st{};

    if (::fstat(fd, &st) == -1) {
        const int error = errno;
        ::close(fd);

        PANIC(
            "failed to load stat: {}, errno: {}",
            path.string(),
            error
            );
    }

    if (st.st_size < 0 ||
        static_cast<std::uintmax_t>(st.st_size) >
        std::numeric_limits<std::size_t>::max()) {
        ::close(fd);

        PANIC("file_too_large: {}", st.st_size);
    }

    if (st.st_size == 0) {
        ::close(fd);
        TODO("don't know");
    }

    const void *const data = ::mmap(
        nullptr,
        static_cast<std::size_t>(st.st_size),
        PROT_READ,
        MAP_PRIVATE,
        fd,
        0
        );

    const int error = data == MAP_FAILED ? errno : 0;

    // mmap 成功后不再需要文件描述符
    ::close(fd);

    if (data == MAP_FAILED) {
        PANIC(
            "mmap failed, error code {}",
            error
            );
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

#ifdef _WIN32
    if (!::UnmapViewOfFile(result.begin)) {
        PANIC(
            "UnmapViewOfFile failed, error code {}",
            ::GetLastError()
            );
    }

    if (!::CloseHandle(result.mapping_handle)) {
        PANIC(
            "CloseHandle failed, error code {}",
            ::GetLastError()
            );
    }
#else
    if (::munmap(
        const_cast<char *>(result.begin),
        result.sz
        ) == -1) {
        PANIC(
            "unmmap failed, errno: {}",
            errno
            );
    }
#endif

    result = {};
}


} // namespace stcc::utils