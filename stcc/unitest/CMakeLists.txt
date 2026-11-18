add_executable(stcc_test)
target_sources(stcc_test PRIVATE
        ./test.cpp
        ./utils/strutils_test.cpp
        ./utils/io_utils_test.cpp
        ./lexer/string_interner.cpp
        ./lexer/directive_test.cpp
        ./lexer/token_kinds_test.cpp
        ./lexer/lexer_test_helper.cpp
        ./source/manager_test.cpp
        #        ${CMAKE_CURRENT_SOURCE_DIR}/
)
target_link_libraries(stcc_test PRIVATE stcc_lib Catch2::Catch2WithMain)
include(Catch)

catch_discover_tests(stcc_test)  # 自动发现所有 TEST_CASE
