/**
 * @file version_test.cpp
 * @author Anis Han
 * @date 2026/6/14
 */
#define CATCH_CONFIG_MAIN  // 只需在一个 .cpp 文件中定义 main 函数
#include <catch2/catch_test_macros.hpp>

int add(int a, int b) {
    return a + b;
}

TEST_CASE("Addition works correctly", "[math]") {
    REQUIRE(add(2, 2) == 4);
    REQUIRE(add(-1, 1) == 0);
}
