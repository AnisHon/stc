#include "stcc/sys/page.h"
#include "stcc/sys/platform.h"

#include <catch2/catch_test_macros.hpp>

#include <cstddef>

#if defined(STCC_PLATFORM_WINDOWS)
#  include <windows.h>
#elif defined(STCC_PLATFORM_POSIX)
#  include <unistd.h>
#endif

TEST_CASE("page_size reports the OS page size", "[sys][page][integration]") {
#if defined(STCC_PLATFORM_WINDOWS)
    SYSTEM_INFO info{};
    ::GetSystemInfo(&info);
    const auto expected = info.dwPageSize;
#elif defined(STCC_PLATFORM_POSIX)
    const auto expected = ::sysconf(_SC_PAGE_SIZE);
#endif

    REQUIRE(expected > 0);
    CHECK(stcc::sys::page_size() == static_cast<std::size_t>(expected));
}
