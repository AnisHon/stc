include(FetchContent)

# 优先在系统中查找已安装的依赖包，如果找不到则自动从 Git 仓库下载并构建
# Arguments:
#   NAME            - 可选，内部目标名称，用于 FetchContent 存储和后续链接，默认为fetch_${PACKAGE_NAME}
#   PACKAGE_NAME    - CMake 包名称，用于 find_package 查找
#   PACKAGE_VERSION - 可选，要求的版本号
#   GIT_REPOSITORY  - Git 仓库 URL，当系统中找不到时使用
#   GIT_TAG         - Git 标签/分支/commit，指定要拉取的版本
#   TARGET_NAME     - 可选，导出的 target 名称（默认为 ${NAME}）
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

    # 参数校验
    if (NOT ARG_PACKAGE_NAME)
        message(FATAL_ERROR "find_or_fetch: PACKAGE_NAME is required")
    endif ()
    if (NOT ARG_GIT_REPOSITORY)
        message(FATAL_ERROR "find_or_fetch: GIT_REPOSITORY is required (for fallback)")
    endif ()
    if (NOT ARG_GIT_TAG)
        message(FATAL_ERROR "find_or_fetch: GIT_TAG is required (for fallback)")
    endif ()

    # 设置默认值
    if (NOT ARG_NAME)
        set(ARG_NAME "fetch_${ARG_PACKAGE_NAME}")
    endif ()
    if (NOT ARG_TARGET_NAME)
        set(ARG_TARGET_NAME "${ARG_PACKAGE_NAME}")
    endif ()

    # 尝试查找系统包
    find_package(${ARG_PACKAGE_NAME} ${ARG_PACKAGE_VERSION} CONFIG QUIET)

    if (${ARG_PACKAGE_NAME}_FOUND)
        message(STATUS "Found system ${ARG_PACKAGE_NAME} ${${ARG_PACKAGE_NAME}_VERSION}")
    else ()
        message(STATUS "Not found ${ARG_PACKAGE_NAME} ${ARG_PACKAGE_VERSION}, fetching ${ARG_NAME}:${ARG_GIT_TAG} from source...")

        FetchContent_Declare(
                ${ARG_NAME}
                GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
                GIT_TAG ${ARG_GIT_TAG}
        )

        FetchContent_MakeAvailable(${ARG_NAME})

        # 验证是否真的填充成功
        if (${ARG_NAME}_POPULATED AND EXISTS ${${ARG_NAME}_SOURCE_DIR})
            message(STATUS "    ✓ ${ARG_NAME}@${ARG_GIT_TAG} fetched to ${${ARG_NAME}_SOURCE_DIR}")
        else ()
            message(FATAL_ERROR "    ✗ Failed to fetch ${ARG_NAME}")
        endif ()
    endif ()

    # 导出变量供调用者使用
    set(${ARG_NAME}_FOUND ${${ARG_PACKAGE_NAME}_FOUND} PARENT_SCOPE)
    set(${ARG_NAME}_SOURCE_DIR ${${ARG_NAME}_SOURCE_DIR} PARENT_SCOPE)
    set(${ARG_NAME}_TARGET ${ARG_TARGET_NAME} PARENT_SCOPE)
endfunction()

# fetch_hdr 表示 fetch_header 获取那些 header-only 且不支持 cmake 的库
#
# 参数说明：
#   PACKAGE_NAME    - 包名（用于 find_package 和 target 命名）
#   GIT_REPOSITORY  - Git 仓库地址
#   GIT_TAG         - Git tag/branch/commit
#   INCLUDE_SUBDIR  - 可选，头文件相对于仓库根目录的子路径（默认为空，即仓库根目录）
#   TARGET_NAME     - 可选，自定义 target 名称（默认为 ${PACKAGE_NAME}::${PACKAGE_NAME}）
#
function(fetch_hdr)
    set(options)
    set(oneValueArgs
            PACKAGE_NAME
            GIT_REPOSITORY
            GIT_TAG
            INCLUDE_SUBDIR
            TARGET_NAME
    )
    cmake_parse_arguments(
            ARG
            "${options}"
            "${oneValueArgs}"
            ""
            ${ARGV}
    )

    # 参数验证
    if (NOT ARG_PACKAGE_NAME)
        message(FATAL_ERROR "fetch_headeronly_library: PACKAGE_NAME is required")
    endif ()
    if (NOT ARG_GIT_REPOSITORY)
        message(FATAL_ERROR "fetch_headeronly_library: GIT_REPOSITORY is required")
    endif ()
    if (NOT ARG_GIT_TAG)
        message(FATAL_ERROR "fetch_headeronly_library: GIT_TAG is required")
    endif ()

    # 设置默认值
    if (NOT ARG_INCLUDE_SUBDIR)
        set(ARG_INCLUDE_SUBDIR "")
    endif ()

    if (NOT ARG_TARGET_NAME)
        set(ARG_TARGET_NAME "${ARG_PACKAGE_NAME}::${ARG_PACKAGE_NAME}")
    else ()
        # 如果用户指定了 TARGET_NAME，确保包含双冒号格式
        if (NOT ARG_TARGET_NAME MATCHES "::")
            set(ARG_TARGET_NAME "${ARG_TARGET_NAME}::${ARG_TARGET_NAME}")
        endif ()
    endif ()

    # 提取包名中的命名空间和组件名（用于 find_package 的变量）
    # 例如 "utf8cpp" 对应的变量是 utf8cpp_FOUND
    string(TOUPPER ${ARG_PACKAGE_NAME} PACKAGE_NAME_UPPER)

    # 尝试 find_package config 模式（库如果提供了 CMake 支持）
    find_package(${ARG_PACKAGE_NAME} CONFIG QUIET)

    if (${ARG_PACKAGE_NAME}_FOUND)
        message(STATUS "Found system ${ARG_PACKAGE_NAME}")
    else ()
        # 系统没找到，从源码获取
        message(STATUS "Not found ${ARG_PACKAGE_NAME}, fetching ${ARG_PACKAGE_NAME}:${ARG_GIT_TAG} from source...")

        # 生成唯一的内部名称（避免重复 fetch）
        set(FETCH_NAME "fetch_${ARG_PACKAGE_NAME}")

        FetchContent_Declare(
                ${FETCH_NAME}
                GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
                GIT_TAG ${ARG_GIT_TAG}
        )

        FetchContent_MakeAvailable(${FETCH_NAME})

        # 验证是否真的 fetch 成功
        if (NOT ${FETCH_NAME}_POPULATED)
            message(FATAL_ERROR "Failed to fetch ${ARG_PACKAGE_NAME}")
        endif ()

        # 构建 include 路径
        set(INCLUDE_PATH "${${FETCH_NAME}_SOURCE_DIR}")
        if (ARG_INCLUDE_SUBDIR)
            set(INCLUDE_PATH "${INCLUDE_PATH}/${ARG_INCLUDE_SUBDIR}")
        endif ()

        # 检查头文件是否存在（验证 fetch 的内容是否正确）
        if (NOT EXISTS ${INCLUDE_PATH})
            message(FATAL_ERROR "fetch_headeronly_library: include path not found: ${INCLUDE_PATH}")
        endif ()

        # 创建 interface target
        string(REPLACE "::" ";" TARGET_PARTS ${ARG_TARGET_NAME})
        list(GET TARGET_PARTS 0 TARGET_NAMESPACE)
        list(GET TARGET_PARTS 1 TARGET_NAME)

        add_library(${TARGET_NAMESPACE}::${TARGET_NAME} INTERFACE IMPORTED)
        target_include_directories(${TARGET_NAMESPACE}::${TARGET_NAME} INTERFACE ${INCLUDE_PATH})

        message(STATUS "    ✓ ${ARG_PACKAGE_NAME} fetched to ${${FETCH_NAME}_SOURCE_DIR}")
    endif ()

    # 导出变量供调用者使用（可选）
    set(${ARG_PACKAGE_NAME}_FOUND TRUE PARENT_SCOPE)
    set(${ARG_PACKAGE_NAME}_TARGET ${ARG_TARGET_NAME} PARENT_SCOPE)
endfunction()