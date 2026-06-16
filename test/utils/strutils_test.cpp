#include "stc/utils/strutils.h"

#include <fmt/ranges.h>
#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;
using namespace stc::utils;

TEST_CASE("get_line_start_indices_test", "[strutils]") {
    constexpr std::array test_cases{
        u8"12\r\n3"sv,
        u8"12\r3"sv,
        u8R"(1
2
3
4)"sv,
        u8R"(   1 int main() {
                2   std::cout << "Hello World" << std::endl; // 😅
                3   return 0;
                4 })"sv,
    };
    std::array<std::vector<std::uint32_t>, 4> test_answers{{
        {0, 4}, {0, 3}, {0, 2, 4, 6}, {0, 18, 87, 117},
    }};

    for (int i = 0; i < test_answers.size(); ++i) {
        auto result = get_line_start_indices(test_cases[i]);
        REQUIRE(result == test_answers[i]);
    }
}