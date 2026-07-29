# 使用uv包管理，检测包
find_program(UV_EXECUTABLE uv REQUIRED)

# 生成 version.h
function(target_generate_version_header TARGET_NAME)
    set(TEMPLATE_FILE "${CMAKE_SOURCE_DIR}/templates/version.h.in")
    set(GEN_HEADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/include")
    set(GEN_HEADER "${GEN_HEADER_DIR}/stc/version.h")

    configure_file(
        "${TEMPLATE_FILE}"
        "${GEN_HEADER}"
        @ONLY
    )

    target_include_directories(${TARGET_NAME} PUBLIC
        $<BUILD_INTERFACE:${GEN_HEADER_DIR}>
    )
endfunction()

# 生成 unicode_props 代码 (.cpp 和 .h)
function(target_generate_unicode_props TARGET_NAME)
    set(SCRIPT_FILE    "${CMAKE_SOURCE_DIR}/script/unicode_gen.py")
    set(RESOURCE_FILE  "${CMAKE_SOURCE_DIR}/resources/DerivedCoreProperties.txt")
    set(TPL_SOURCE     "${CMAKE_SOURCE_DIR}/templates/unicode_props.cpp.jinja")
    set(TPL_HEADER     "${CMAKE_SOURCE_DIR}/templates/unicode_props.h.jinja")

    set(GEN_DIR        "${CMAKE_CURRENT_BINARY_DIR}/generated")
    set(OUT_CPP        "${GEN_DIR}/unicode_props.cpp")
    set(OUT_H          "${GEN_DIR}/unicode_props.h")

    # 注册增量构建规则
    add_custom_command(
        OUTPUT "${OUT_CPP}" "${OUT_H}"
        COMMAND ${UV_EXECUTABLE} run "${SCRIPT_FILE}" "${RESOURCE_FILE}"
                --output_source "${OUT_CPP}"
                --output_header "${OUT_H}"
                --template_source "${TPL_SOURCE}"
                --template_header "${TPL_HEADER}"
                --include_path "unicode_props.h"
                --namespace "stc::utils::gen"
        DEPENDS
                "${SCRIPT_FILE}"
                "${RESOURCE_FILE}"
                "${TPL_SOURCE}"
                "${TPL_HEADER}"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "[CodeGen] Generating unicode_props code for ${TARGET_NAME} via uv..."
    )

    target_sources(${TARGET_NAME} PRIVATE "${OUT_CPP}")
    target_include_directories(${TARGET_NAME} PRIVATE "${GEN_DIR}")
endfunction()


# 生成 token 宏定义文件 (.inc)
function(target_generate_token_inc TARGET_NAME)
    set(SCRIPT_FILE    "${CMAKE_SOURCE_DIR}/script/token_gen.py")
    set(RESOURCE_FILE  "${CMAKE_SOURCE_DIR}/resources/token_def.txt")
    set(TPL_FILE       "${CMAKE_SOURCE_DIR}/templates/token_kinds.inc.jinja")

    set(GEN_DIR        "${CMAKE_CURRENT_BINARY_DIR}/generated")
    set(OUT_DIR        "${GEN_DIR}/stc/lexer")
    set(OUT_INC        "${OUT_DIR}/token_kinds.inc")

    add_custom_command(
        OUTPUT "${OUT_INC}"
        COMMAND ${UV_EXECUTABLE} run "${SCRIPT_FILE}" "${RESOURCE_FILE}"
                --template "${TPL_FILE}"
                -o "${OUT_INC}"
        DEPENDS
                "${SCRIPT_FILE}"
                "${RESOURCE_FILE}"
                "${TPL_FILE}"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "[CodeGen] Generating token_kinds.inc for ${TARGET_NAME} via uv..."
    )

    target_sources(${TARGET_NAME} PUBLIC "${OUT_INC}")
    target_include_directories(${TARGET_NAME} PUBLIC "${GEN_DIR}")
endfunction()

