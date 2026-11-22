#include "stcc/sys/file_key.h"
#include "stcc/sys/platform.h"

namespace stcc::sys {

namespace detail {

#if defined(STCC_PLATFORM_POSIX)

/* static */ std::expected<PosixFileKey, std::error_code> PosixFileKey::create(int fd) {
    struct stat st{};
    if (::fstat(fd, &st) != 0) {
        return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    }

    return PosixFileKey{st.st_dev, st.st_ino};
}

#endif

} // namespace detail

} // namespace stcc::sys
