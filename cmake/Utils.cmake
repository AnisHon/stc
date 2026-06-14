include(FetchContent)

# FetchContent_Declare 和 FetchContent_MakeAvailable 的组合
# Arguments:
#   NAME            - 用于 FetchContent 存储和后续链接
#   GIT_REPOSITORY  - Git 仓库 URL
#   GIT_TAG         - Git 标签/分支/commit，指定要拉取的版本
function(fetch_and_declare)
    set(options)
    set(oneValueArgs
        NAME
        GIT_REPOSITORY
        GIT_TAG
    )

    cmake_parse_arguments(
        ARG
        "${options}"
        "${oneValueArgs}"
        ""
        ${ARGV}
    )

    FetchContent_Declare(
        ${ARG_NAME}
        GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
        GIT_TAG ${ARG_GIT_TAG}
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(${ARG_NAME})
endfunction()


# 优先在系统中查找已安装的依赖包，如果找不到则自动从 Git 仓库下载并构建
# Arguments:
#   NAME            - 内部目标名称，用于 FetchContent 存储和后续链接
#   PACKAGE_NAME    - CMake 包名称，用于 find_package 查找
#   GIT_REPOSITORY  - Git 仓库 URL，当系统中找不到时使用
#   GIT_TAG         - Git 标签/分支/commit，指定要拉取的版本
function(find_or_fetch)
    set(options)
    set(oneValueArgs
        NAME
        PACKAGE_NAME
        PACKAGE_VERSION
        GIT_REPOSITORY
        GIT_TAG
        TARGET_NAME
    )

    cmake_parse_arguments(
        ARG
        "${options}"
        "${oneValueArgs}"
        ""
        ${ARGV}
    )

    if("${ARG_NAME}" STREQUAL "")
        message(FATAL_ERROR "NAME is empty")
    endif()

    if("${ARG_PACKAGE_NAME}" STREQUAL "")
        message(FATAL_ERROR "PACKAGE_NAME is empty")
    endif()

    if("${ARG_TARGET_NAME}" STREQUAL "")
        set(ARG_TARGET_NAME "${ARG_NAME}")
    endif()

    find_package(${ARG_PACKAGE_NAME} ${PACKAGE_VERSION} CONFIG QUIET)

    if(NOT ${ARG_PACKAGE_NAME}_FOUND)
        message(STATUS "Not found ${ARG_PACKAGE_NAME}, fetching ${ARG_NAME}:${ARG_GIT_TAG} from source...")

        fetch_and_declare(
            NAME ${ARG_NAME}
            GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
            GIT_TAG ${ARG_GIT_TAG}
        )
    else()
        message(STATUS "Found system ${ARG_PACKAGE_NAME}")
    endif()

endfunction()


# utf8cpp 比较特殊，fetch后没有现成的utf8cpp的target可以用，所以单独写了一个工具
# Arguments:
#   GIT_TAG         - Git 标签/分支/commit，指定要拉取的版本
function(fetch_utf8cpp GIT_TAG)
    set(options)
    set(oneValueArgs
        PACKAGE_VERSION
        GIT_TAG
    )
    cmake_parse_arguments(
        ARG
        "${options}"
        "${oneValueArgs}"
        ""
        ${ARGV}
    )

    set(ARG_PACKAGE_NAME utf8cpp)
    set(ARG_NAME utf8cpp)
    set(ARG_GIT_REPOSITORY https://github.com/nemtrif/utfcpp.git)

    find_package(${ARG_PACKAGE_NAME} ${PACKAGE_VERSION} CONFIG QUIET)
    if(NOT ${ARG_PACKAGE_NAME}_FOUND)
        message(STATUS "Not found ${ARG_PACKAGE_NAME}, fetching ${ARG_NAME}:${ARG_GIT_TAG} from source...")

        fetch_and_declare(
            NAME ${ARG_NAME}
            GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
            GIT_TAG ${ARG_GIT_TAG}
        )
        add_library(utf8cpp::utf8cpp INTERFACE IMPORTED)
        target_include_directories(utf8cpp::utf8cpp INTERFACE "${utf8cpp_SOURCE_DIR}/source")
    else()
        message(STATUS "Found system ${ARG_PACKAGE_NAME}")
    endif()

endfunction()