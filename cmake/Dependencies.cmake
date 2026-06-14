include(Utils)

# Dependencies: utf8cpp fmt catch2 spdlog

fetch_utf8cpp(GIT_TAG v4.1.1)

find_or_fetch(
        NAME fmt
        PACKAGE_NAME fmt

        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 12.1.0
)

find_or_fetch(
        NAME spdlog
        PACKAGE_NAME spdlog

        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.17.0
)

find_or_fetch(
        NAME expected
        PACKAGE_NAME expected

        GIT_REPOSITORY https://github.com/TartanLlama/expected.git
        GIT_TAG v1.3.1
)

# 选择性启用 catch2
if (ENABLE_TESTS)
    find_or_fetch(
            NAME catch2
            PACKAGE_NAME Catch2
            PACKAGE_VERSION 3

            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.15.0
    )

    if (TARGET Catch2::Catch2WithMain)
        message(STATUS "Catch2::Catch2WithMain exists")
    else ()
        message(STATUS "Catch2::Catch2WithMain NOT exists")
    endif ()
endif ()

