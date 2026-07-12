cmake_minimum_required(VERSION 3.20)

project(stc VERSION 11.45.14)
set(PROJECT_VERSION_SUFFIX "alpha")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WARN_UNINITIALIZED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CTest)

# add dependencies
include(Dependencies)

# init python
include(PyCodegen)

# set global vars
set(STC_INC_DIR "${CMAKE_SOURCE_DIR}/include" CACHE INTERNAL "STC include directory")
set(STC_PUB_INC_DIR "${STC_INC_DIR}/stc" CACHE INTERNAL "STC public include directory")

# project source code
add_subdirectory(src/utils)
add_subdirectory(src/source)
add_subdirectory(src/lexer)
add_subdirectory(src)

# enable tests
if (BUILD_TESTING)
    add_subdirectory(test)
endif ()
