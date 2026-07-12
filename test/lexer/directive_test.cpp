#include <catch2/catch_test_macros.hpp>

#include "stc/lexer/directive.h"

using stc::preprocessor::DirectiveKind;

TEST_CASE("preprocessor directive names map to their kinds", "[lexer][preprocessor]") {
    REQUIRE(stc::preprocessor::from_string("#define") == DirectiveKind::PpDefine);
    REQUIRE(stc::preprocessor::from_string("#include") == DirectiveKind::PpInclude);
    REQUIRE(stc::preprocessor::from_string("#endif") == DirectiveKind::PpEndIf);
    REQUIRE(stc::preprocessor::from_string("#pragma") == DirectiveKind::PpPragma);
}

TEST_CASE("directive matching requires the complete spelling", "[lexer][preprocessor]") {
    REQUIRE(stc::preprocessor::from_string("define") == DirectiveKind::Invalid);
    REQUIRE(stc::preprocessor::from_string("#define ") == DirectiveKind::Invalid);
    REQUIRE(stc::preprocessor::from_string("#unknown") == DirectiveKind::Invalid);
    REQUIRE(stc::preprocessor::from_string("") == DirectiveKind::Invalid);
}
