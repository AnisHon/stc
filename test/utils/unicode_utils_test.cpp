/**
 * @file unicode_utils_test.cpp
 * @author anishan
 * @date 2026/6/17
 */


#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <fmt/format.h>
#include <fmt/std.h>

#include "stc/utils/unicode_utils.h"

TEST_CASE("has_unicode_class_test", "[unicode_utils]") {
    using stc::utils::gen::UnicodeClass;
    struct TestCase {
        char32_t chr;
        bool is_start;
        bool is_continue;
    };

    std::array<TestCase, 5> test_cases = {{
        {U'0', false, true},
        {U'_', false, true},
        {U'a', true, true},
        {U'测', true, true},
        {U'😄', false, false}
    }};

    for (const auto& [chr, is_start, is_continue] : test_cases) {
        const bool res1 = stc::utils::has_unicode_class(chr,
                                                        static_cast<uint32_t>(
                                                            UnicodeClass::XIDStart));
        const bool res2 = stc::utils::has_unicode_class(chr,
                                                        static_cast<uint32_t>(
                                                            UnicodeClass::XIDContinue));

        fmt::print("U+{:04X}: res1={}, res2={}, expected1={}, expected2={}\n",
                   static_cast<uint32_t>(chr),
                   res1,
                   res2,
                   is_start,
                   is_continue);

        REQUIRE(res1 == is_start);
        REQUIRE(res2 == is_continue);
    }
}