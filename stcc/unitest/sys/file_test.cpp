#include "stcc/sys/file.h"

#include "../utils/temporary_directory.h"

#include <catch2/catch_test_macros.hpp>

#include <string_view>
#include <system_error>
#include <type_traits>
#include <utility>

using stcc::sys::File;
using stcc::test::TemporaryDirectory;

static_assert(!std::is_copy_constructible_v<File>);
static_assert(!std::is_copy_assignable_v<File>);

TEST_CASE("File reports the exact byte size", "[sys][file][integration]") {
    const TemporaryDirectory directory;
    std::string_view bytes;
    SECTION("a binary file") { bytes = std::string_view{"A\0B\r\n", 5}; }
    SECTION("an empty file") { bytes = {}; }

    const auto file = File::open(directory.write_file("size.bin", bytes));
    REQUIRE(file.has_value());
    const auto size = file->file_sz();
    REQUIRE(size.has_value());
    CHECK(*size == bytes.size());
}

TEST_CASE("File reports a missing path", "[sys][file][integration]") {
    const TemporaryDirectory directory;
    const auto file = File::open(directory.path() / "missing.bin");

    REQUIRE_FALSE(file.has_value());
    CHECK(file.error() == std::errc::no_such_file_or_directory);
}

TEST_CASE("File keys identify files independently of their content",
          "[sys][file][file_key][integration]") {
    const TemporaryDirectory directory;
    const auto path = directory.write_file("first.bin", "same bytes");
    const auto other_path = directory.write_file("second.bin", "same bytes");
    const auto first = File::open(path);
    const auto reopened = File::open(path);
    const auto other = File::open(other_path);
    REQUIRE(first.has_value());
    REQUIRE(reopened.has_value());
    REQUIRE(other.has_value());

    const auto first_key = first->file_key();
    const auto reopened_key = reopened->file_key();
    const auto other_key = other->file_key();
    REQUIRE(first_key.has_value());
    REQUIRE(reopened_key.has_value());
    REQUIRE(other_key.has_value());
    CHECK(*first_key == *reopened_key);
    CHECK_FALSE(*first_key == *other_key);
}

TEST_CASE("File transfers handle ownership when moved", "[sys][file][integration]") {
    const TemporaryDirectory directory;
    const auto source_path = directory.write_file("source.bin", "source");
    const auto target_path = directory.write_file("target.bin", "target data");
    auto target = File::open(target_path);
    REQUIRE(target.has_value());

    {
        auto source = File::open(source_path);
        REQUIRE(source.has_value());
        const auto handle = source->native_handle();
        File moved(std::move(*source));
        CHECK(moved.native_handle() == handle);
        CHECK(source->native_handle() != handle);

        *target = std::move(moved);
        CHECK(target->native_handle() == handle);
        CHECK(moved.native_handle() != handle);
    }

    const auto size = target->file_sz();
    REQUIRE(size.has_value());
    CHECK(*size == std::string_view{"source"}.size());
}
