enable_testing()
add_executable(stc_test
)

target_sources(stc_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/strutils_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/unicode_utils_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lexer/string_interner.cpp
        #        ${CMAKE_CURRENT_SOURCE_DIR}/

)

target_link_libraries(stc_test PRIVATE stc_exec_lib Catch2::Catch2WithMain)

include(CTest)
add_test(NAME auto_test COMMAND test)