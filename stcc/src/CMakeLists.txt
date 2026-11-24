# libs
add_library(stcc_lib INTERFACE)
add_library(stcc::lib ALIAS stcc_lib)

# project source code
add_subdirectory(commons)
add_subdirectory(utils)
add_subdirectory(sys)
add_subdirectory(source)
add_subdirectory(lexer)

target_generate_version_header(stcc::lib)
target_link_libraries(
    stcc_lib
    INTERFACE stcc::lexer
              stcc::source
              stcc::utils
              stcc::sys
              stcc::cxx23
)
target_include_directories(stcc_lib INTERFACE ${STCC_INC_DIR})

# executable
add_executable(stcc_exec main.cpp)
target_link_libraries(stcc_exec PRIVATE stcc::lib)
