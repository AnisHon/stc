# libs
add_library(stc_exec_lib libs.cpp)
target_link_libraries(stc_exec_lib PUBLIC stc_dependencies)
target_link_libraries(stc_exec_lib PUBLIC stc_lexer stc_source stc_preprocessor stc_utils)
target_include_directories(stc_exec_lib PUBLIC ${STC_INC_DIR})

# executable
add_executable(stc_exec main.cpp)
target_link_libraries(stc_exec PRIVATE stc_exec_lib)
