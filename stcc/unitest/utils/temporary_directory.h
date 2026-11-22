#ifndef STCC_TEST_TEMPORARY_DIRECTORY_H
#define STCC_TEST_TEMPORARY_DIRECTORY_H

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <string_view>
#include <system_error>

namespace stcc::test {

class TemporaryDirectory {
public:
    TemporaryDirectory() {
        std::random_device random;
        path_ = std::filesystem::temp_directory_path() /
            ("stcc_utils_test_" + std::to_string(random()) + "_" + std::to_string(random()));
        REQUIRE(std::filesystem::create_directory(path_));
    }

    TemporaryDirectory(const TemporaryDirectory &) = delete;
    TemporaryDirectory &operator=(const TemporaryDirectory &) = delete;

    ~TemporaryDirectory() {
        std::error_code error;
        std::filesystem::remove_all(path_, error);
    }

    const std::filesystem::path &path() const { return path_; }

    std::filesystem::path write_file(const std::filesystem::path &name,
                                     const std::string_view bytes) const {
        const auto file = path_ / name;
        std::ofstream output(file, std::ios::binary);
        REQUIRE(output.is_open());
        if (!bytes.empty()) {
            output.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
        }
        output.close();
        REQUIRE(output.good());
        return file;
    }

private:
    std::filesystem::path path_;
};

} // namespace stcc::test

#endif // STCC_TEST_TEMPORARY_DIRECTORY_H
