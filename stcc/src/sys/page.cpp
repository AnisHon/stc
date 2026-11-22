/**
 * @file page.cpp
 * @author anishan
 * @date 2026/11/19
 */

#include "stcc/sys/page.h"
#include "stcc/sys/platform.h"

#if defined(STCC_PLATFORM_WINDOWS)
#  include <windows.h>
#elif defined(STCC_PLATFORM_POSIX)
#  include <unistd.h>
#endif

namespace stcc::sys {

size_t page_size() {
    static const size_t PAGE_SIZE = [] {
#if defined(STCC_PLATFORM_WINDOWS)
        SYSTEM_INFO SystemInfo;
        GetSystemInfo(&SystemInfo);
        return SystemInfo.dwAllocationGranularity;
#elif defined(STCC_PLATFORM_POSIX)
        return sysconf(_SC_PAGE_SIZE);
#endif
    }();
    return PAGE_SIZE;
}

} // namespace stcc::sys
