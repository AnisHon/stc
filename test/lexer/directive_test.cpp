#include <catch2/catch_test_macros.hpp>

#include "stc/lexer/directive.h"

using stc::pp::DirectiveKind;

TEST_CASE("pp directive names map to their kinds", "[lexer][pp]") {
    REQUIRE(stc::pp::from_string("#define") == DirectiveKind::PpDefine);
    REQUIRE(stc::pp::from_string("#include") == DirectiveKind::PpInclude);
    REQUIRE(stc::pp::from_string("#endif") == DirectiveKind::PpEndIf);
    REQUIRE(stc::pp::from_string("#pragma") == DirectiveKind::PpPragma);
}

TEST_CASE("directive matching requires the complete spelling", "[lexer][pp]") {
    REQUIRE(stc::pp::from_string("define") == DirectiveKind::Invalid);
    REQUIRE(stc::pp::from_string("#define ") == DirectiveKind::Invalid);
    REQUIRE(stc::pp::from_string("#unknown") == DirectiveKind::Invalid);
    REQUIRE(stc::pp::from_string("") == DirectiveKind::Invalid);
}
