cmake_minimum_required(VERSION 3.25)

project(stcc VERSION 11.45.14)
set(PROJECT_VERSION_SUFFIX "alpha")

add_library(stcc_cxx23 INTERFACE)
add_library(stcc::cxx23 ALIAS stcc_cxx23)
target_compile_features(stcc_cxx23 INTERFACE cxx_std_23)

set(CMAKE_WARN_UNINITIALIZED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CTest)

# add dependencies
include(Dependencies) # dependencies
include(CodeGen) # codegen

# set global vars
set(STCC_INC_DIR "${PROJECT_SOURCE_DIR}/include" CACHE INTERNAL "STCC include directory")
set(STCC_PUB_INC_DIR "${STCC_INC_DIR}/stcc" CACHE INTERNAL "STCC public include directory")

add_subdirectory(src)

# enable tests
if (BUILD_TESTING)
    add_subdirectory(unitest)
endif ()
