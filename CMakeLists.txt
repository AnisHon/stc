cmake_minimum_required(VERSION 4.2)
project(stc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WARN_UNINITIALIZED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(fmt REQUIRED)
find_package(utf8cpp REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)