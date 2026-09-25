include(FetchContent)

# Dependencies:  fmt cpptrace utf8cpp catch2

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 12.1.0
        GIT_SHALLOW TRUE
        SYSTEM
)
FetchContent_MakeAvailable(fmt)


FetchContent_Declare(
        cpptrace
        GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
        GIT_TAG v1.0.4
        GIT_SHALLOW TRUE
        SYSTEM
)
FetchContent_MakeAvailable(cpptrace)


FetchContent_Declare(
        utf8cpp
        GIT_REPOSITORY https://github.com/nemtrif/utfcpp.git
        GIT_TAG v4.1.1
        GIT_SHALLOW TRUE
        SYSTEM
)
FetchContent_MakeAvailable(utf8cpp)
if (TARGET utf8cpp AND NOT TARGET utf8cpp::utf8cpp) # 补丁：防御性创建别名 Target，防止依赖找不到名称
    add_library(utf8cpp::utf8cpp ALIAS utf8cpp)
endif ()


if (BUILD_TESTING)
    message(STATUS "Using fetched Catch2 v3.15.0")
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.15.0
            GIT_SHALLOW TRUE
            SYSTEM
    )
    FetchContent_MakeAvailable(Catch2)
endif ()

add_library(stc_common_deps INTERFACE)
target_link_libraries(stc_common_deps INTERFACE
        fmt::fmt
        cpptrace::cpptrace
)
