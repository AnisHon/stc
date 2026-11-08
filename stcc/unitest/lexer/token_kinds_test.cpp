#include "stcc/lexer/lexer.h"

#include <catch2/catch_test_macros.hpp>

#include "stcc/lexer/token_kinds.h"

using stcc::PPTokenKind;

TEST_CASE("punctuator metadata is internally consistent", "[lexer][token-kind]") {
    REQUIRE(stcc::to_string(PPTokenKind::ShiftLeftEqual) == "ShiftLeftEqual");
    REQUIRE(stcc::to_symbol(PPTokenKind::ShiftLeftEqual) == "<<=");
    REQUIRE(stcc::to_symbol(PPTokenKind::HashHashAlt) == "%:%:");
    REQUIRE(stcc::is_preprocessor(PPTokenKind::Hash));
    REQUIRE(stcc::is_preprocessor(PPTokenKind::HashHash));
    REQUIRE_FALSE(stcc::is_preprocessor(PPTokenKind::Plus));
}
