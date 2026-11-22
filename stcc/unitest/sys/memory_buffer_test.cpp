#include "stcc/sys/memory_buffer.h"

#include "stcc/sys/file.h"
#include "stcc/sys/page.h"
#include "../utils/temporary_directory.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

using stcc::test::TemporaryDirectory;
using stcc::sys::File;
using stcc::sys::MemoryBuffer;
using stcc::sys::page_size;

static_assert(!std::is_copy_constructible_v<MemoryBuffer>);
static_assert(!std::is_copy_assignable_v<MemoryBuffer>);

namespace {

auto open_buffer(const std::filesystem::path &path) {
    auto file = File::open(path);
    REQUIRE(file.has_value());
    return MemoryBuffer::open_file(std::move(*file));
}

} // namespace

TEST_CASE("MemoryBuffer reads and releases a mapped file", "[sys][memory_buffer][integration]") {
    const TemporaryDirectory directory;
    constexpr std::string_view expected{"a\0b\n", 4};
    REQUIRE(page_size() > expected.size());
    const auto path = directory.write_file("mapped.bin", expected);

    const auto buffer = open_buffer(path);
    REQUIRE(buffer.has_value());
    CHECK(buffer->get_data() == expected);
}

TEST_CASE("MemoryBuffer exposes exactly the bytes of a page-aligned file",
          "[sys][memory_buffer][integration]") {
    const TemporaryDirectory directory;
    const auto alignment = page_size();
    REQUIRE(alignment > 0);
    std::string expected(alignment, 'x');
    expected.front() = '\0';
    const auto path = directory.write_file("aligned.bin", expected);

    const auto buffer = open_buffer(path);
    REQUIRE(buffer.has_value());
    const auto data = buffer->get_data();
    CHECK(data.size() == expected.size());
    CHECK(data.substr(0, expected.size()) == expected);
}

TEST_CASE("MemoryBuffer exposes an empty view for an empty file",
          "[sys][memory_buffer][integration]") {
    const TemporaryDirectory directory;
    const auto path = directory.write_file("empty.bin", {});

    const auto buffer = open_buffer(path);
    REQUIRE(buffer.has_value());
    CHECK(buffer->get_data().empty());
}

TEST_CASE("MemoryBuffer transfers ownership on move construction and assignment",
          "[sys][memory_buffer][integration]") {
    const TemporaryDirectory directory;
    const auto alignment = page_size();
    REQUIRE(alignment > 0);
    const std::string expected(alignment, 's');
    const auto source_path = directory.write_file("source.bin", expected);
    const auto target_path = directory.write_file("target.bin", std::string(alignment, 't'));

    auto target = open_buffer(target_path);
    REQUIRE(target.has_value());
    {
        auto source = open_buffer(source_path);
        REQUIRE(source.has_value());
        const auto original = source->get_data();
        const void *original_address = original.data();

        MemoryBuffer moved(std::move(*source));
        CHECK(static_cast<const void *>(moved.get_data().data()) == original_address);
        CHECK(moved.get_data().size() == original.size());

        *target = std::move(moved);
        CHECK(static_cast<const void *>(target->get_data().data()) == original_address);
        CHECK(target->get_data().size() == original.size());
    }
    CHECK(target->get_data().substr(0, expected.size()) == expected);
}
