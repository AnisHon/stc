# libs
add_library(stcc_lib INTERFACE)
add_library(stcc::lib ALIAS stcc_lib)

target_link_libraries(stcc_lib INTERFACE stcc::lexer stcc::source stcc::utils)
target_include_directories(stcc_lib INTERFACE ${STCC_INC_DIR})

target_generate_version_header(stcc::lib)

# executable
add_executable(stcc_exec main.cpp)
target_link_libraries(stcc_exec PRIVATE stcc::lib)
