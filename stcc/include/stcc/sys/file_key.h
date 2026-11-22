/**
 * @file file_key.h
 * @author anishan
 * @date 2026/11/22
 */

#ifndef STCC_FILE_KEY_H
#define STCC_FILE_KEY_H

#include "stcc/sys/platform.h"

#if defined(STCC_PLATFORM_WINDOWS)
#  include <cstring>
#  include <windows.h>
#elif defined(STCC_PLATFORM_POSIX)
#  include <sys/stat.h>
#  include <unistd.h>
#endif

#include <stdint.h>
#include <expected>
#include <functional>
#include <string_view>
#include <system_error>

namespace stcc::sys {

namespace detail {

#if defined(STCC_PLATFORM_WINDOWS)

/**
 * Windows 下的文件唯一标识符 (卷, file id)
 */
struct WindowsFileKey {
    ULONGLONG volume;
    FILE_ID_128 file;

    bool operator==(const WindowsFileKey &rhs) const {
        return volume == rhs.volume
            && std::memcmp(file.Identifier, rhs.file.Identifier, sizeof(file.Identifier)) == 0;
    }

    static std::expected<WindowsFileKey, std::error_code> create(HANDLE handle) {
        FILE_ID_INFO info{};

        if (!::GetFileInformationByHandleEx(handle, FileIdInfo, &info, sizeof(info))) {
            return std::unexpected{
                std::error_code{static_cast<int>(::GetLastError()), std::system_category()}
            };
        }

        return WindowsFileKey{info.VolumeSerialNumber, info.FileId};
    }
};

#elif defined(STCC_PLATFORM_POSIX)

/**
 * Posix 兼容系统的文件唯一标识符 (设备ID, inode)
 */
struct PosixFileKey {
    dev_t device;
    ino_t inode;

    bool operator==(const PosixFileKey &) const = default;

    static std::expected<PosixFileKey, std::error_code> create(int fd);
};

#endif

} // namespace detail

/**
 * FileKey 文件唯一标识符，指向文件唯一
 */
#if defined(STCC_PLATFORM_WINDOWS)
using FileKey = detail::WindowsFileKey;
#elif defined(STCC_PLATFORM_POSIX)
using FileKey = detail::PosixFileKey;
#endif

} // namespace stcc::sys

#if defined(STCC_PLATFORM_WINDOWS)
template <>
struct std::hash<stcc::sys::detail::WindowsFileKey> {
    std::size_t operator()(const stcc::sys::detail::WindowsFileKey &key) const noexcept {
        const std::string_view identifier{
            reinterpret_cast<const char *>(key.file.Identifier),
            sizeof(key.file.Identifier),
        };
        const auto h1 = std::hash<uint64_t>{}(key.volume);
        const auto h2 = std::hash<std::string_view>{}(identifier);

        // 简单的 hash combine
        return h1 ^ (h2 + 0x9e3779b9U + (h1 << 6) + (h1 >> 2));
    }
};
#elif defined(STCC_PLATFORM_POSIX)
template <>
struct std::hash<stcc::sys::detail::PosixFileKey> {
    std::size_t operator()(const stcc::sys::detail::PosixFileKey &key) const noexcept {
        size_t h1 = std::hash<uint64_t>{}(key.device);
        size_t h2 = std::hash<uint64_t>{}(key.inode);

        // 简单的 hash combine
        return h1 ^ (h2 + 0x9e3779b9U + (h1 << 6) + (h1 >> 2));
    }
};
#endif // STCC_PLATFORM_POSIX

#endif // STCC_FILE_KEY_H
