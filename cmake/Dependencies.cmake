include(FetchContent)

# Dependencies:  fmt cpptrace spdlog utf8cpp catch2

find_package(fmt CONFIG QUIET)
if (fmt_FOUND)
    message(STATUS "Found system fmt ${fmt_VERSION}")
else ()
    message(STATUS "System fmt not found, fetching v12.1.0")
    FetchContent_Declare(fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG 12.1.0
    )
    FetchContent_MakeAvailable(fmt)
endif ()

find_package(cpptrace CONFIG QUIET)
if (cpptrace_FOUND)
    message(STATUS "Found system cpptrace ${cpptrace_VERSION}")
else ()
    message(STATUS "System cpptrace not found, fetching v1.0.4")
    FetchContent_Declare(cpptrace
            GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
            GIT_TAG v1.0.4
    )
    FetchContent_MakeAvailable(cpptrace)
endif ()

find_package(spdlog CONFIG QUIET)
if (spdlog_FOUND)
    message(STATUS "Found system spdlog ${spdlog_VERSION}")
else ()
    message(STATUS "System spdlog not found, fetching v1.17.0")
    FetchContent_Declare(spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG v1.17.0
    )
    FetchContent_MakeAvailable(spdlog)
endif ()

find_package(utf8cpp CONFIG QUIET)
if (utf8cpp_FOUND)
    message(STATUS "Found system utf8cpp ${utf8cpp_VERSION}")
else ()
    message(STATUS "System utf8cpp not found, fetching v4.1.1")
    FetchContent_Declare(utf8cpp
            GIT_REPOSITORY https://github.com/nemtrif/utfcpp.git
            GIT_TAG v4.1.1
    )
    FetchContent_MakeAvailable(utf8cpp)
endif ()

if (BUILD_TESTING)
    find_package(Catch2 3 CONFIG QUIET)
    if (Catch2_FOUND)
        message(STATUS "Found system Catch2 ${Catch2_VERSION}")
    else ()
        message(STATUS "System Catch2 3 not found, fetching v3.15.0")
        FetchContent_Declare(Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.15.0
        )
        FetchContent_MakeAvailable(Catch2)
    endif ()
endif ()

add_library(stc_dependencies INTERFACE)
target_link_libraries(stc_dependencies INTERFACE
        fmt::fmt
        utf8cpp::utf8cpp
        spdlog::spdlog
        cpptrace::cpptrace
)
