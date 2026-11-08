/**
 * @file string_interner.cpp
 * @author Anis Han
 * @date 2026/6/16
 */

#include <string_view>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "stcc/lexer/string_interner.h"

using namespace std::string_view_literals;

TEST_CASE("string_interner", "[lexer]") {
    const std::string_view view = GENERATE("123"sv, "if"sv, "\"😄😅\""sv, "你好世界"sv);
    stcc::StringInterner intern;

    char str[] = "a";
    auto str1 = intern.intern(view);
    auto str2 = intern.intern(view);
    REQUIRE(str1 == str2);

    for (char8_t c = 'a'; c <= 'z'; c++) {
        str[0] = c;
        intern.intern(view);
        auto str3 = intern.intern(str);
        REQUIRE(str3 != str2);
    }
}
