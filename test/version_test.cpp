/**
 * @file version_test.cpp
 * @author Anis Han
 * @date 2026/6/14
 */

#include <catch2/catch_test_macros.hpp>
#include "stc/utils/version.h"

TEST_CASE("TEST_VERSION", "[version]") {
    REQUIRE(!stc::get_version().empty());
}

