/**
 * @file platform.h
 * @author anishan
 * @date 2026/11/22
 */

#ifndef STCC_PLATFORM_H
#define STCC_PLATFORM_H

namespace stcc::sys {

#if defined(_WIN32)

#  define STCC_PLATFORM_WINDOWS 1

#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) \
    || defined(__NetBSD__)

#  define STCC_PLATFORM_POSIX 1

#else

#  error "Unsupported operating system."

#endif

} // namespace stcc::sys

#endif // STCC_PLATFORM_H
