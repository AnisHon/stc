/**
 * @file platform.h
 * @author anishan
 * @date 2026/11/22
 */

#ifndef STCC_PLATFORM_H
#define STCC_PLATFORM_H

#if defined(_WIN32)

#  define STCC_PLATFORM_WINDOWS 1

#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) \
    || defined(__NetBSD__)

#  include <fcntl.h>

#  define STCC_PLATFORM_POSIX 1

#  if defined(__linux__)
#    define STCC_PLATFORM_LINUX 1
#  endif

#  if defined(F_GETPATH)
#    define STCC_PLATFORM_HAS_FCNTL_GET_PATH 1
#  endif

#  if defined(F_KINFO)
#    define STCC_PLATFORM_HAS_FCNTL_KINFO 1
#  endif

#else

#  error "Unsupported operating system."

#endif

#endif // STCC_PLATFORM_H
