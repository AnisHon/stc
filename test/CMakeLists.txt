add_executable(stc_test)
target_sources(stc_test PRIVATE
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
target_link_libraries(stc_test PRIVATE stc_exec_lib Catch2::Catch2WithMain)
include(Catch)

catch_discover_tests(stc_test)  # 自动发现所有 TEST_CASE
