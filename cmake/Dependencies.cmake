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

# 选择性启用 catch2
if (ENABLE_TESTS)
    find_or_fetch(
        NAME catch2
        PACKAGE_NAME catch2
        PACKAGE_VERSION 3

        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.15.0
    )

endif ()

