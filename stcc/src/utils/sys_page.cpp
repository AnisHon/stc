/**
 * @file sys_page.cpp
 * @author anishan
 * @date 2026/11/19
 */

#ifdef _WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#endif

#include "sys_page.h"

namespace stcc::utils {

size_t page_size() {
    static const size_t PAGE_SIZE = [] {
#ifdef _WIN32
        SYSTEM_INFO SystemInfo;
        GetSystemInfo(&SystemInfo);
        return SystemInfo.dwAllocationGranularity;
#else
        return sysconf(_SC_PAGE_SIZE);
#endif
    }();
    return PAGE_SIZE;
}

} // namespace stcc::utils
