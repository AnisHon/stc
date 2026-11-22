/**
 * @file file.cpp
 * @author anishan
 * @date 2026/11/22
 */

#include "stcc/sys/file.h"
#include <expected>
#include <system_error>
#include <utility>

#ifdef STCC_PLATFORM_POSIX
#  include <sys/stat.h>
#  include <fcntl.h>
#  include <unistd.h>
#endif

namespace stcc::sys {

namespace {

#if defined(STCC_PLATFORM_WINDOWS)
const FileT invalid_file = INVALID_HANDLE_VALUE;
#elif defined(STCC_PLATFORM_POSIX)
constexpr FileT invalid_file = -1;
#endif

} // namespace

/* static */ std::expected<File, std::error_code> File::open(const std::filesystem::path &path) {
    FileT fd{};
#if defined(STCC_PLATFORM_WINDOWS)
    fd = ::CreateFileW(
        path.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (fd == INVALID_HANDLE_VALUE) {
        return std::unexpected(
            std::error_code(static_cast<int>(::GetLastError()), std::system_category())
        );
    }

#elif defined(STCC_PLATFORM_POSIX)

    fd = ::open(path.c_str(), O_RDONLY);

    if (fd == -1) {
        return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    }

#endif
    return File(fd);
}

File::File(File &&other) noexcept
    : fd_{std::exchange(other.fd_, invalid_file)} {
}

File &File::operator=(File &&other) noexcept {
    if (this != &other) {
        close();
        fd_ = std::exchange(other.fd_, invalid_file);
    }
    return *this;
}

File::~File() {
    close();
}

void File::close() noexcept {
    const auto fd = std::exchange(fd_, invalid_file);
    if (fd == invalid_file) {
        return;
    }
#if defined(STCC_PLATFORM_WINDOWS)
    ::CloseHandle(fd);
#elif defined(STCC_PLATFORM_POSIX)
    ::close(fd);
#endif
}

std::expected<FileKey, std::error_code> File::file_key() const {
#if defined(STCC_PLATFORM_WINDOWS)
    return detail::WindowsFileKey::create(this->fd_);
#elif defined(STCC_PLATFORM_POSIX)
    return detail::PosixFileKey::create(this->fd_);
#endif
}

std::expected<size_t, std::error_code> File::file_sz() const {
    size_t file_size{};
#if defined(STCC_PLATFORM_WINDOWS)

    LARGE_INTEGER size{};

    if (!::GetFileSizeEx(this->fd_, &size)) {
        return std::unexpected(
            std::error_code(static_cast<int>(::GetLastError()), std::system_category())
        );
    }

    file_size = static_cast<size_t>(size.QuadPart);
#elif defined(STCC_PLATFORM_POSIX)
    struct stat st{};

    if (::fstat(this->fd_, &st) == -1) {
        return std::unexpected(std::error_code(errno, std::generic_category()));
    }

    file_size = static_cast<size_t>(st.st_size);
#endif
    return file_size;
}
} // namespace stcc::sys
