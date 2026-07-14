include(FetchContent)

# Dependencies:  fmt cpptrace utf8cpp catch2

if (STC_USE_SYSTEM_DEPENDENCIES)
    find_package(fmt 12.1.0 CONFIG QUIET)
endif ()
if (fmt_FOUND)
    message(STATUS "Found system fmt ${fmt_VERSION}")
else ()
    message(STATUS "Using fetched fmt v12.1.0")
    FetchContent_Declare(fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG 12.1.0
    )
    FetchContent_MakeAvailable(fmt)
endif ()

if (STC_USE_SYSTEM_DEPENDENCIES)
    find_package(cpptrace 1.0.4 CONFIG QUIET)
endif ()
if (cpptrace_FOUND)
    message(STATUS "Found system cpptrace ${cpptrace_VERSION}")
else ()
    message(STATUS "Using fetched cpptrace v1.0.4")
    FetchContent_Declare(cpptrace
            GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
            GIT_TAG v1.0.4
    )
    FetchContent_MakeAvailable(cpptrace)
endif ()

if (STC_USE_SYSTEM_DEPENDENCIES)
    find_package(utf8cpp 4.1.1 CONFIG QUIET)
endif ()
if (utf8cpp_FOUND)
    message(STATUS "Found system utf8cpp ${utf8cpp_VERSION}")
else ()
    message(STATUS "Using fetched utf8cpp v4.1.1")
    FetchContent_Declare(utf8cpp
            GIT_REPOSITORY https://github.com/nemtrif/utfcpp.git
            GIT_TAG v4.1.1
    )
    FetchContent_MakeAvailable(utf8cpp)
    add_library(utf8cpp::utf8cpp ALIAS utf8cpp)
endif ()

if (BUILD_TESTING)
    if (STC_USE_SYSTEM_DEPENDENCIES)
        find_package(Catch2 3.15.0 CONFIG QUIET)
    endif ()
    if (Catch2_FOUND)
        message(STATUS "Found system Catch2 ${Catch2_VERSION}")
    else ()
        message(STATUS "Using fetched Catch2 v3.15.0")
        FetchContent_Declare(Catch2
                GIT_REPOSITORY https://github.com/catchorg/Catch2.git
                GIT_TAG v3.15.0
        )
        FetchContent_MakeAvailable(Catch2)
    endif ()
endif ()

add_library(stc_common_deps INTERFACE)
target_link_libraries(stc_common_deps INTERFACE
        fmt::fmt
        cpptrace::cpptrace
)