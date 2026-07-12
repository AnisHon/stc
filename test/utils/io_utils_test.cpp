#include <chrono>
#include <filesystem>
#include <fstream>

#include <catch2/catch_test_macros.hpp>

#include "stc/utils/io_utils.h"

namespace {

class TemporaryFile {
public:
    explicit TemporaryFile(const std::string_view suffix) {
        const auto nonce = std::chrono::steady_clock::now().time_since_epoch().count();
        path_ = std::filesystem::temp_directory_path() /
                ("stc_io_test_" + std::to_string(nonce) + std::string(suffix));
    }

    ~TemporaryFile() {
        std::error_code error;
        std::filesystem::remove(path_, error);
    }

    [[nodiscard]] const std::filesystem::path& path() const { return path_; }

private:
    std::filesystem::path path_;
};

} // namespace

TEST_CASE("read_u8string preserves valid UTF-8 bytes", "[utils][io]") {
    TemporaryFile file("_valid.c");
    const std::u8string expected = u8"int \u53d8\u91cf = 42;\n";
    {
        std::ofstream output(file.path(), std::ios::binary);
        output.write(reinterpret_cast<const char*>(expected.data()),
                     static_cast<std::streamsize>(expected.size()));
    }

    const auto result = stc::utils::read_u8string(file.path());

    REQUIRE(result.has_value());
    REQUIRE(*result == expected);
}

TEST_CASE("read_u8string rejects invalid UTF-8", "[utils][io]") {
    TemporaryFile file("_invalid.c");
    {
        std::ofstream output(file.path(), std::ios::binary);
        constexpr char invalid_utf8[] = {static_cast<char>(0xC3), static_cast<char>(0x28)};
        output.write(invalid_utf8, sizeof(invalid_utf8));
    }

    const auto result = stc::utils::read_u8string(file.path());

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == stc::utils::IOError::InvalidUtf8);
}

TEST_CASE("read_u8string reports a missing file", "[utils][io]") {
    TemporaryFile file("_missing.c");

    const auto result = stc::utils::read_u8string(file.path());

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == stc::utils::IOError::OpenFailed);
}
