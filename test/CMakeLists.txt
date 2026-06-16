enable_testing()
add_executable(stc_test)

target_sources(stc_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/strutils_test.cpp
)

target_link_libraries(stc_test PRIVATE stc_exec_lib Catch2::Catch2WithMain)

#add_test(NAME stc_tests COMMAND stc_test)

include(CTest)
add_test(NAME auto_test COMMAND test)