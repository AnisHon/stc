#include "stc/lexer/lexer.h"

#include <catch2/catch_test_macros.hpp>

#include "stc/lexer/token_kinds.h"

using stc::PPTokenKind;

TEST_CASE("punctuator metadata is internally consistent", "[lexer][token-kind]") {
    REQUIRE(stc::to_string(PPTokenKind::ShiftLeftEqual) == "ShiftLeftEqual");
    REQUIRE(stc::to_symbol(PPTokenKind::ShiftLeftEqual) == "<<=");
    REQUIRE(stc::to_symbol(PPTokenKind::HashHashAlt) == "%:%:");
    REQUIRE(stc::is_preprocessor(PPTokenKind::Hash));
    REQUIRE(stc::is_preprocessor(PPTokenKind::HashHash));
    REQUIRE_FALSE(stc::is_preprocessor(PPTokenKind::Plus));
}
