include(FetchContent)

# Dependencies:  fmt cpptrace utf8cpp catch2

fetchcontent_declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 12.1.0
    GIT_SHALLOW TRUE
    SYSTEM
)
fetchcontent_makeavailable(fmt)

fetchcontent_declare(
    cpptrace
    GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
    GIT_TAG v1.0.4
    GIT_SHALLOW TRUE
    SYSTEM
)
fetchcontent_makeavailable(cpptrace)

fetchcontent_declare(
    utf8cpp
    GIT_REPOSITORY https://github.com/nemtrif/utfcpp.git
    GIT_TAG v4.1.1
    GIT_SHALLOW TRUE
    SYSTEM
)
fetchcontent_makeavailable(utf8cpp)
if(TARGET utf8cpp AND NOT TARGET utf8cpp::utf8cpp) # 补丁：防御性创建别名 Target，防止依赖找不到名称
    add_library(utf8cpp::utf8cpp ALIAS utf8cpp)
endif()

fetchcontent_declare(GSL GIT_REPOSITORY https://github.com/microsoft/GSL.git GIT_TAG v4.2.0)
fetchcontent_makeavailable(GSL)

if(BUILD_TESTING)
    message(STATUS "Using fetched Catch2 v3.15.0")
    fetchcontent_declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.15.0
        GIT_SHALLOW TRUE
        SYSTEM
    )
    fetchcontent_makeavailable(Catch2)
endif()
