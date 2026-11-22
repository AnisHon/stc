#include "stcc/sys/mmap.h"

#include "stcc/sys/file.h"
#include "../utils/temporary_directory.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <string_view>
#include <system_error>

namespace {

struct ScopedMapping {
    explicit ScopedMapping(const std::filesystem::path &path) {
        const auto file = stcc::sys::File::open(path);
        REQUIRE(file.has_value());
        const auto mapping = stcc::sys::mmap_file(file->native_handle());
        REQUIRE(mapping.has_value());
        result = *mapping;
    }

    ScopedMapping(const ScopedMapping &) = delete;
    ScopedMapping &operator=(const ScopedMapping &) = delete;

    ~ScopedMapping() { stcc::sys::unmmap_file(result); }

    stcc::sys::MMapResult result;
};

} // namespace

TEST_CASE("mmap preserves binary data and clears the result on unmap",
          "[sys][mmap][integration]") {
    const stcc::test::TemporaryDirectory directory;
    constexpr std::string_view expected{"A\0B\r\n", 5};
    const auto path = directory.write_file("input.bin", expected);

    ScopedMapping mapping(path);
    auto &result = mapping.result;
    REQUIRE(static_cast<const void *>(result.begin) != nullptr);
    REQUIRE(result.sz == expected.size());
    const std::string_view bytes{result.begin, result.sz};
    CHECK(bytes == expected);

    stcc::sys::unmmap_file(result);
    CHECK(static_cast<const void *>(result.begin) == nullptr);
    CHECK(result.sz == 0);
}

TEST_CASE("mmap accepts unmapping a default result", "[sys][mmap]") {
    stcc::sys::MMapResult result{};
    stcc::sys::unmmap_file(result);

    CHECK(static_cast<const void *>(result.begin) == nullptr);
    CHECK(result.sz == 0);
}

TEST_CASE("mmap reports an error for an empty file", "[sys][mmap][integration]") {
    const stcc::test::TemporaryDirectory directory;
    const auto file = stcc::sys::File::open(directory.write_file("empty.bin", {}));
    REQUIRE(file.has_value());

    auto mapping = stcc::sys::mmap_file(file->native_handle());
    if (mapping) {
        stcc::sys::unmmap_file(*mapping);
    }
    REQUIRE_FALSE(mapping.has_value());
    CHECK(mapping.error() == std::errc::invalid_argument);
}
