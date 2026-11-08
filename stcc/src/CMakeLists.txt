# libs
add_library(stcc_exec_lib libs.cpp)
target_link_libraries(stcc_exec_lib PUBLIC stcc_common_deps)
target_link_libraries(stcc_exec_lib PUBLIC stcc_lexer stcc_source stcc_utils)
target_include_directories(stcc_exec_lib PUBLIC ${STCC_INC_DIR})

target_generate_version_header(stcc_exec_lib)

# executable
add_executable(stcc_exec main.cpp)
target_link_libraries(stcc_exec PRIVATE stcc_exec_lib)
