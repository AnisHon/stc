add_executable(stcc_test test.cpp)

add_subdirectory(utils)
add_subdirectory(sys)
add_subdirectory(lexer)
add_subdirectory(source)

target_link_libraries(stcc_test PRIVATE stcc_lib Catch2::Catch2WithMain)
include(Catch)

catch_discover_tests(stcc_test)  # 自动发现所有 TEST_CASE
