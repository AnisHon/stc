cmake_minimum_required(VERSION 3.20)

project(stcc VERSION 11.45.14)
set(PROJECT_VERSION_SUFFIX "alpha")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WARN_UNINITIALIZED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CTest)

option(STCC_USE_SYSTEM_DEPENDENCIES
        "Use compatible dependencies installed on the system"
        ON
)

# add dependencies
include(Dependencies) # dependiencies
include(CodeGen) # codegen

# set global vars
set(STCC_INC_DIR "${PROJECT_SOURCE_DIR}/include" CACHE INTERNAL "STCC include directory")
set(STCC_PUB_INC_DIR "${STCC_INC_DIR}/stcc" CACHE INTERNAL "STCC public include directory")

# project source code
add_subdirectory(src/utils)
add_subdirectory(src/source)
add_subdirectory(src/lexer)
add_subdirectory(src)

# enable tests
if (BUILD_TESTING)
    add_subdirectory(unitest)
endif ()
