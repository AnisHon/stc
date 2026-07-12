#include <catch2/catch_test_macros.hpp>

#include "stc/lexer/token_kinds.h"

using stc::lexer::PPTokenKind;

TEST_CASE("punctuator metadata is internally consistent", "[lexer][token-kind]") {
    REQUIRE(stc::lexer::to_string(PPTokenKind::ShiftLeftEqual) == "ShiftLeftEqual");
    REQUIRE(stc::lexer::to_symbol(PPTokenKind::ShiftLeftEqual) == "<<=");
    REQUIRE(stc::lexer::to_symbol(PPTokenKind::HashHashAlt) == "%:%:");
    REQUIRE(stc::lexer::is_preprocessor(PPTokenKind::Hash));
    REQUIRE(stc::lexer::is_preprocessor(PPTokenKind::HashHash));
    REQUIRE_FALSE(stc::lexer::is_preprocessor(PPTokenKind::Plus));
}

TEST_CASE("punctuator starts cover C punctuators only", "[lexer][token-kind]") {
    REQUIRE(stc::lexer::is_punctuators_start(U'#'));
    REQUIRE(stc::lexer::is_punctuators_start(U'{'));
    REQUIRE(stc::lexer::is_punctuators_start(U'.'));
    REQUIRE_FALSE(stc::lexer::is_punctuators_start(U'a'));
    REQUIRE_FALSE(stc::lexer::is_punctuators_start(U'0'));
}
