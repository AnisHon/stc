#include <catch2/catch_test_macros.hpp>

#include "stcc/lexer/directive.h"

using stcc::pp::DirectiveKind;

TEST_CASE("pp directive names map to their kinds", "[lexer][pp]") {
    REQUIRE(stcc::pp::from_string("#define") == DirectiveKind::PpDefine);
    REQUIRE(stcc::pp::from_string("#include") == DirectiveKind::PpInclude);
    REQUIRE(stcc::pp::from_string("#endif") == DirectiveKind::PpEndIf);
    REQUIRE(stcc::pp::from_string("#pragma") == DirectiveKind::PpPragma);
}

TEST_CASE("directive matching requires the complete spelling", "[lexer][pp]") {
    REQUIRE(stcc::pp::from_string("define") == DirectiveKind::Invalid);
    REQUIRE(stcc::pp::from_string("#define ") == DirectiveKind::Invalid);
    REQUIRE(stcc::pp::from_string("#unknown") == DirectiveKind::Invalid);
    REQUIRE(stcc::pp::from_string("") == DirectiveKind::Invalid);
}
