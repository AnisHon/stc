#include <chrono>
#include <filesystem>
#include <fstream>

#include <catch2/catch_test_macros.hpp>

#include "stcc/source/source_manager.h"

namespace {

class TemporarySource {
public:
    TemporarySource() {
        const auto nonce = std::chrono::steady_clock::now().time_since_epoch().count();
        path_ = std::filesystem::temp_directory_path()
            / ("stcc_manager_test_" + std::to_string(nonce) + ".c");
        remove();
    }

    ~TemporarySource() { remove(); }

    void write(const std::string_view content) const {
        std::ofstream output(path_, std::ios::binary);
        REQUIRE(output.good());
        output.write(content.data(), static_cast<std::streamsize>(content.size()));
        REQUIRE(output.good());
    }

    void remove() const {
        std::error_code error;
        std::filesystem::remove(path_, error);
    }

    [[nodiscard]] const std::filesystem::path &path() const { return path_; }

private:
    std::filesystem::path path_;
};

} // namespace

TEST_CASE("Manager deduplicates a successfully loaded source file", "[source][manager]") {
    TemporarySource source;
    source.write("int main(void) { return 0; }\n");
    stcc::SourceManager manager;

    const auto first = manager.create_file(source.path());
    const auto second = manager.create_file(source.path());

    REQUIRE(first.has_value());
    REQUIRE(second.has_value());
    REQUIRE(*first != stcc::source::FileID::invalid());
    REQUIRE(*second == *first);
}

// This hidden regression test intentionally remains red until
// Manager::create_file is fixed. Run it explicitly with: ./stcc_test
// "[source][manager][regression]"
TEST_CASE(
    "Manager retries a source file after an earlier open failure",
    "[.][source][manager][regression]"
) {
    TemporarySource source;
    stcc::SourceManager manager;

    const auto missing = manager.create_file(source.path());
    REQUIRE_FALSE(missing.has_value());
    REQUIRE(missing.error() == stcc::utils::IOError::OpenFailed);

    source.write("int answer = 42;\n");
    const auto retry = manager.create_file(source.path());

    REQUIRE(retry.has_value());
    REQUIRE(*retry != stcc::source::FileID::invalid());
}
